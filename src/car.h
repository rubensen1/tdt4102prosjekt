#pragma once

#include "raylib.h"
#include <vector>

class Car {
public:
    Car(Vector2 startPosition, float startHeadingDegrees);

    void update(float dt, const std::vector<Rectangle>& walls);
    void draw() const;

    void setInputs(float throttleAmount, float steeringAmount);
    // void reset(Vector2 startPosition, float startHeadingDegrees);

    Vector2 getPosition() const;
    Vector2 getVelocity() const;
    float getHeadingDegrees() const;
    float getDriftOffsetDegrees() const;
    bool getIsDead() const;
    float castRayToWalls(float angleOffsetDegrees, const std::vector<Rectangle>& walls, float maxDistance) const;

private:
    Vector2 position;
    Vector2 velocity;
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

    std::vector<float> sensorDegreeValues;
    std::vector<float> sensorDistances;
    float sensorMaxDistance;

    Vector2 getForwardVector() const;
    Vector2 getRightVector() const;

    float dot(Vector2 a, Vector2 b) const;
    float length(Vector2 v) const;
    float normalizeAngleDegrees(float angle) const;

    Vector2 rotateLocalPoint(Vector2 localPoint) const;
    void getCorners(Vector2 corners[4]) const;
    bool isCollidingWithWalls(const std::vector<Rectangle>& walls) const;
};