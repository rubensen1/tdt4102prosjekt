#include "neural.h"

#include <cmath>
#include <random>
#include <iostream>

static constexpr float PI_VALUE = 3.14159265358979323846f;

static float sigmoid(float h) {
    return 1.0f/(1.0f + (std::exp(-h)));
}

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

void mutate(float mutationRate, float mutationStrength) {
    
}

std::vector<float> NeuralNetwork::getOutput(const std::vector<float>& inputs) const {
    // Hidden layer
    std::vector<float> hidden(hiddenSize, 0.0f);
    for (int j = 0; j < hiddenSize; j++) {
        float sum = biasHidden[j];
        for (int i = 0; i < inputSize; i++) {
            sum += inputs[i] * weightsInput[i][j];
        }
        hidden[j] = sigmoid(sum);  // sigmoid er allerede definert i filen
    }

    // Output layer
    std::vector<float> output(outputSize, 0.0f);
    for (int k = 0; k < outputSize; k++) {
        float sum = biasOutput[k];
        for (int j = 0; j < hiddenSize; j++) {
            sum += hidden[j] * weightsHidden[j][k];
        }
        output[k] = std::tanh(sum);  // tanh gir verdier i [-1, 1], bra for throttle/steering
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
