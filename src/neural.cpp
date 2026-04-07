#include "neural.h"
#include "raylib.h"

#include <cmath>
#include <random>
#include <iostream>
#include <algorithm>
#include <string>

static constexpr float PI_VALUE = 3.14159265408979323846f;

static float sigmoid(float h) {
    return 1.0f/(1.0f + (std::exp(-h)));
}

static void drawThingy(std::vector<float> output) {
    
}

// static Color WeightToColorRB(float weight) {
//     Color newColor = {255,255,255,255};
//     if (weight < 0) {
//         weight = -weight;
//         newColor.r = 255-weight*255/5;
//         newColor.g = 255-weight*255/5;
        
//     } else {
//         newColor.b = 255-weight*255/5;
//         newColor.g = 255-weight*255/5;
//     }

//     return newColor;
// }

static Color WeightToColorRB(float weight) {
    float t = std::clamp(std::abs(weight)/5.0f, 0.0f, 1.0f);
    uint8_t lineærFargeGradient = static_cast<uint8_t>(255-255*t);

    if (weight > 0) {
        return {255, lineærFargeGradient, lineærFargeGradient, 255};
    } else {
        return {lineærFargeGradient, lineærFargeGradient, 255, 255};
    }
}

std::mt19937 NeuralNetwork::gen(std::random_device{}());

NeuralNetwork::NeuralNetwork(int inputSize, int hiddenSize, int outputSize)
  : inputSize(inputSize),
    hiddenSize(hiddenSize),
    outputSize(outputSize),
    weightsInput(inputSize, std::vector<float>(hiddenSize)),
    weightsHidden(hiddenSize, std::vector<float>(outputSize)),
    biasHidden(hiddenSize),
    biasOutput(outputSize)

    {
        fillRandomly();
    }

void NeuralNetwork::mutate(float mutationRate, float mutationStrength, int generation) {
    mutationRate*=std::exp(-generation/70.0f);
    mutationStrength*=std::exp(-generation/70.0f);
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
    // Hidden layer
    std::vector<float> hidden(hiddenSize, 0.0f);
    for (int j = 0; j < hiddenSize; j++) {
        float sum = biasHidden[j];
        for (int i = 0; i < inputSize; i++) {
            sum += inputs[i] * weightsInput[i][j];
        }
        hidden[j] = sigmoid(sum);
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
    std::uniform_real_distribution<float> dist(-1.0f, 1.0f);
    
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

void NeuralNetwork::draw(const std::vector<float>& inputs) {
    //første biten, med vektene
    const float xInput  = 870;
    const float xHidden = 1120;
    const float xOutput = 1370;
    const float yTop = 150;
    const float yBot = 550;
    const std::vector<std::string> inputsTypes= {"-75.0° sensor (0,1)", "-40.0° sensor(0,1)", "-15.0° sensor(0,1)", "0.0° sensor(0,1)", "15.0° sensor(0,1)", "40.0° sensor(0,1)", "75.0° sensor(0,1)", "Speed (-1,1)", "Drift Offset (-1,1)"};
    const std::vector<std::string> OutputTypes= {"Throttle (-1,1)", "Turn power (-1,1)"};
    
    for (int i = 0; i < inputSize; i++) {
        for (int y = 0; y < hiddenSize; y++) {
            DrawLineEx(Vector2(xInput, yTop+40*i), Vector2(xHidden, yTop+40*y), 2.0f, WeightToColorRB(weightsInput[i][y]));
        }
    }

    for (int i = 0; i < hiddenSize; i++) {
        for (int y = 0; y < outputSize; y++) {
            DrawLineEx(Vector2(xHidden, yTop+40*i), Vector2(xOutput, yTop+160*y), 2.0f, WeightToColorRB(weightsHidden[i][y]));
        }
    }

    for (int i = 0; i < inputSize; i++) {
        DrawCircleV(Vector2(xInput, yTop+40*i), 12.0f, PURPLE);
        DrawText(inputsTypes[i].c_str() , 700, yTop+40*i-9, 17, BLACK);
    }
    
    for (int i = 0; i < hiddenSize; i++) {
        DrawCircleV(Vector2(xHidden, yTop+40*i), 12.0f, WeightToColorRB(biasHidden[i]));
    }
    
    for (int i = 0; i < outputSize; i++) {
        DrawCircleV(Vector2(xOutput, yTop+160*i), 12.0f, WeightToColorRB(biasOutput[i]));
        DrawText(OutputTypes[i].c_str() , 1400, yTop+160*i-9, 17, BLACK);
    }


    //inputs lalalala
    for (int i = 0; i<inputSize;i++) {
        DrawCircleV(Vector2(xInput, yBot+40*i), 12.0f, WeightToColorRB(inputs[i]));
        DrawText(TextFormat("%.3f", inputs[i]) , 700, yBot+40*i-9, 17, BLACK);
    }
    
    
    std::vector<float> hidden(hiddenSize, 0.0f);
    
    for (int j = 0; j < hiddenSize; j++) {
        float sum = biasHidden[j];
        for (int i = 0; i < inputSize; i++) {
            sum += inputs[i] * weightsInput[i][j];
            DrawLineEx(Vector2(xInput, yBot+40*i), Vector2(xHidden, yBot+40*j), 2.0f, WeightToColorRB(inputs[i] * weightsInput[i][j]));
        }
        hidden[j] = sigmoid(sum);
    }
    
    for (int i = 0; i<hiddenSize;i++) {
        DrawCircleV(Vector2(xHidden, yBot+40*i), 12.0f, WeightToColorRB(hidden[i]));
    }
    
    
    std::vector<float> output(outputSize, 0.0f);
    
    for (int k = 0; k < outputSize; k++) {
        float sum = biasOutput[k];
        for (int j = 0; j < hiddenSize; j++) {
            sum += hidden[j] * weightsHidden[j][k];
            DrawLineEx(Vector2(xHidden, yBot+40*j), Vector2(xOutput, yBot+160*k), 2.0f, WeightToColorRB(hidden[j] * weightsHidden[j][k]));
        }
        output[k] = std::tanh(sum);
    }
    
    for (int i = 0; i<outputSize;i++) {
        DrawCircleV(Vector2(xOutput, yBot+160*i), 12.0f, WeightToColorRB(output[i]));
        DrawText(TextFormat("%.3f", output[i]) , 1400, yBot+160*i-9, 17, BLACK);
    }
}
