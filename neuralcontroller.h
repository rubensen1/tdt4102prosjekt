#pragma once
#include "controller.h"
#include "neural.h"

class NeuralController : public Controller {
private:
    NeuralNetwork nn;

public:
    NeuralController(int in, int hid, int out)
        : nn(in, hid, out) {}

    NeuralController(const NeuralNetwork& brain)
        : nn(brain) {}

    std::vector<float> getOutput(const std::vector<float>& inputs) override {
        return nn.getOutput(inputs);
    }

    NeuralNetwork& getNN() {
        return nn;
    }
};