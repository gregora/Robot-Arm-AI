# Robot-Arm-AI
A project for training a neural network to control a robotic arm.

## Problem
Solving inverse kinematics can often be challenging in complex robots. This project tries to illustrate how neural networks can be taught to solve such problems.
The robot is a simple planar arm with 3 sections and the whole project is simply a proof of concept.

## Results
1000 generations were needed for great results. However, I think the method has proven itself - at least for simple robots.

https://github.com/gregora/Robot-Arm-AI/assets/26600857/820e87c5-0127-4b7d-af2a-d16c778b5e73

Accompanying Youtube video: [https://youtu.be/oZJqmPPVW6Q?si=ku-KLPI7Qgqxm4LI](https://youtu.be/oZJqmPPVW6Q?si=ku-KLPI7Qgqxm4LI)


## Dependencies
* `lib/libarm.a` - [Robot-Arm](https://github.com/gregora/Robot-Arm)
* `lib/libnn.a` - [NN-Lib](https://github.com/gregora/NN-Lib)
* `lib/libbox2d.a` - [box2d (version 2.4.1)](https://github.com/erincatto/box2d/releases/tag/v2.4.1)
* `SFML` - [SFML (version 2.5.1)](https://www.sfml-dev.org/download/sfml/2.5.1/)

## Compiling

### Linux

Make sure you have SFML installed.
If not, you can do that by running `sudo apt-get install libsfml-dev`.

Run `make` command.

## Running

### Linux

#### Training

To train your network you need to run a command `./main.out -population [population] -generatons [generations]`.

Your networks will be saved every 10 generations into the  `networks/` folder.

To load a saved generation simply add `-load [generation]` to your command line arguments.

#### Loading already trained networks

To load a network you must run a command `./main.out -display [generation] -population [population]`.

There are already pretrained networks of 1000th generation in the `networks/` folder, that you can run with `./main.out -display 1000 -population 100`.
