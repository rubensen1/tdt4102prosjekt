#include "neural.h"
#include "AnimationWindow.h"

#include <cmath>
#include <fstream>
#include <random>
#include <algorithm>
#include <string>
#include <iostream>

static TDT4102::Color weightToColor(float weight) {
    float t = std::pow(std::abs(weight / 5.0f), 0.7f);
    int c = static_cast<int>(255.0f - 255.0f * std::clamp(t, 0.0f, 1.0f));
    if (weight > 0)
        return TDT4102::Color(255, c, c);   // rød
    else
        return TDT4102::Color(c, c, 255);   // blå
}

static TDT4102::Color valueToColor(float value) {
    float t = std::clamp(std::abs(value), 0.0f, 1.0f);
    int c = static_cast<int>(255.0f - 255.0f*t);
    if (value > 0)
        return TDT4102::Color(255, c, c);
    else
        return TDT4102::Color(c, c, 255);
}

// Tegner en liten "kontrollpanel" for gass og sving
static void drawThingy(TDT4102::AnimationWindow& window,
                       const std::vector<float>& output,
                       float x, float y) {
    // Gass-søyle (vertikal)
    window.draw_rectangle({static_cast<int>(x+150), static_cast<int>(y-45)},
                           10, 100, TDT4102::Color::white);
    // Sving-søyle (horisontal)
    window.draw_rectangle({static_cast<int>(x+105), static_cast<int>(y+154)},
                           100, 10, TDT4102::Color::white);

    // Gass-indikator
    window.draw_rectangle(
        {static_cast<int>(x+145), static_cast<int>(y - 45 - output[0]*50 + 50)},
        20, 5, weightToColor(output[0] * 5.0f));

    // Sving-indikator
    window.draw_rectangle(
        {static_cast<int>(x + 105 + output[1]*50 + 50), static_cast<int>(y+149)},
        5, 20, weightToColor(output[1] * 5.0f));
}

std::mt19937 NeuralNetwork::gen(std::random_device{}());

NeuralNetwork::NeuralNetwork(int inputSize, int hiddenSize, int outputSize)
    : inputSize(inputSize), hiddenSize(hiddenSize), outputSize(outputSize),
      weightsInput(inputSize, std::vector<float>(hiddenSize)),
      weightsHidden(hiddenSize, std::vector<float>(outputSize)),
      biasHidden(hiddenSize),
      biasOutput(outputSize) {
    fillRandomly();
}

void NeuralNetwork::mutate(float mutationRate, float mutationStrength, int generation) {
    mutationRate*=std::exp(-generation/60.0f);
    mutationStrength*=std::exp(-generation/60.0f);

    std::uniform_real_distribution<float> chanceDist(0.0f, 1.0f);
    std::uniform_real_distribution<float> deltaDist(-mutationStrength, mutationStrength);
    
    for (int i = 0; i < inputSize; i++) {
        for (int j = 0; j < hiddenSize; j++) {
            if (chanceDist(gen) < mutationRate) {
                weightsInput[i][j] = std::clamp(weightsInput[i][j] + deltaDist(gen), -5.0f, 5.0f);
            }
        }
    }
    
    for (int i = 0; i < hiddenSize; i++) {
        for (int j = 0; j < outputSize; j++) {
            if (chanceDist(gen) < mutationRate) {
                weightsHidden[i][j] = std::clamp(weightsHidden[i][j] + deltaDist(gen), -5.0f, 5.0f);
            }
        }
    }

    for (int i = 0; i < hiddenSize; i++) {
        if (chanceDist(gen) < mutationRate) {
            biasHidden[i] = std::clamp(biasHidden[i] + deltaDist(gen), -5.0f, 5.0f);
        }
    }

    for (int i = 0; i < outputSize; i++) {
        if (chanceDist(gen) < mutationRate) {
            biasOutput[i] = std::clamp(biasOutput[i] + deltaDist(gen), -5.0f, 5.0f);
        }
    }
}

std::vector<float> NeuralNetwork::getOutput(const std::vector<float>& inputs) const {
    std::vector<float> hidden(hiddenSize, 0.0f);
    for (int j = 0; j < hiddenSize; j++) {
        float sum = biasHidden[j];
        for (int i = 0; i < inputSize; i++) {
            sum += inputs[i] * weightsInput[i][j];
        }
        hidden[j] = tanh(sum);
    }

    std::vector<float> output(outputSize, 0.0f);
    for (int k = 0; k < outputSize; k++) {
        float sum = biasOutput[k];
        for (int j = 0; j < hiddenSize; j++) {
            sum += hidden[j] * weightsHidden[j][k];
        }
        output[k] = std::tanh(sum);  // tanh gir verdier i [-1, 1]
    }
    
    return output;
}

void NeuralNetwork::fillRandomly() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dist(-0.4f, 0.4f);
    
    for (int i = 0; i < inputSize; i++) {
        for (int j = 0; j < hiddenSize; j++) {
            weightsInput[i][j] = dist(gen);
        }
    }

    for (int i = 0; i < hiddenSize; i++) {
        for (int j = 0; j < outputSize; j++) {
            weightsHidden[i][j] = dist(gen);
        }
    }
    
    for (int i = 0; i < hiddenSize; i++) {
        biasHidden[i] = dist(gen);
    }
    
    for (int i = 0; i < outputSize; i++) {
        biasOutput[i] = dist(gen);
    }
}


