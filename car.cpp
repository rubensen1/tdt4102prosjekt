#include "car.h"
#include "neural.h"

#include <cmath>
#include <iostream>

static constexpr float PI_VALUE = 3.14159265358979323846f;

static bool rayIntersectsRectangle(Vec2 origin,             
                                   Vec2 direction,
                                   Rect rect,
                                   float& outDistance) {
    const float epsilon = 0.00001f;         //Chat generert funksjon, ikke i forbindelse med the great animationwindow transfer of '26, men rett og slett fordi jeg synes det var vanskelig å lage selv
    float tMin = -INFINITY;
    float tMax =  INFINITY;

    if (std::fabs(direction.x) < epsilon) {
        if (origin.x < rect.x || origin.x > rect.x + rect.width) return false;
    } else {
        float tx1 = (rect.x - origin.x) / direction.x;
        float tx2 = (rect.x + rect.width - origin.x) / direction.x;
        tMin = std::fmax(tMin, std::fmin(tx1, tx2));
        tMax = std::fmin(tMax, std::fmax(tx1, tx2));
    }

    if (std::fabs(direction.y) < epsilon) {
        if (origin.y < rect.y || origin.y > rect.y + rect.height) return false;
    } else {
        float ty1 = (rect.y - origin.y) / direction.y;
        float ty2 = (rect.y + rect.height - origin.y) / direction.y;
        tMin = std::fmax(tMin, std::fmin(ty1, ty2));
        tMax = std::fmin(tMax, std::fmax(ty1, ty2));
    }

    if (tMax < 0.0f || tMin > tMax) return false;

    outDistance = (tMin >= 0.0f) ? tMin : tMax;
    return true;
}

static Vec2 angleToDirection(float angleDegrees) {
    float rad = angleDegrees * PI_VALUE / 180.0f;
    return {std::cos(rad), std::sin(rad)};
}

Car::Car(Vec2 startPosition, float startHeadingDegrees)
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

    bren(9,8,2),

    controller(nullptr)
    
    {}

Car::Car(Vec2 startPosition, float startHeadingDegrees, const NeuralNetwork& brain)
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

    bren(brain),

    controller(nullptr)
    
    {}

void Car::update(float dt, const std::vector<Rect>& walls, const std::vector<Rect>& checkpoints, float generation) {
   if (isDead) {
        return;
    }

    Vec2 forward = getForwardVector();
    Vec2 right = getRightVector();

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

    float velocityAngleDeg = std::atan2(velocity.y, velocity.x) * 180.0f / PI_VALUE;
    driftOffsetDegrees = normalizeAngleDegrees(velocityAngleDeg - headingDegrees);

    for (int i = 0; i<sensorAmount; i++) {
        sensorDistances[i] = castRayToWalls(sensorDegreeValues[i], walls, sensorMaxDistance);
    };
    
    if (pointInRect(position, checkpoints[currentCheckpointIndex])) {
        currentCheckpointIndex++;
        fitness += 100.0f + speed/100;
        if (currentCheckpointIndex==35) {
            currentCheckpointIndex = 0;
        }
    }
}

void Car::draw(TDT4102::AnimationWindow& window, const std::vector<Rect>& walls) const {
    TDT4102::Color bodyColor = TDT4102::Color::orange;
    if (leader == 1) {
        bodyColor = TDT4102::Color::violet;
    }
    else if (leader == 2) {
        bodyColor = TDT4102::Color::pink;
    }

    // Roterte hjørner → draw_quad (erstatter DrawRectanglePro)
    Vec2 corners[4];        //CHAT-GENERERT I OVERGANGEN FRA RAYLIB TIL ANIMATIONWINDOW
    getCorners(corners);
    window.draw_quad(
        corners[0].toPoint(),
        corners[1].toPoint(),
        corners[2].toPoint(),
        corners[3].toPoint(),
        bodyColor
    );

    // Hastighetspil (grønn linje)
    // float spd = length(velocity);
    // if (spd > 0.01f) {
    //     Vec2 velDir = {velocity.x / spd, velocity.y / spd};
    //     TDT4102::Point velEnd = {
    //         static_cast<int>(position.x + velDir.x * 40.0f),
    //         static_cast<int>(position.y + velDir.y * 40.0f)
    //     };
    //     window.draw_line(position.toPoint(), velEnd, TDT4102::Color::green);
    // }

    // Sensorer (blå linjer)
    for (int i = 0; i < sensorAmount; i++)
        drawSensor(window, i, sensorDegreeValues[i], 500.0f, TDT4102::Color::blue);
}

