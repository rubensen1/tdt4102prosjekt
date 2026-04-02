#pragma once

#include "raylib.h"
#include <vector>

class NeuralNetwork {
public:
    NeuralNetwork(int inputSize, int hiddenSize, int outputSize);

    // static NeuralNetwork createRandom(int inputSize, int hiddenSize, int outputSize);

    std::vector<float> getOutput(const std::vector<float>& inputs) const;

    void mutate(float mutationRate, float mutationStrength);

private:
    int inputSize;
    int hiddenSize;
    int outputSize;

    std::vector<std::vector<float>> weightsInputHidden;
    std::vector<std::vector<float>> weightsHiddenOutput;
    std::vector<float> biasHidden;
    std::vector<float> biasOutput;

    static float activation(float x);
};