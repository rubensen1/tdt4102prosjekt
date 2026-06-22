#include "car.h"
#include "AnimationWindow.h"
#include "widgets/Button.h"

#include <iostream>
#include <chrono>
#include <algorithm>
#include <string>

int main() {
    TDT4102::AnimationWindow window(100, 50, 1600, 1000, "Car");
    TDT4102::Button saveButton{{700, 900}, 160, 40, "Save best"};
    window.add(saveButton);
    TDT4102::Button loadButton{{900, 900}, 160, 40, "Load file"};
    window.add(loadButton);

    int cars = 200;
    float mutationRate = 0.2f;
    float mutationStrength = 0.5f;

    std::vector<Rect> walls = {         //chat endra til rect struktur ved raylib-animationwindow overgangen
        {30,  50,  20,  900},
        {650, 50,  20,  900},
        {30,  30,  640, 20},
        {30,  950, 640, 20},
        {340, 50,  20,  500},
        {190, 150, 20,  500},
        {490, 150, 20,  500},
        {190, 650, 320, 20},
    };

    std::vector<Rect> checkpoints = {
        {510,450,140,8},{510,390,140,8},{510,330,140,8},{510,270,140,8},
        {510,210,140,8},{510,150,140,8},
        {496,50,8,100},
        {360,150,130,8},{360,210,130,8},{360,270,130,8},{360,330,130,8},
        {360,390,130,8},{360,450,130,8},{360,510,130,8},
        {346,550,8,100},
        {210,510,130,8},{210,450,130,8},{210,390,130,8},{210,330,130,8},
        {210,270,130,8},{210,210,130,8},{210,150,130,8},
        {196,50,8,100},
        {50,150,140,8},{50,210,140,8},{50,270,140,8},{50,330,140,8},
        {50,390,140,8},{50,450,140,8},{50,510,140,8},{50,570,140,8},
        {50,630,140,8},
        {196,670,8,280},{246,670,8,280},{296,670,8,280},{346,670,8,280},
        {396,670,8,280},{446,670,8,280},{496,670,8,280},
    };

    std::vector<Car> carList;

    for (int i = 0; i<cars;i++) {
        carList.push_back(Car({580.0f, 550.0f}, -90.0f));
    }

    int generation = 1;
    float lastBestFitness = 0;
        
    while (!window.should_close()) {

        std::vector<Car> carQueue;

        saveButton.setCallback([&]() {
            carList[0].bren.saveToFile();
        });

        loadButton.setCallback([&]() {
            Car nyBil = Car({580.0f, 550.0f}, -90.0f);
            nyBil.bren.loadFromFIle();
            carQueue.push_back(nyBil);
        });
            
        auto startTime = std::chrono::steady_clock::now();
        auto lastFrame = startTime;
        
        while (true) {
            if (window.should_close()) break;

            auto now = std::chrono::steady_clock::now();
            float dt = std::chrono::duration<float>(now - lastFrame).count();
            lastFrame = now;
            float elapsed = std::chrono::duration<float>(now - startTime).count();
            
            // Klamp dt så fysikken ikke sprekker ved lav FPS
            // dt = std::min(dt, 0.05f);
            
            int carsAlive = 0;
            for (Car& car : carList) {
                if (!car.getIsDead()) {
                    car.updateAI();
                    car.update(dt, walls, checkpoints, generation);
                    carsAlive++;
                }
            }
            float effRate = mutationRate * std::exp(-generation / 60.0f);
            float effStr  = mutationStrength * std::exp(-generation / 60.0f);
            
            window.draw_rectangle({0, 0}, 1600, 1000, TDT4102::Color::gray);
            
            window.draw_rectangle({50, 50}, 600, 900, TDT4102::Color::dark_gray);

            for (const Rect& w : walls) {
                window.draw_rectangle(
                    {static_cast<int>(w.x), static_cast<int>(w.y)},
                    static_cast<int>(w.width), static_cast<int>(w.height),
                    TDT4102::Color::red);
            }

            for (const Rect& cp : checkpoints) {
                window.draw_rectangle(      //CHAT-GENERERT I OVERGANGEN FRA RAYLIB TIL ANIMATIONWINDOW
                    {static_cast<int>(cp.x), static_cast<int>(cp.y)},
                    static_cast<int>(cp.width), static_cast<int>(cp.height),
                    TDT4102::Color::green);
            }

            for (Car& car : carList) {
                if (!car.getIsDead()) {     //CHAT-GENERERT I OVERGANGEN FRA RAYLIB TIL ANIMATIONWINDOW
                    car.draw(window, walls);
                }
            }

            // Leader-biler tegnes oppå (og neural net for leader 1)
            for (Car& car : carList) {
                if (car.leader) {
                    if (car.leader == 1)
                        car.bren.draw(window, car.getInputs());
                    car.draw(window, walls);
                }
            }

            //alt av HUD endra chat i den store raylib-animationwindow overgangen 
            window.draw_text({700, 20},
                "Cars alive: " + std::to_string(carsAlive) + "/" + std::to_string(cars),//CHAT-GENERERT I OVERGANGEN FRA RAYLIB TIL ANIMATIONWINDOW
                TDT4102::Color::black, 17);
            window.draw_text({700, 40},
                "Mutation rate: " + std::to_string(effRate).substr(0,6),//CHAT-GENERERT I OVERGANGEN FRA RAYLIB TIL ANIMATIONWINDOW
                TDT4102::Color::black, 17);
            window.draw_text({700, 60},
                "Mutation strength: " + std::to_string(effStr).substr(0,6),//CHAT-GENERERT I OVERGANGEN FRA RAYLIB TIL ANIMATIONWINDOW
                TDT4102::Color::black, 17);
            window.draw_text({700, 80},
                "Generation: " + std::to_string(generation),//CHAT-GENERERT I OVERGANGEN FRA RAYLIB TIL ANIMATIONWINDOW
                TDT4102::Color::black, 17);
            window.draw_text({700, 100},
                "FPS: " + std::to_string(1/dt).substr(0,6),//CHAT-GENERERT I OVERGANGEN FRA RAYLIB TIL ANIMATIONWINDOW
                TDT4102::Color::black, 15);

            window.draw_text({1150, 20},
                "Best fitness last round: " + std::to_string(lastBestFitness).substr(0,7),//CHAT-GENERERT I OVERGANGEN FRA RAYLIB TIL ANIMATIONWINDOW
                TDT4102::Color::black, 17);
            window.draw_text({1150, 40},
                "1st: " + std::to_string(carList[0].fitness).substr(0,7),//CHAT-GENERERT I OVERGANGEN FRA RAYLIB TIL ANIMATIONWINDOW
                TDT4102::Color::black, 17);
            window.draw_text({1150, 60},
                "2nd: " + std::to_string(carList[1].fitness).substr(0,7),//CHAT-GENERERT I OVERGANGEN FRA RAYLIB TIL ANIMATIONWINDOW
                TDT4102::Color::black, 17);
            window.draw_text({1150, 80},
                "3rd: " + std::to_string(carList[2].fitness).substr(0,7),//CHAT-GENERERT I OVERGANGEN FRA RAYLIB TIL ANIMATIONWINDOW
                TDT4102::Color::black, 17);

            window.next_frame();

            bool allDead = true;
            for (const Car& car : carList)
                if (!car.getIsDead()) { allDead = false; break; }

            if (elapsed >= 20.0f || allDead) break;
        }

        std::ranges::sort(carList, std::ranges::greater{}, &Car::fitness);

        std::vector<Car> nyCarList;
        nyCarList.push_back(carList[0]);
        nyCarList.push_back(carList[1]);
        nyCarList.push_back(carList[2]);
        nyCarList[0].leader = 1;
        nyCarList[1].leader = 2;
        nyCarList[2].leader = 2;

        if (carQueue.size()>0) {
            nyCarList[0].leader = 2;
            for (Car car : carQueue) {
                car.leader = 1;
                nyCarList.insert(nyCarList.begin(), car);
            }
        }

        // if (lastBestFitness >= carList[0].fitness) {
        //     mutationRate += 0.02f;
        //     mutationStrength += 0.04f;
        // } else {
        //     mutationRate -= 0.02f;
        //     mutationStrength -= 0.04f;
        // }
        mutationRate += 0.015f;
        mutationStrength+=0.03f;
        lastBestFitness = carList[0].fitness;

        for (int i = 0; i < 10; i++) {
            for (int y = 0; y < 20; y++) {
                NeuralNetwork brain = carList[i].bren;
                brain.mutate(mutationRate, mutationStrength, generation);
                Car child(Vec2{580.0f, 550.0f}, -90.0f, brain);
                nyCarList.push_back(child);
            }
        }

        carList = nyCarList;
        for (Car& car : carList)
            car.reset(Vec2{580.0f, 550.0f}, -90.0f);

        generation++;
    }

    return 0;
}