void NeuralNetwork::draw(TDT4102::AnimationWindow& window, const std::vector<float>& inputs) {
    const int xInput  = 870;
    const int xHidden = 1120;
    const int xOutput = 1370;
    const int yTop    = 150;
    const int yBot    = 550;

    const std::vector<std::string> inputLabels= {"-75.0° sensor (0,1)", "-40.0° sensor(0,1)", "-15.0° sensor(0,1)", "0.0° sensor(0,1)", "15.0° sensor(0,1)", "40.0° sensor(0,1)", "75.0° sensor(0,1)", "Speed (-1,1)", "Drift Offset (-1,1)"};
    const std::vector<std::string> outputLabels= {"Throttle (-1,1)", "Turn power (-1,1)"};
  
    // I denne blokken har claude AI skrevet om alle raylibfunksjonene til animationwindow, fordi jeg er lat 
    for (int i = 0; i < inputSize; i++)
        for (int j = 0; j < hiddenSize; j++)
            window.draw_line({xInput, yTop+40*i}, {xHidden, yTop+40*j},
                             weightToColor(weightsInput[i][j]));

    for (int i = 0; i < hiddenSize; i++)
        for (int j = 0; j < outputSize; j++)
            window.draw_line({xHidden, yTop+40*i}, {xOutput, yTop+160*j},
                             weightToColor(weightsHidden[i][j]));

    for (int i = 0; i < inputSize; i++) {
        window.draw_circle({xInput, yTop+40*i}, 12, TDT4102::Color::purple);
        window.draw_text({700, yTop+40*i-9}, inputLabels[i], TDT4102::Color::black, 17);
    }
    for (int i = 0; i < hiddenSize; i++)
        window.draw_circle({xHidden, yTop+40*i}, 12, weightToColor(biasHidden[i]));
    for (int i = 0; i < outputSize; i++) {
        window.draw_circle({xOutput, yTop+160*i}, 12, weightToColor(biasOutput[i]));
        window.draw_text({1400, yTop+160*i-9}, outputLabels[i], TDT4102::Color::black, 17);
    }

    //inputs lalalala
    for (int i = 0; i < inputSize; i++) {
        window.draw_circle({xInput, yBot+40*i}, 12, valueToColor(inputs[i]));
        window.draw_text({700, yBot+40*i-9},
                         std::to_string(inputs[i]).substr(0, 5),
                         TDT4102::Color::black, 17);
    }

    std::vector<float> hidden(hiddenSize, 0.0f);
    for (int j = 0; j < hiddenSize; j++) {
        float sum = biasHidden[j];
        for (int i = 0; i < inputSize; i++) {
            window.draw_line({xInput, yBot+40*i}, {xHidden, yBot+40*j},
                             weightToColor(inputs[i] * weightsInput[i][j]));
            sum += inputs[i] * weightsInput[i][j];
        }
        hidden[j] = std::tanh(sum);
        window.draw_circle({xHidden, yBot+40*j}, 12, weightToColor(hidden[j]));
    }

    std::vector<float> output(outputSize, 0.0f);
    for (int k = 0; k < outputSize; k++) {
        float sum = biasOutput[k];
        for (int j = 0; j < hiddenSize; j++) {
            window.draw_line({xHidden, yBot+40*j}, {xOutput, yBot+160*k},
                             weightToColor(hidden[j] * weightsHidden[j][k]));
            sum += hidden[j] * weightsHidden[j][k];
        }
        output[k] = std::tanh(sum);
        window.draw_circle({xOutput, yBot+160*k}, 12, valueToColor(output[k]));
        window.draw_text({1400, yBot+160*k-9},
                         std::to_string(output[k]).substr(0, 5),
                         TDT4102::Color::black, 17);
    }

    drawThingy(window, output, xOutput, yBot);
}

void NeuralNetwork::saveToFile() {
    std::ofstream file("car.txt");

    file << inputSize << "\n";
    file << hiddenSize << "\n";
    file << outputSize << "\n";
    for (std::vector<float>& v : weightsInput) {
        for (float& w : v) {
            file << w << "\n";
        }
    }
    for (float& w : biasHidden) {
        file << w << "\n";
    }
    for (std::vector<float>& v : weightsHidden) {
        for (float& w : v) {
            file << w << "\n";
        }
    }
    for (float& w : biasOutput) {
        file << w << "\n";
    }

    std::cout<<"lalala"<<std::endl;
}

void NeuralNetwork::loadFromFIle() {
    try {
        std::ifstream file("car.txt");
        file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    
        std::string line;
        std::getline(file, line);
        inputSize = std::stoi(line);
        std::getline(file, line);
        hiddenSize = std::stoi(line);
        std::getline(file, line);
        outputSize = std::stoi(line);
        
        for (int i = 0; i < inputSize; i++) {
            for (int y = 0; y < hiddenSize; y++) {
                std::getline(file, line);
                weightsInput[i][y] = std::stof(line);
            }
        }
        for (int y = 0; y < hiddenSize; y++) {
            std::getline(file, line);
            biasHidden[y] = std::stof(line);
        }
        for (int i = 0; i < hiddenSize; i++) {
            for (int y = 0; y < outputSize; y++) {
                std::getline(file, line);
                weightsHidden[i][y] = std::stof(line);
            }
        }
        for (int y = 0; y < outputSize; y++) {
            std::getline(file, line);
            biasOutput[y] = std::stof(line);
        }
    
    
        file.close();

    } catch (const std::ios_base::failure& e) {
        std::cerr << "Feil i lesing av fila: " << e.what() << "\n";
    }
}