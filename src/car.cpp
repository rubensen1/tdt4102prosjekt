#include "Car.h"

#include <cmath>

static constexpr float PI_VALUE = 3.14159265358979323846f;

Car::Car(Vector2 startPosition, float startHeadingDegrees)
    : position(startPosition),
      velocity{0.0f, 0.0f},
      headingDegrees(startHeadingDegrees),
      driftOffsetDegrees(0.0f),
      throttleInput(0.0f),
      steeringInput(0.0f),
      isDead(false),
      width(40.0f),
      height(20.0f),
      engineForce(450.0f),
      turnRate(150.0f),
      lateralGrip(3.0f),
      drag(0.80f) {}

void Car::update(float dt, const std::vector<Rectangle>& walls) {
    if (isDead) {
        return;
    }

    Vector2 forward = getForwardVector();
    Vector2 right = getRightVector();

    float speed = length(velocity);

    if (speed > 5.0f) {
        headingDegrees += steeringInput * turnRate * dt;
    }

    forward = getForwardVector();
    right = getRightVector();

    velocity.x += forward.x * throttleInput * engineForce * dt;
    velocity.y += forward.y * throttleInput * engineForce * dt;

    float forwardSpeed = dot(velocity, forward);
    float sideSpeed = dot(velocity, right);

    sideSpeed -= sideSpeed * lateralGrip * dt;
    forwardSpeed -= forwardSpeed * drag * dt;

    velocity.x = forward.x * forwardSpeed + right.x * sideSpeed;
    velocity.y = forward.y * forwardSpeed + right.y * sideSpeed;

    position.x += velocity.x * dt;
    position.y += velocity.y * dt;

    if (isCollidingWithWalls(walls)) {
        isDead = true;
        velocity = {0.0f, 0.0f};
        throttleInput = 0.0f;
        steeringInput = 0.0f;
        driftOffsetDegrees = 0.0f;
        return;
    }

    float newSpeed = length(velocity);
    if (newSpeed > 0.01f) {
        float velocityAngleRadians = std::atan2(velocity.y, velocity.x);
        float velocityAngleDegrees = velocityAngleRadians * 180.0f / PI_VALUE;
        driftOffsetDegrees =
            normalizeAngleDegrees(velocityAngleDegrees - headingDegrees);
    } else {
        driftOffsetDegrees = 0.0f;
    }
}

void Car::draw() const {
    Rectangle body = {
        position.x,
        position.y,
        width,
        height,
    };

    Vector2 origin = {
        width / 2.0f,
        height / 2.0f,
    };

    DrawRectanglePro(body, origin, headingDegrees, ORANGE);

    Vector2 forward = getForwardVector();
    Vector2 nose = {
        position.x + forward.x * (width * 0.5f),
        position.y + forward.y * (width * 0.5f),
    };
    // DrawLineEx(position, nose, 3.0f, DARKBLUE);

    if (length(velocity) > 0.01f) {
        Vector2 velocityDirection = {
            velocity.x / length(velocity),
            velocity.y / length(velocity),
        };

        Vector2 velocityEnd = {
            position.x + velocityDirection.x * 40.0f,
            position.y + velocityDirection.y * 40.0f,
        };

        DrawLineEx(position, velocityEnd, 2.0f, GREEN);
    }

    // For å tegne hitboxen
    // Vector2 corners[4];
    // getCorners(corners);

    // for (int i = 0; i < 4; i++) {
    //     DrawCircleV(corners[i], 4.0f, YELLOW);
    // }

    // for (int i = 0; i < 4; i++) {
    //     Vector2 a = corners[i];
    //     Vector2 b = corners[(i + 1) % 4];
    //     DrawLineEx(a, b, 2.0f, ORANGE);
    // }
}

void Car::setInputs(float throttleAmount, float steeringAmount) {
    if (isDead) {
        return;
    }

    throttleInput = throttleAmount;
    steeringInput = steeringAmount;
}

void Car::reset(Vector2 startPosition, float startHeadingDegrees) {
    position = startPosition;
    velocity = {0.0f, 0.0f};
    headingDegrees = startHeadingDegrees;
    driftOffsetDegrees = 0.0f;
    throttleInput = 0.0f;
    steeringInput = 0.0f;
    isDead = false;
}

Vector2 Car::getPosition() const {
    return position;
}

Vector2 Car::getVelocity() const {
    return velocity;
}

float Car::getHeadingDegrees() const {
    return headingDegrees;
}

float Car::getDriftOffsetDegrees() const {
    return driftOffsetDegrees;
}

bool Car::getIsDead() const {
    return isDead;
}

Vector2 Car::getForwardVector() const {
    float headingRadians = headingDegrees * PI_VALUE / 180.0f;
    return {
        std::cos(headingRadians),
        std::sin(headingRadians),
    };
}

Vector2 Car::getRightVector() const {
    float headingRadians = headingDegrees * PI_VALUE / 180.0f;
    return {
        -std::sin(headingRadians),
        std::cos(headingRadians),
    };
}

float Car::dot(Vector2 a, Vector2 b) const {
    return a.x * b.x + a.y * b.y;
}

float Car::length(Vector2 v) const {
    return std::sqrt(v.x * v.x + v.y * v.y);
}

float Car::normalizeAngleDegrees(float angle) const {
    while (angle > 180.0f) {
        angle -= 360.0f;
    }
    while (angle < -180.0f) {
        angle += 360.0f;
    }
    return angle;
}

Vector2 Car::rotateLocalPoint(Vector2 localPoint) const {
    float headingRadians = headingDegrees * PI_VALUE / 180.0f;
    float c = std::cos(headingRadians);
    float s = std::sin(headingRadians);

    return {
        localPoint.x * c - localPoint.y * s,
        localPoint.x * s + localPoint.y * c,
    };
}

void Car::getCorners(Vector2 corners[4]) const {
    float halfWidth = width / 2.0f;
    float halfHeight = height / 2.0f;

    Vector2 localCorners[4] = {
        {-halfWidth, -halfHeight},
        { halfWidth, -halfHeight},
        { halfWidth,  halfHeight},
        {-halfWidth,  halfHeight},
    };

    for (int i = 0; i < 4; i++) {
        Vector2 rotated = rotateLocalPoint(localCorners[i]);
        corners[i] = {
            position.x + rotated.x,
            position.y + rotated.y,
        };
    }
}

bool Car::isCollidingWithWalls(const std::vector<Rectangle>& walls) const {
    Vector2 corners[4];
    getCorners(corners);

    for (const Rectangle& wall : walls) {
        for (int i = 0; i < 4; i++) {
            if (CheckCollisionPointRec(corners[i], wall)) {
                return true;
            }
        }
    }

    return false;
}