#pragma once

#include "raylib.h"
#include <vector>

class NeuralNetwork {
public:
    NeuralNetwork(int inputSize, int hiddenSize, int outputSize);

    // static NeuralNetwork createRandom(int inputSize, int hiddenSize, int outputSize);

    std::vector<float> getOutput(const std::vector<float>& inputs) const;

    void fillRandomly();

    void mutate(float mutationRate, float mutationStrength);

private:
    int inputSize;
    int hiddenSize;
    int outputSize;

    std::vector<std::vector<float>> weightsInput;
    std::vector<std::vector<float>> weightsHidden;
    std::vector<float> biasHidden;
    std::vector<float> biasOutput;
};