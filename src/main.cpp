#include "car.h"
#include "raylib.h"

#include <iostream>
#include <chrono>
#include <algorithm>
#include <string>

int main() {
    // signal(SIGABRT, [](int){ __builtin_trap(); });
    InitWindow(1600, 1000, "Car");
    SetTargetFPS(60);

    int cars = 200;
    float mutationRate = 0.2f;
    float mutationStrength = 0.5f;

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
        
        {210,510,130,7},
        {210,450,130,7},
        {210,390,130,7},
        {210,330,130,7},
        {210,270,130,7},
        {210,210,130,7},
        {210,150,130,7},
        
        {197,50,6,100},

        {50,150,140,7},
        {50,210,140,7},
        {50,270,140,7},
        {50,330,140,7},
        {50,390,140,7},
        {50,450,140,7},
    };
    
    std::vector<Car> carList;

    for (int i = 0; i<cars;i++) {
        carList.push_back(Car({580.0f, 550.0f}, -90.0f));
    }

    int generation = 1;

    while (!WindowShouldClose()) {

        auto startTime = std::chrono::steady_clock::now();

        while (true) {

            if (WindowShouldClose()) {
                break;
            }
            auto now = std::chrono::steady_clock::now();
            float elapsed = std::chrono::duration<float>(now - startTime).count();

            float dt = GetFrameTime();
    
            // float throttle = 0.0f;
            // float steering = 0.0f;
    
            // if (IsKeyDown(KEY_UP)) {
            //     throttle += 1.0f;
            // }
            // if (IsKeyDown(KEY_DOWN)) {
            //     throttle -= 1.0f;
            // }
            // if (IsKeyDown(KEY_LEFT)) {
            //     steering -= 1.0f;
            // }
            // if (IsKeyDown(KEY_RIGHT)) {
            //     steering += 1.0f;
            // }
    
            for (Car& car : carList) {
                if (!car.getIsDead()) {
                    car.updateAI();
                    car.update(dt, walls, checkpoints);
                }
            }
    
            // manuell mode:
            // for (Car& car : carList) {               
            //     if (!car.getIsDead()) {
            //         car.setOutputs(throttle, steering);
            //         car.update(dt, walls, checkpoints);
            //     }
            // }
    
            BeginDrawing();
            ClearBackground(RAYWHITE);
    
            DrawRectangle(50, 50, 600, 900, DARKGRAY);   // asfaltblokk yuh
    
            for (const Rectangle& wall : walls) {
                DrawRectangleRec(wall, RED);
            }
            for (const Rectangle& checkpoint : checkpoints) {
                DrawRectangleRec(checkpoint, GREEN);
            }
    
            for (Car& car : carList) {
                if (!car.getIsDead()) {
                    car.draw(walls);
                }
            }

            DrawText(TextFormat("Cars: %d", cars) , 700, 20, 15, RED);
            DrawText(TextFormat("Mutationrate: %.1f", mutationRate) , 700, 40, 15, RED);
            DrawText(TextFormat("Mutationstrength: %.1f", mutationStrength) , 700, 60, 15, RED);
            DrawText(TextFormat("Generation: %d", generation) , 700, 80, 15, RED);
            DrawText(TextFormat("fps: %.2f", 1/dt) , 700, 100, 10, RED);

            carList[0].bren.draw();

            EndDrawing();

            bool allDead = true;
            for (const Car& car : carList) {
                if (!car.getIsDead()) {
                    allDead = false;
                    break;
                }
            }

            if (elapsed >= 20.0f || allDead) {
                break;
            }
        }

        std::ranges::sort(carList, std::ranges::greater{}, &Car::fitness);

        std::cout<<carList[0].fitness<<std::endl;
        std::cout<<carList[1].fitness<<std::endl;
        std::cout<<carList[2].fitness<<std::endl;
        std::cout<<carList[3].fitness<<std::endl;

        std::vector<Car> nyCarList;

        nyCarList.push_back(carList[0]);
        nyCarList.push_back(carList[1]);
        nyCarList.push_back(carList[2]);
        nyCarList[0].leader = 1;
        nyCarList[1].leader = 2;
        nyCarList[2].leader = 2;

        for (int i = 0; i < 10; i++) {
            for (int y = 0; y <20;y++) {
                NeuralNetwork goodBrain = carList[i].bren;
                goodBrain.mutate(0.8f,0.4f);

                Car goodChild({580.0f, 550.0f}, -90.0f, goodBrain);
                // goodChild.bren = goodBrain;
                nyCarList.push_back(goodChild);
            }
        }

        carList = nyCarList;

        for (Car& car : carList) {
            car.reset({580.0f, 550.0f}, -90.0f);
        }

        //nå mutere da?
        generation++;

        




    }

    CloseWindow();
    return 0;
}