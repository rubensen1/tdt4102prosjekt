#pragma once
#include <vector>

class Controller {
public:
    virtual std::vector<float> getOutput(const std::vector<float>& inputs) {
        return {0.0f, 0.0f};
    }

    virtual ~Controller() = default;
};