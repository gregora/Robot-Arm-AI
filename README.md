# Robot-Arm-AI
A project for training a neural network to control a robot arm

## Dependencies
* `lib/libarm.a` - [Robot-Arm](https://github.com/gregora/Robot-Arm)
* `lib/libnn.a` - [NN-Lib](https://github.com/gregora/NN-Lib)
* `lib/libbox2d.a` - [box2d (version 2.4.1)](https://github.com/erincatto/box2d/releases/tag/v2.4.1)

## Compiling

### Linux

Run `make` command.

## Running

### Linux

#### Training

To train your network you need to run a command `./main.out -population [population] -generatons [generations]`.

Your networks will be saved every 10 generations into the  `networks/` folder.

#### Loading already trained networks

To load a network you must run a command `./main.out -display [generation] -population [number of arms to render]`.

There are already pretrained networks of 1000th generation in the `networks/` folder, that you can run with `./main.out -display 1000 -population 100`.
