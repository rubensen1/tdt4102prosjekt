#include "car.h"
#include "neural.h"

#include <cmath>
#include <iostream>

static constexpr float PI_VALUE = 3.14159265358979323846f;

static bool rayIntersectsRectangle(Vector2 origin,
                                   Vector2 direction,
                                   Rectangle rect,
                                   float& outDistance) {
    const float epsilon = 0.00001f;

    float tMin = -INFINITY;
    float tMax = INFINITY;

    if (std::fabs(direction.x) < epsilon) {
        if (origin.x < rect.x || origin.x > rect.x + rect.width) {
            return false;
        }
    } else {
        float tx1 = (rect.x - origin.x) / direction.x;
        float tx2 = ((rect.x + rect.width) - origin.x) / direction.x;

        float txMin = std::fmin(tx1, tx2);
        float txMax = std::fmax(tx1, tx2);

        tMin = std::fmax(tMin, txMin);
        tMax = std::fmin(tMax, txMax);
    }

    if (std::fabs(direction.y) < epsilon) {
        if (origin.y < rect.y || origin.y > rect.y + rect.height) {
            return false;
        }
    } else {
        float ty1 = (rect.y - origin.y) / direction.y;
        float ty2 = ((rect.y + rect.height) - origin.y) / direction.y;

        float tyMin = std::fmin(ty1, ty2);
        float tyMax = std::fmax(ty1, ty2);

        tMin = std::fmax(tMin, tyMin);
        tMax = std::fmin(tMax, tyMax);
    }

    if (tMax < 0.0f) {
        return false;
    }

    if (tMin > tMax) {
        return false;
    }

    outDistance = (tMin >= 0.0f) ? tMin : tMax;
    return true;
}

static Vector2 angleToDirection(float angleDegrees) {
    float angleRadians = angleDegrees * PI_VALUE / 180.0f;
    return {std::cos(angleRadians), std::sin(angleRadians)};
}

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
    engineForce(400.0f),
    turnRate(150.0f),
    lateralGrip(3.0f),
    drag(0.80f),
    sensorDegreeValues{
        -75.0f, -40.0f, -15.0f, 0.0f, 15.0f, 40.0f, 75.0f
    },
    sensorMaxDistance(500.0f),
    sensorAmount(sensorDegreeValues.size()),
    sensorDistances(sensorAmount, 0.0f),
    fitness(0.0f),
    currentCheckpointIndex(0),
    leader(0),

    bren(9,8,2)
    
    {}

Car::Car(Vector2 startPosition, float startHeadingDegrees, const NeuralNetwork& brain)
  : position(startPosition),
    velocity{0.0f, 0.0f},
    headingDegrees(startHeadingDegrees),
    driftOffsetDegrees(0.0f),
    throttleInput(0.0f),
    steeringInput(0.0f),
    isDead(false),
    width(40.0f),
    height(20.0f),
    engineForce(400.0f),
    turnRate(150.0f),
    lateralGrip(3.0f),
    drag(0.80f),
    sensorDegreeValues{
        -75.0f, -40.0f, -15.0f, 0.0f, 15.0f, 40.0f, 75.0f
    },
    sensorMaxDistance(500.0f),
    sensorAmount(sensorDegreeValues.size()),
    sensorDistances(sensorAmount, 0.0f),
    fitness(0.0f),
    currentCheckpointIndex(0),
    leader(0),

    bren(brain)
    
    {}

void Car::update(float dt, const std::vector<Rectangle>& walls, const std::vector<Rectangle>& checkpoints) {
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
    // if (newSpeed > 0.01f) {
        float velocityAngleRadians = std::atan2(velocity.y, velocity.x);
        float velocityAngleDegrees = velocityAngleRadians * 180.0f / PI_VALUE;
        driftOffsetDegrees =
            normalizeAngleDegrees(velocityAngleDegrees - headingDegrees);
    // } else {
    //     driftOffsetDegrees = 0.0f;
    // }

    for (int i = 0; i<sensorAmount; i++) {
        sensorDistances[i] = castRayToWalls(sensorDegreeValues[i], walls, sensorMaxDistance);
    };
    // std::cout << sensorDistances[8]<<std::endl; 90 grader til høyre
    
    if (CheckCollisionPointRec(position, checkpoints[currentCheckpointIndex])) {
        currentCheckpointIndex++;
        fitness += 100.0f;
        if (currentCheckpointIndex==35) {
            currentCheckpointIndex = 0;
        }
    }
    
    // std::cout << position.y<<std::endl;
    // std::cout << newSpeed<<std::endl;
    // std::cout << currentCheckpointIndex<<std::endl;
}

void Car::draw(const std::vector<Rectangle>& walls) const {
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

    if (!leader) {
        DrawRectanglePro(body, origin, headingDegrees, ORANGE);
    // } else if (leader == 3){
    //     DrawRectanglePro(body, origin, headingDegrees, PINK);
    } else if (leader == 2){
        DrawRectanglePro(body, origin, headingDegrees, PINK);
    } else {
        DrawRectanglePro(body, origin, headingDegrees, VIOLET);
    }

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

    // tegne sensorene
    for (int i = 0; i<sensorAmount; i++) {
        drawSensor(i, sensorDegreeValues[i], walls, 500.0f, BLUE);
    };

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

void Car::setOutputs(float throttleAmount, float steeringAmount) {
    // if (isDead) {
    //     return;
    // }

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
    sensorDistances.assign(sensorAmount, 0.0f);
    fitness = 0.0f;
    currentCheckpointIndex = 0;
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

float Car::castRayToWalls(float angleOffsetDegrees,
                          const std::vector<Rectangle>& walls,
                          float maxDistance) const {
    Vector2 origin = position;
    Vector2 direction = angleToDirection(headingDegrees + angleOffsetDegrees);

    float closestDistance = maxDistance;
    bool hitAnything = false;

    for (const Rectangle& wall : walls) {
        float hitDistance = 0.0f;
        if (rayIntersectsRectangle(origin, direction, wall, hitDistance)) {
            if (hitDistance >= 0.0f && hitDistance < closestDistance) {
                closestDistance = hitDistance;
                hitAnything = true;
            }
        }
    }

    if (!hitAnything) {
        return maxDistance;
    }

    return closestDistance;
}

void Car::drawSensor(int sensorNr, float angleOffsetDegrees,const std::vector<Rectangle>& walls,float maxDistance, Color color) const {
    float distance = sensorDistances[sensorNr];
    Vector2 direction = angleToDirection(headingDegrees + angleOffsetDegrees);
    
    Vector2 endPoint = {
        position.x + direction.x * distance,
        position.y + direction.y * distance,
    };
    
    DrawLineEx(position, endPoint, 2.0f, color);
    DrawCircleV(endPoint, 3.0f, color);
}

const std::vector<float>& Car::getSensorDistances() const {
    return sensorDistances;
}

void Car::updateAI() {
    std::vector<float> inputs = getInputs();
    std::vector<float> outputs = bren.getOutput(inputs);
    setOutputs(outputs[0], outputs[1]);
}

std::vector<float> Car::getInputs() {               //legger alle inputs etter hverandre
    std::vector<float> inputs;
    for (float& value : sensorDistances) {
        inputs.push_back(value/500);
    }
    inputs.push_back(length(velocity)/500);             //burde finne theoretical maks velocity for å normalisere den til -1,1
    inputs.push_back(driftOffsetDegrees/180);           //normalisert??
    return inputs;
}