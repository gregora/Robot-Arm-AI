# Robot-Arm-AI
A project for training a neural network to control a robotic arm

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
