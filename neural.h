#pragma once

#include "AnimationWindow.h"
#include <vector>
#include <random>

class NeuralNetwork {
    static std::mt19937 gen;
public:
    NeuralNetwork(int inputSize, int hiddenSize, int outputSize);

    std::vector<float> getOutput(const std::vector<float>& inputs) const;
    void fillRandomly();
    void mutate(float mutationRate, float mutationStrength, int generation);

    void draw(TDT4102::AnimationWindow& window, const std::vector<float>& inputs);

    void saveToFile();
    void loadFromFIle();

private:
    int inputSize;
    int hiddenSize;
    int outputSize;

    std::vector<std::vector<float>> weightsInput;
    std::vector<std::vector<float>> weightsHidden;
    std::vector<float> biasHidden;
    std::vector<float> biasOutput;
};
