#pragma once

#include "raylib.h"

class Car {
public:
    Car(Vector2 startPosition, float startHeadingDegrees);

    void update(float dt);
    void draw() const;

    void setInputs(float throttleAmount, float steeringAmount);
    void reset(Vector2 startPosition, float startHeadingDegrees);

    Vector2 getPosition() const;
    Vector2 getVelocity() const;
    float getHeadingDegrees() const;
    float getDriftOffsetDegrees() const;

private:
    Vector2 position;
    Vector2 velocity;

    float headingDegrees;
    float driftOffsetDegrees;

    float throttleInput;
    float steeringInput;

    float width;
    float height;

    float engineForce;
    float turnRate;
    float lateralGrip;
    float drag;

    Vector2 getForwardVector() const;
    Vector2 getRightVector() const;

    float dot(Vector2 a, Vector2 b) const;
    float length(Vector2 v) const;
    float normalizeAngleDegrees(float angle) const;
};