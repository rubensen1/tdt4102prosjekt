#include "neural.h"
#include "raylib.h"

#include <cmath>
#include <random>
#include <iostream>
#include <algorithm>

static constexpr float PI_VALUE = 3.14159265358979323846f;

static float sigmoid(float h) {
    return 1.0f/(1.0f + (std::exp(-h)));
}

static Color WeightToColorRB(float weight) {
    return RED;
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

void NeuralNetwork::mutate(float mutationRate, float mutationStrength) {
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

void NeuralNetwork::draw() {
    for (int i = 0; i < inputSize; i++) {
        DrawCircleV(Vector2(720, 150+30*i), 12.0f, WeightToColorRB(1));
    };
    // DrawLineEx(position, endPoint, 2.0f, color);
    // std::cout<<biasHidden[0]<<std::endl;
    // std::cout<<biasOutput[0]<<std::endl;

}
