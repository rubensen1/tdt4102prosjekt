#pragma once

#include "raylib.h"
#include "neural.h"
#include <vector>

class Car {
public:
    Car(Vector2 startPosition, float startHeadingDegrees);

    void update(float dt, const std::vector<Rectangle>& walls, const std::vector<Rectangle>& checkpoints);
    void draw(const std::vector<Rectangle>& walls) const;

    void updateAI();
    void setOutputs(float throttleAmount, float steeringAmount);
    // void reset(Vector2 startPosition, float startHeadingDegrees);

    Vector2 getPosition() const;
    Vector2 getVelocity() const;
    float getHeadingDegrees() const;
    float getDriftOffsetDegrees() const;
    bool getIsDead() const;
    const std::vector<float>& getSensorDistances() const;
    NeuralNetwork bren;
    
private:
    Vector2 position;
    Vector2 velocity;
    float headingDegrees;
    float driftOffsetDegrees;
    float throttleInput;
    float steeringInput;
    bool isDead;

    std::vector<float> getInputs();
    
    float width;
    float height;
    float engineForce;
    float turnRate;
    float lateralGrip;
    float drag;
    float fitness;
    int currentCheckpointIndex;
    
    std::vector<float> sensorDegreeValues;
    float sensorMaxDistance;
    int sensorAmount;
    std::vector<float> sensorDistances;
    void drawSensor(int sensorNr, float angleOffsetDegrees,const std::vector<Rectangle>& walls,float maxDistance,Color color) const;
    float castRayToWalls(float angleOffsetDegrees, const std::vector<Rectangle>& walls, float maxDistance) const;

    Vector2 getForwardVector() const;
    Vector2 getRightVector() const;

    float dot(Vector2 a, Vector2 b) const;
    float length(Vector2 v) const;
    float normalizeAngleDegrees(float angle) const;

    Vector2 rotateLocalPoint(Vector2 localPoint) const;
    void getCorners(Vector2 corners[4]) const;
    bool isCollidingWithWalls(const std::vector<Rectangle>& walls) const;
};