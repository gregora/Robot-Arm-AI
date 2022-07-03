#include "include/algorithms.h"
#include "include/arm.h"
#include <stdio.h>
#include <iostream>

using namespace std;
using namespace nnlib;

#define TIME 5
#define SAMPLES 5

void render(Arm a, Network* n, float time);

float evaluate(Network* n, float target_x, float target_y){
		Arm a({1,1,1}, 5);

		float delta = 1/60.0f;
		float passed = 0;

		float motors_speed;

		while(passed <= TIME){

			float angles[3];
			a.getAngles(angles);

			Matrix input(1, 5);
			input.setValue(0, 0, angles[0]);
			input.setValue(0, 1, angles[1]);
			input.setValue(0, 2, angles[2]);
			input.setValue(0, 3, target_x);
			input.setValue(0, 4, target_y);

			Matrix output = n -> eval(&input);

			vector<float> speeds = {0,0,0};
			speeds[0] = output.getValue(0, 0);
			speeds[1] = output.getValue(0, 1);
			speeds[2] = output.getValue(0, 2);

			//printf("%f %f %f\n", speeds[0], speeds[1], speeds[2]);
			motors_speed =  + abs(speeds[0]) + abs(speeds[1]) + abs(speeds[2]);

			a.applySpeeds(speeds);
			a.physics(delta);

			passed += delta;
		}

	 	return abs(a.getArmLocation().x - target_x) + abs(a.getArmLocation().y - target_y) + motors_speed;
}

void evaluate(Network* n, float* score){

	*score = 0;

	float x = 0;
	float y = 0;

	for(int i = 0; i < SAMPLES; i++){
		x = (float) (134234*i % 2000);
		y = (float) (852343*i % 2000);
		*score += evaluate(n, y / 1000, x / 1000);
	}

	*score = *score / 5;
}

int main(int argsn, char** args){

	int generation = 0;
	bool display = false;

	int POPULATION = 100;
	int GENERATIONS = 100;

	for(int i = 0; i < argsn; i++){
		if(strcmp(args[i], "-load") == 0){
			generation = atol(args[i + 1]);
		}else if(strcmp(args[i], "-display") == 0){
			generation = atol(args[i + 1]);
			display = true;
		}else if(strcmp(args[i], "-population") == 0){
			POPULATION = atoi(args[i + 1]);
		}else if(strcmp(args[i], "-generations") == 0){
			GENERATIONS = atoi(args[i + 1]);
		}

	}

	Network * networks[POPULATION];

	for(int i = 0; i < POPULATION; i++){
		networks[i] = new Network();

		if(generation == 0){
			Dense* d1 = new Dense(5, 5);
			Dense* d2 = new Dense(5, 3);

			d1 -> setActivationFunction(atan);
			d2 -> setActivationFunction(atan);

			networks[i] -> addLayer(d1);
			networks[i] -> addLayer(d2);
		}else{
			networks[i] -> load("networks/" + to_string(generation) + ".AI");
		}
	}

	gen_settings settings = {
		population: POPULATION,
		generations: GENERATIONS, //number of generations to run
		mutations: 1, //number of mutations on each child

		rep_coef: 0.5, //percent of population to reproduce

		min: -5, //minimum value for weights / biases
		max: 5, //maximum value for weights / biases

		recompute_parents: false, //recompute parents (for non-deterministic evaluation functions)
		multithreading: true,

		output: true,
		start_generation: generation
	};

	if(!display){
		genetic(networks, evaluate, settings);
		networks[0] -> save("networks/" + to_string(generation + GENERATIONS - 1) + ".AI");
	}else{
		Arm a({1,1,1}, 5);
		render(a, networks[0], 20);
	}


}


void render(Arm a, Network* n, float time){

	int WIDTH = 800;
	int HEIGHT = 600;


	sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Arm");
	sf::View view(sf::Vector2f(0, 0), sf::Vector2f(WIDTH / 2, HEIGHT / 2));
	view.zoom(0.03);
	window.setView(view);

	float target_x = 1;
	float target_y = 1;

	sf::Clock clock;
	float passed = 0;
	while (window.isOpen())
	{
		window.clear(sf::Color::Black);

		sf::Time delta = clock.restart();
		float angles[3];
		a.getAngles(angles);

		Matrix input(1, 5);
		input.setValue(0, 0, angles[0]);
		input.setValue(0, 1, angles[1]);
		input.setValue(0, 2, angles[2]);
		input.setValue(0, 3, target_x);
		input.setValue(0, 4, target_y);

		Matrix output = n -> eval(&input);

		vector<float> speeds = {0,0,0};
		speeds[0] = output.getValue(0, 0);
		speeds[1] = output.getValue(0, 1);
		speeds[2] = output.getValue(0, 2);

		a.applySpeeds(speeds);
		a.physics(delta.asSeconds());

		window.draw(a);

		// end the current frame
		window.display();


		passed+=delta.asSeconds();
		if(passed >= time){
			window.close();
			return;
		}

		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed){
				window.close();
				return;
			}
		}

	}

}
