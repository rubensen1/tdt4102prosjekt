#include "Car.h"
#include "raylib.h"
#include <iostream>

int main() {
    InitWindow(1600, 1000, "Car");
    SetTargetFPS(60);

    std::vector<Rectangle> walls = {
        {30, 50, 20, 900},  //vertikal rundt
        {650, 50, 20, 900},
        {30, 30, 640, 20},  //horisontal rundt
        {30, 950, 640, 20},

        {340,50,20,500},    //løype?
        {190,150,20,500},
        {490,150,20,500},
        {190,650,320,20},   //horisontal midt
    };

    std::vector<Rectangle> checkpoints = {
        {510,450,140,7},
        {510,390,140,7},
        {510,330,140,7},
        {510,270,140,7},
        {510,210,140,7},
        {510,150,140,7},

        {497,50,6,100},
        
        {360,150,130,7},
        {360,210,130,7},
        {360,270,130,7},
        {360,330,130,7},
        {360,390,130,7},
        {360,450,130,7},
        {360,510,130,7},

        {347,550,6,100},
    };

    Car car({600.0f, 500.0f}, -90.0f);

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
        car.update(dt, walls, checkpoints);

        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawRectangle(50, 50, 600, 900, DARKGRAY);   // rett strekning

        for (const Rectangle& wall : walls) {
            DrawRectangleRec(wall, RED);
        }
        for (const Rectangle& checkpoint : checkpoints) {
            DrawRectangleRec(checkpoint, GREEN);
        }

        car.draw(walls);
        
        EndDrawing();
    }

    CloseWindow();
    return 0;
}