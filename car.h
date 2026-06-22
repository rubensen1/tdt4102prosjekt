#pragma once

#include "AnimationWindow.h"
#include "neural.h"
#include "controller.h"
#include <vector>

// Enkel float-vektor for fysikk (TDT4102::Point bruker int, uegnet til fysikk)
struct Vec2 {   //CHAT-GENERERT I OVERGANGEN FRA RAYLIB TIL ANIMATIONWINDOW
    float x = 0.0f;
    float y = 0.0f;

    // Hjelpefunksjon for å konvertere til Point ved tegning
    TDT4102::Point toPoint() const {
        return {static_cast<int>(x), static_cast<int>(y)};
    }
};

// Enkel rektangel-struct (erstatter raylib Rectangle)
struct Rect {   //CHAT-GENERERT I OVERGANGEN FRA RAYLIB TIL ANIMATIONWINDOW
    float x, y, width, height;
};

// Erstatter raylib CheckCollisionPointRec
inline bool pointInRect(Vec2 p, Rect r) {       //CHAT-GENERERT I OVERGANGEN FRA RAYLIB TIL ANIMATIONWINDOW
    return p.x >= r.x && p.x <= r.x + r.width &&
           p.y >= r.y && p.y <= r.y + r.height;
}

class Car {
public:
    Car(Vec2 startPosition, float startHeadingDegrees);
    Car(Vec2 startPosition, float startHeadingDegrees, const NeuralNetwork& brain);

    void update(float dt, const std::vector<Rect>& walls, const std::vector<Rect>& checkpoints, float generation);
    void draw(TDT4102::AnimationWindow& window, const std::vector<Rect>& walls) const;

    void updateAI();
    void setOutputs(float throttleAmount, float steeringAmount);
    void reset(Vec2 startPosition, float startHeadingDegrees);

    bool getIsDead() const;
    const std::vector<float>& getSensorDistances() const;
    NeuralNetwork bren;

    float fitness;
    int leader;
    std::vector<float> getInputs();

    Controller* controller;

private:
    Vec2 position;
    Vec2 velocity;
    float headingDegrees;
    float driftOffsetDegrees;
    float throttleInput;
    float steeringInput;
    bool isDead;

    float width;
    float height;
    float engineForce;
    float turnRate;
    float lateralGrip;
    float drag;
    int currentCheckpointIndex;

    std::vector<float> sensorDegreeValues;
    float sensorMaxDistance;
    int sensorAmount;
    std::vector<float> sensorDistances;

    void drawSensor(TDT4102::AnimationWindow& window, int sensorNr, float angleOffsetDegrees, float maxDistance, TDT4102::Color color) const;
    float castRayToWalls(float angleOffsetDegrees, const std::vector<Rect>& walls, float maxDistance) const;

    Vec2 getForwardVector() const;
    Vec2 getRightVector() const;

    float dot(Vec2 a, Vec2 b) const;
    float length(Vec2 v) const;
    float normalizeAngleDegrees(float angle) const;

    Vec2 rotateLocalPoint(Vec2 localPoint) const;
    void getCorners(Vec2 corners[4]) const;
    bool isCollidingWithWalls(const std::vector<Rect>& walls) const;
};
