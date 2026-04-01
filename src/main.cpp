#include "Car.h"
#include "raylib.h"

int main() {
    InitWindow(1600, 1000, "Car");
    SetTargetFPS(60);

    std::vector<Rectangle> walls = {
    {30, 50, 20, 900},
    {650, 50, 20, 900},
    {30, 30, 640, 20},
    {30, 950, 640, 20},

    {340,50,20,500},
    {190,150,20,500},
    {490,150,20,500},
    {190,650,320,20},
    };

    Car car({600.0f, 400.0f}, -90.0f);

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
        car.update(dt, walls);

        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawRectangle(50, 50, 600, 900, DARKGRAY);   // rett strekning

        for (const Rectangle& wall : walls) {
            DrawRectangleRec(wall, RED);
        }

        car.draw();

        EndDrawing();
    }

    CloseWindow();
    return 0;
}