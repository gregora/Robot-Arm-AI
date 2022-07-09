#include "include/algorithms.h"
#include "include/arm.h"
#include <stdio.h>
#include <iostream>
#include <sys/stat.h>

using namespace std;
using namespace nnlib;

#define TIME 7
#define SAMPLES 20

float linear(float x){
	return x;
}

void render(Arm a, Network* n);
void save_population(Network ** networks, uint population, string folder);
void load_population(Network ** networks, uint population, string folder);

float evaluate(Arm* a, Network* n, float target_x, float target_y){

		Arm b({1,1,1}, 20);
		a = &b;

		float delta = 1/60.0f;
		float passed = 0;

		float motors_speed;

		while(passed <= TIME){

			float angles[3];
			a -> getAngles(angles);

			Matrix input(1, 5);
			input.setValue(0, 0, angles[0]);
			input.setValue(0, 1, angles[1]);
			input.setValue(0, 2, angles[2]);
			input.setValue(0, 3, target_x);
			input.setValue(0, 4, target_y);

			Matrix output = n -> eval(&input);

			vector<float> speeds = {0,0,0};
			speeds[0] = 1 - output.getValue(0, 0);
			speeds[1] = 1 - output.getValue(0, 1);
			speeds[2] = 1 - output.getValue(0, 2);

			motors_speed = abs(speeds[0]) + abs(speeds[1]) + abs(speeds[2]);

			a -> applySpeeds(speeds);
			a -> physics(delta);

			passed += delta;
		}

		float x,y;
		x = a -> getArmLocation().x - target_x;
		y = a -> getArmLocation().y - target_y;

	 	return x*x + y*y + motors_speed;
}

void evaluate(Network* n, float* score){

	Arm a({1,1,1}, 20);

	*score = 0;

	float x = 0;
	float y = 0;

	for(int i = 1; i <= SAMPLES; i++){
		x = (float) (134234*i % 4000 - 2000); //random x coordinate
		y = (float) (852343*i % 4000 - 2000); //random y coordinate
		*score += evaluate(&a, n, y / 1000, x / 1000);
	}

	*score = *score / SAMPLES;
}

int main(int argsn, char** args){

	int generation = 0;
	bool display = false;

	int POPULATION = 100;
	int GENERATIONS = 100;
	bool MULTITHREADING = false;

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
		}else if(strcmp(args[i], "-multithreading") == 0){
			MULTITHREADING = true;
		}

	}

	Network * networks[POPULATION];

	if(generation == 0){

		for(int i = 0; i < POPULATION; i++){
			networks[i] = new Network();

			if(generation == 0){
				Dense* d1 = new Dense(5, 20);
				Dense* d2 = new Dense(20, 30);
				Dense* d3 = new Dense(30, 100);
				Dense* d4 = new Dense(100, 30);
				Dense* d5 = new Dense(30, 20);
				Dense* d6 = new Dense(20, 3);

				d1 -> setActivationFunction(atan);
				d2 -> setActivationFunction(atan);
				d3 -> setActivationFunction(atan);
				d4 -> setActivationFunction(atan);
				d5 -> setActivationFunction(atan);
				d6 -> setActivationFunction(linear);

				networks[i] -> addLayer(d1);
				networks[i] -> addLayer(d2);
				networks[i] -> addLayer(d3);
				networks[i] -> addLayer(d4);
				networks[i] -> addLayer(d5);
				networks[i] -> addLayer(d6);
			}
		}

	}else{
		load_population(networks, POPULATION, "networks/"+to_string(generation)+"/");
	}

	gen_settings settings = {
		population: POPULATION,
		generations: 11, //number of generations to run
		mutations: 5, //number of mutations on each child

		rep_coef: 0.5, //percent of population to reproduce

		min: -5, //minimum value for weights / biases
		max: 5, //maximum value for weights / biases

		recompute_parents: false, //recompute parents (for non-deterministic evaluation functions)
		multithreading: MULTITHREADING,

		output: true,
		start_generation: generation
	};

	if(!display){
		//save every 10 generations
		for(int i = 10; i <= GENERATIONS; i+=10){
			genetic(networks, evaluate, settings);
			printf("\nSaving networks ...\n\n\n\n");
			save_population(networks, POPULATION, "networks/" + to_string(generation + i)+"/");
			settings.start_generation = settings.start_generation + 10;
		}

		settings.generations = (GENERATIONS % 10) + 1;
		genetic(networks, evaluate, settings);
		printf("\nSaving networks ...\n\n\n\n");
		save_population(networks, POPULATION, "networks/" + to_string(generation + GENERATIONS)+"/");

	}else{
		Arm a({1,1,1}, 5);
		render(a, networks[0]);
	}


}


void render(Arm a, Network* n){

	int WIDTH = 800;
	int HEIGHT = 600;


	sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Arm");
	sf::View view(sf::Vector2f(0, 0), sf::Vector2f(WIDTH / 2, HEIGHT / 2));
	view.zoom(0.03);
	window.setView(view);

	float target_x = 0.234;
	float target_y = -1.67;

	sf::Clock clock;
	float passed = 0;
	while (window.isOpen())
	{


		sf::Vector2i pixelPos = sf::Mouse::getPosition(window);
		sf::Vector2f worldPos = window.mapPixelToCoords(pixelPos);

		if(sf::Mouse::isButtonPressed(sf::Mouse::Left)){
			target_x = worldPos.x;
			target_y = -worldPos.y;
		}


		//printf("%f %f\n", target_x, target_y);

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
		speeds[0] = 1 - output.getValue(0, 0);
		speeds[1] = 1 - output.getValue(0, 1);
		speeds[2] = 1 - output.getValue(0, 2);

		if(sf::Mouse::isButtonPressed(sf::Mouse::Right)){
			speeds[0] = -5*(angles[0]);
			speeds[1] = -5*(angles[1]);
			speeds[2] = -5*(angles[2]);
		}

		a.applySpeeds(speeds);
		a.physics(delta.asSeconds());

		sf::CircleShape target(0.05);
		target.setOrigin(0.05, 0.05);
		target.setFillColor(sf::Color(255, 0, 0));

		target.setPosition(target_x, -target_y);

		window.draw(target);
		window.draw(a);

		// end the current frame
		window.display();


		passed+=delta.asSeconds();

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


void save_population(Network ** networks, uint population, string folder){

	mkdir(folder.c_str(), 0777);

	for(uint i = 0; i < population; i++){
		networks[i] -> save(folder + to_string(i) + ".AI");
	}

}

void load_population(Network ** networks, uint population, string folder){
	for(uint i = 0; i < population; i++){
		networks[i] = new Network;
		networks[i] -> load(folder + to_string(i) + ".AI");
	}
}
