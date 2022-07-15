#include "include/algorithms.h"
#include "include/arm.h"
#include "include/gauge.h"
#include <stdio.h>
#include <iostream>
#include <sys/stat.h>
#include <stdlib.h>

using namespace std;
using namespace nnlib;

#define SAMPLES 100
#define TIME 7

void render(Network* n, bool record = false);
void save_population(Network ** networks, uint population, string folder);
void load_population(Network ** networks, uint population, string folder);

float evaluate(Network* n, float target_x, float target_y){

		Arm a({1,1,1}, 20);

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

			motors_speed = abs(speeds[0]) + abs(speeds[1]) + abs(speeds[2]);

			a.applySpeeds(speeds);
			a.physics(delta);

			passed += delta;
		}

		float x,y;
		x = a.getArmLocation().x - target_x;
		y = a.getArmLocation().y - target_y;

	 	return x*x + y*y + motors_speed;
}

void evaluate(Network* n, float* score){

	*score = 0;

	float x = 0;
	float y = 0;

	for(int i = 1; i <= SAMPLES; i++){
		x = (float) (134234*i % 4000 - 2000); //random x coordinate
		y = (float) (852343*i % 4000 - 2000); //random y coordinate
		*score += evaluate(n, x / 1000, y / 1000);
	}

	*score = *score / SAMPLES;
}

int main(int argsn, char** args){

	int generation = 0;
	bool display = false;

	int POPULATION = 100;
	int GENERATIONS = 100;
	bool MULTITHREADING = false;
	bool RENDER = false;

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
		}else if(strcmp(args[i], "-render") == 0){
			RENDER = true;
		}

	}

	Network * networks[POPULATION];

	if(generation == 0){

		for(int i = 0; i < POPULATION; i++){
			networks[i] = new Network();

			if(generation == 0){
				Dense* d1 = new Dense(5, 20);
				Dense* d2 = new Dense(20, 3);

				d1 -> randomize(-1, 1);
				d2 -> randomize(-1, 1);

				d1 -> setActivationFunction("atan");
				d2 -> setActivationFunction("linear");

				networks[i] -> addLayer(d1);
				networks[i] -> addLayer(d2);
			}
		}

	}else{
		load_population(networks, POPULATION, "networks/"+to_string(generation)+"/");
	}

	gen_settings settings = {
		population: POPULATION,
		generations: 11, //number of generations to run
		mutation_rate: 0.2, //number of mutations on each child

		rep_coef: 0.1, //percent of population to reproduce

		delta: 0.2,

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
		render(networks[0], RENDER);
	}


}


void render(Network* n, bool record){

	Arm a({1,1,1}, 5);

	int WIDTH = 800;
	int HEIGHT = 600;
	float zoom = 0.03;


	if(record){
		WIDTH = 1920;
		HEIGHT = 1080;
		zoom = 0.02;
	}

	sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Arm");
	sf::View view(sf::Vector2f(0, 0), sf::Vector2f(WIDTH / 2, HEIGHT / 2));
	sf::View default_view = window.getView();

	view.zoom(zoom);
	window.setView(view);

	float target_x = nnlib::random()*4 - 2;
	float target_y = nnlib::random()*4 - 2;

	uint frame = 0;

	sf::Clock clock;
	float passed = 0;
	while (window.isOpen())
	{
		frame++;
		float dist = (b2Vec2(target_x, target_y) - a.getArmLocation()).Length();

		sf::Vector2i pixelPos = sf::Mouse::getPosition(window);
		sf::Vector2f worldPos = window.mapPixelToCoords(pixelPos);

		if(sf::Mouse::isButtonPressed(sf::Mouse::Left)){
			if(window.hasFocus()){
				target_x = worldPos.x;
				target_y = -worldPos.y;
			}
		}

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

		if(sf::Mouse::isButtonPressed(sf::Mouse::Right)){
			if(window.hasFocus()){
				speeds[0] = -5*(angles[0]);
				speeds[1] = -5*(angles[1]);
				speeds[2] = -5*(angles[2]);
				passed = 0;
			}
		}

		float d = delta.asSeconds();

		if(record){
			d = 1.0f/60;
		}

		a.applySpeeds(speeds);
		a.physics(d);

		sf::RectangleShape ground;
		ground.setSize(sf::Vector2f(WIDTH, HEIGHT / 2));
		ground.setFillColor(sf::Color(15, 15, 15));
		ground.setPosition(-WIDTH/2, 3);

		sf::RectangleShape pillar;
		pillar.setOrigin(0.07, 1.5);
		pillar.setSize(sf::Vector2f(0.14, 5));
		pillar.setFillColor(sf::Color(25, 25, 25));


		sf::Texture target_texture;
		target_texture.loadFromFile("textures/target.png");
		sf::Sprite target(target_texture);
		target.setOrigin(10, 10);
		target.setScale(sf::Vector2f(0.01, 0.01));
		target.setPosition(target_x, -target_y);

		sf::RectangleShape line;
		line.setFillColor(sf::Color(50, 0, 0));
		line.setSize(sf::Vector2f(0.02, dist));
		line.setPosition(a.getArmLocation().x, -a.getArmLocation().y);
		line.setRotation(-90 + 180.0f/3.145f * vector2angle(target_x - a.getArmLocation().x, -target_y + a.getArmLocation().y));

		sf::Text text;
		sf::Font font;
		font.loadFromFile("fonts/Prototype.ttf");
		text.setFont(font);
		char str[40];
		sprintf(str, "Distance: %4.2f m", dist);
		text.setString(str);
		text.setCharacterSize(24);
		text.setFillColor(sf::Color::White);
		text.setPosition(10,10);

		Gauge gauge1("Motor 1", -150, 150);
		gauge1.unit = "deg/s";
		gauge1.setPosition(20, 100);
		gauge1.value = speeds[0]*RAD2DEG;

		Gauge gauge2("Motor 2", -150, 150);
		gauge2.unit = "deg/s";
		gauge2.setPosition(20, 200);
		gauge2.value = speeds[1]*RAD2DEG;

		Gauge gauge3("Motor 3", -150, 150);
		gauge3.unit = "deg/s";
		gauge3.setPosition(20, 300);
		gauge3.value = speeds[2]*RAD2DEG;


		//tower
		pillar.setRotation(10);
		pillar.setPosition(-0.3, 1.5);
		window.draw(pillar);
		pillar.setRotation(-10);
		pillar.setPosition(0.3, 1.5);
		window.draw(pillar);


		pillar.setRotation(25);
		pillar.setPosition(-0.35, 2.7);
		window.draw(pillar);
		pillar.setRotation(-25);
		pillar.setPosition(0.35, 2.7);
		window.draw(pillar);

		window.draw(ground);


		window.draw(line);
		window.draw(target);

		window.draw(a);

		window.setView(default_view);
		window.draw(text);

		window.draw(gauge1);
		window.draw(gauge2);
		window.draw(gauge3);
		window.setView(view);

		// end the current frame
		window.display();

		if(record){
			window.capture().saveToFile("render/" + to_string(frame) + ".png");


			/*if(passed >= TIME + 1){
				window.close();
				system("cd render/ && ./render.sh");
				return;
			}*/
		}

		passed+=d;

		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed){
				window.close();

				if(record){
					system("cd render/ && ./render.sh");
					return;
				}

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


bool b2ContactFilter::ShouldCollide	(	b2Fixture * 	fixtureA,
b2Fixture * 	fixtureB
){
	return false;
}
