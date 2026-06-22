# Car

Car is an easy neural network that drives a car around a lap.

![Demo](demo.gif)

## How it works
Every car is controlled by a neural network, that receives sensor-data and outputs throttle and turn, it learns by evolution. By default this is not deep learning because there is only one hidden layer. And it is not NEAT algorithm, since it lacks crossover, it is not really to be called reinforcement learning either.

Well what does it do?

Throughout the making of this project, most of the questions have been about what AI-library I am using, but I made my own, and this has a very big advantage in that its very intuitive and easy to understand, its a good entry-level implementation of neural networks and learning. The code for the neural network is less than 100 lines of code and 
can very easily be modified to use deep learning, NEAT, or other learning algorithms. 

Activation method used is the tanh formula

## Requirements
- Windows
- Meson
- Ninja
- C++ compiler

## Run without building
Download the repository and run:

./build/program.exe

## Installation
In order:

To build:

```bash
meson setup build
```
To compile:

```bash
meson compile -Cbuild
```

To run:
```bash
./build/program.exe
```

## Usage

Launch the program, sit back, check in once in a while to see how "evolution" is doing.

Sometimes (rarely) they will evolve doing cool tricks, as that was how your "run" evolved by random chance.

Included in root are 3 .txt files that can be loaded onto a live run, rename the file you are testing to "car.txt" and click "Load File".

 - Note that "Save File" overwrites "car.txt" with the current best neural network.

The learning follows a curve given by (0.5+0.03*gen)*e^(-gen/60), that has showed potential in testing.

## Contributing

The project is no more maintained or worked on at all.
Early raylib version can be cloned at https://github.com/rubensen1/tdt4102prosjekt.git

Might make another git for the project after the great animationwindow transfer of '26

 - Updated git to use finished animationwindow version 22/06/2026

## License

No license