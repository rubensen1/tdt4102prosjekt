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
        {510,450,140,8},
        {510,390,140,8},
        {510,330,140,8},
        {510,270,140,8},
        {510,210,140,8},
        {510,150,140,8},
        
        {496,50,8,100},
        
        {360,150,130,8},
        {360,210,130,8},
        {360,270,130,8},
        {360,330,130,8},
        {360,390,130,8},
        {360,450,130,8},
        {360,510,130,8},
        
        {346,550,8,100},
        
        {210,510,130,8},
        {210,450,130,8},
        {210,390,130,8},
        {210,330,130,8},
        {210,270,130,8},
        {210,210,130,8},
        {210,150,130,8},
        
        {196,50,8,100},
        
        {50,150,140,8},
        {50,210,140,8},
        {50,270,140,8},
        {50,330,140,8},
        {50,390,140,8},
        {50,450,140,8},
        {50,510,140,8},
        {50,570,140,8},
        {50,630,140,8},

        {196,670,8,280},
        {246,670,8,280},
        {296,670,8,280},
        {346,670,8,280},
        {396,670,8,280},
        {446,670,8,280},
        {496,670,8,280},
    };
    
    std::vector<Car> carList;

    for (int i = 0; i<cars;i++) {
        carList.push_back(Car({580.0f, 550.0f}, -90.0f));
    }

    int generation = 1;
    float lastBestFitness = 0;

    while (!WindowShouldClose()) {

        auto startTime = std::chrono::steady_clock::now();

        
        while (true) {
            
            if (WindowShouldClose()) {
                break;
            }
            auto now = std::chrono::steady_clock::now();
            float elapsed = std::chrono::duration<float>(now - startTime).count();
            
            float dt = GetFrameTime();
            int carsAlive = 0;
    
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
                    carsAlive += 1;
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
            ClearBackground(GRAY);
    
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

            for (Car& car : carList) {
                if (car.leader) {
                    if (car.leader ==1){
                        car.bren.draw(car.getInputs());
                    }
                    car.draw(walls);
                }
            }
            
            DrawText(TextFormat("Cars alive: %d/%d",carsAlive ,cars) , 700, 20, 17, BLACK);
            DrawText(TextFormat("MutationRate: %.2f * e^(-%d/70) = %.3f", mutationRate, generation, mutationRate*std::exp(-generation/70.0f)) , 700, 40, 17, BLACK);
            DrawText(TextFormat("MutationStrength: %.2f * e^(-%d/70) = %.3f", mutationStrength, generation, mutationStrength*std::exp(-generation/70.0f)) , 700, 60, 17, BLACK);
            DrawText(TextFormat("Generation: %d", generation) , 700, 80, 17, BLACK);
            DrawText(TextFormat("fps: %.2f", 1/dt) , 700, 100, 15, BLACK);

            DrawText(TextFormat("Last gen best fitness: %.2f",lastBestFitness) , 1050, 20, 17, BLACK);
            DrawText(TextFormat("Last gen 1st fitness: %.2f",carList[0].fitness) , 1050, 40, 17, BLACK);
            DrawText(TextFormat("Last gen 2nd fitness: %.2f",carList[1].fitness) , 1050, 60, 17, BLACK);
            DrawText(TextFormat("Last gen 3rd fitness: %.2f",carList[2].fitness) , 1050, 80, 17, BLACK);



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

        // std::cout<<carList[0].fitness<<std::endl;
        // std::cout<<carList[1].fitness<<std::endl;
        // std::cout<<carList[2].fitness<<std::endl;
        // std::cout<<carList[3].fitness<<std::endl;

        std::vector<Car> nyCarList;

        nyCarList.push_back(carList[0]);
        nyCarList.push_back(carList[1]);
        nyCarList.push_back(carList[2]);

        nyCarList[0].leader = 1;
        nyCarList[1].leader = 2;
        nyCarList[2].leader = 2;

        if (lastBestFitness>=carList[0].fitness){
            mutationRate+=0.02;
            mutationStrength+=0.04;
        } else {
            mutationRate-=0.02;
            mutationStrength-=0.04;
        }

        lastBestFitness = carList[0].fitness;

        for (int i = 0; i < 10; i++) {
            for (int y = 0; y <20;y++) {
                NeuralNetwork goodBrain = carList[i].bren;
                goodBrain.mutate(mutationRate,mutationStrength,generation);

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