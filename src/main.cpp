#include "raylib.h"
#include <cmath>

struct Car {
    Vector2 position;
    float width;
    float height;
    float rotation;
    float speed;
};

int main() {
    const int screenWidth = 1200;
    const int screenHeight = 800;

    InitWindow(screenWidth, screenHeight, "Car AI");
    SetTargetFPS(60);

    Car car;
    car.position = {600.0f, 400.0f};
    car.width = 60.0f;
    car.height = 30.0f;
    car.rotation = -90.0f;
    car.speed = 0.0f;

    while (!WindowShouldClose()) {
        if (IsKeyDown(KEY_UP)) {
            car.speed += 0.2f;
        }
        if (IsKeyDown(KEY_DOWN)) {
            car.speed -= 0.1f;
        }

        if (IsKeyDown(KEY_LEFT)) {
            car.rotation -= 2.0f;
        }
        if (IsKeyDown(KEY_RIGHT)) {
            car.rotation += 2.0f;
        }

        if (car.speed > 5.0f) car.speed = 5.0f;
        if (car.speed < -2.0f) car.speed = -2.0f;

        float angleRad = car.rotation * PI / 180.0f;
        car.position.x += std::cos(angleRad) * car.speed;
        car.position.y += std::sin(angleRad) * car.speed;

        car.speed *= 0.98f;

        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawRectangleLines(100, 100, 1000, 600, BLACK);

        Rectangle rect = {
            car.position.x,
            car.position.y,
            car.width,
            car.height
        };

        Vector2 origin = {
            car.width / 2.0f,
            car.height / 2.0f
        };

        DrawRectanglePro(rect, origin, car.rotation, RED);

        DrawCircleV(car.position, 3.0f, BLUE);

        DrawText("Arrow keys to move", 20, 20, 20, DARKGRAY);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}