void Car::drawSensor(TDT4102::AnimationWindow& window, int sensorNr,//CHAT-GENERERT I OVERGANGEN FRA RAYLIB TIL ANIMATIONWINDOW
                     float angleOffsetDegrees, float /*maxDistance*/,
                     TDT4102::Color color) const {
    float dist = sensorDistances[sensorNr];
    Vec2 dir   = angleToDirection(headingDegrees + angleOffsetDegrees);
    TDT4102::Point endPt = {
        static_cast<int>(position.x + dir.x * dist),
        static_cast<int>(position.y + dir.y * dist)
    };
    window.draw_line(position.toPoint(), endPt, color);
}

void Car::setOutputs(float throttleAmount, float steeringAmount) {
    throttleInput = throttleAmount;
    steeringInput = steeringAmount;
}

void Car::reset(Vec2 startPosition, float startHeadingDegrees) {
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

bool Car::getIsDead() const {
    return isDead;
}

const std::vector<float>& Car::getSensorDistances() const {
    return sensorDistances;
}

Vec2 Car::getForwardVector() const {
    float rad = headingDegrees * PI_VALUE / 180.0f;
    return {std::cos(rad), std::sin(rad)};
}

Vec2 Car::getRightVector() const {
    float rad = headingDegrees * PI_VALUE / 180.0f;
    return {-std::sin(rad), std::cos(rad)};
}
float Car::dot(Vec2 a, Vec2 b) const {
    return a.x * b.x + a.y * b.y;
}

float Car::length(Vec2 v) const {
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

Vec2 Car::rotateLocalPoint(Vec2 localPoint) const {
    float headingRadians = headingDegrees * PI_VALUE / 180.0f;
    float c = std::cos(headingRadians);
    float s = std::sin(headingRadians);

    return {
        localPoint.x * c - localPoint.y * s,
        localPoint.x * s + localPoint.y * c,
    };
}

void Car::getCorners(Vec2 corners[4]) const {
    float halfWidth = width / 2.0f;
    float halfHeight = height / 2.0f;

    Vec2 localCorners[4] = {
        {-halfWidth, -halfHeight},
        { halfWidth, -halfHeight},
        { halfWidth,  halfHeight},
        {-halfWidth,  halfHeight},
    };

    for (int i = 0; i < 4; i++) {
        Vec2 rotated = rotateLocalPoint(localCorners[i]);
        corners[i] = {
            position.x + rotated.x,
            position.y + rotated.y,
        };
    }
}

bool Car::isCollidingWithWalls(const std::vector<Rect>& walls) const {
    Vec2 corners[4];
    getCorners(corners);

    for (const Rect& wall : walls) {
        for (int i = 0; i < 4; i++) {
            if (pointInRect(corners[i], wall)) {
                return true;
            }
        }
    }

    return false;
}

float Car::castRayToWalls(float angleOffset, const std::vector<Rect>& walls, float maxDist) const {
    Vec2 dir = angleToDirection(headingDegrees + angleOffset);

    float closestDistance = maxDist;


    for (const Rect& wall : walls) {
        float hitDistance = 0.0f;
        if (rayIntersectsRectangle(position, dir, wall, hitDistance)) {
            if (hitDistance >= 0.0f && hitDistance < closestDistance) {
                closestDistance = hitDistance;
            }
        }
    }
    return closestDistance;
}   

void Car::updateAI() {
    std::vector<float> inputs = getInputs();
    std::vector<float> outputs;

    if (controller != nullptr) {
        outputs = controller->getOutput(inputs);
    } else {
        outputs = bren.getOutput(inputs); // fallback
    }

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