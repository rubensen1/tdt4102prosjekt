#include "Car.h"
#include "raylib.h"

int main() {
    InitWindow(1200, 800, "Car");
    SetTargetFPS(60);

    Car car({600.0f, 400.0f}, 0.0f);

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();

        float throttle = 0.0f;
        float steering = 0.0f;

        if (IsKeyDown(KEY_UP)) {
            throttle += 1.0f;
        }
        if (IsKeyDown(KEY_DOWN)) {
            throttle -= 1.0f;
        }
        if (IsKeyDown(KEY_LEFT)) {
            steering -= 1.0f;
        }
        if (IsKeyDown(KEY_RIGHT)) {
            steering += 1.0f;
        }

        car.setInputs(throttle, steering);
        car.update(dt);

        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawRectangleLines(100, 100, 1000, 600, BLACK);
        car.draw();

        DrawText("Arrow keys to drive", 20, 20, 20, DARKGRAY);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}