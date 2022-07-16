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
#define TIME 3
#define SAVE_PERIOD 1

void render(Network** n, int population, bool record = false, float max_time = 0, string title = "");
void save_population(Network ** networks, uint population, string folder);
void load_population(Network ** networks, uint population, string folder);

float evaluate(Network* n, float target_x, float target_y){

		Arm a({1,1,1}, 20, false);

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
		generations: SAVE_PERIOD + 1, //number of generations to run
		mutation_rate: 0.2, //number of mutations on each child

		rep_coef: 0.1, //percent of population to reproduce

		delta: 0.2,

		recompute_parents: false, //recompute parents (for non-deterministic evaluation functions)
		multithreading: MULTITHREADING,

		output: true,
		start_generation: generation
	};

	if(!display){
		//save every SAVE_PERIOD generations
		for(int i = SAVE_PERIOD; i <= GENERATIONS; i+=SAVE_PERIOD){
			genetic(networks, evaluate, settings);
			printf("\nSaving networks ...\n\n\n\n");
			save_population(networks, POPULATION, "networks/" + to_string(generation + i)+"/");
			settings.start_generation = settings.start_generation + SAVE_PERIOD;
		}

		settings.generations = (GENERATIONS % SAVE_PERIOD) + 1;
		genetic(networks, evaluate, settings);
		printf("\nSaving networks ...\n\n\n\n");
		save_population(networks, POPULATION, "networks/" + to_string(generation + GENERATIONS)+"/");

	}else{
		render(networks, POPULATION, RENDER, 32, "GENERATION " + to_string(generation));
	}


}


void render(Network** n, int population, bool record, float max_time, string title){

	Arm* a[population];

	float points[][2] = {{1.0, -1.34}, {-0.33, -1.4}, {-1.7, 1.23}, {0.23, 1.8}};

	for(int i = 0; i < population; i++){
		a[i] = new Arm({1,1,1}, 5);
		if(i != 0){
			a[i] -> opacity = 50;
		}
	}

	int WIDTH = 800;
	int HEIGHT = 600;
	float zoom = 0.03;


	if(record){
		WIDTH = 1920;
		HEIGHT = 1080;
		zoom = 0.015;
	}

	sf::ContextSettings settings;
	settings.antialiasingLevel = 8;
	sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Arm", sf::Style::Default, settings);
	sf::View view(sf::Vector2f(0, 0), sf::Vector2f(WIDTH / 2, HEIGHT / 2));
	sf::View default_view = window.getView();

	view.zoom(zoom);
	window.setView(view);

	float target_x = points[0][0];
	float target_y = points[0][1];

	uint frame = 0;

	sf::Clock clock;
	float passed = 0;
	float real_time = 0;

	Gauge gauge1("Motor 1", -150, 150);
	gauge1.unit = "deg/s";
	Gauge gauge2("Motor 2", -150, 150);
	gauge2.unit = "deg/s";
	Gauge gauge3("Motor 3", -150, 150);
	gauge3.unit = "deg/s";

	sf::RectangleShape line;
	line.setFillColor(sf::Color(50, 0, 0));

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
	target_texture.setSmooth(true);
	target.setOrigin(10, 10);
	target.setScale(sf::Vector2f(0.007, 0.007));

	sf::Text text;
	sf::Font font;
	font.loadFromFile("fonts/Prototype.ttf");
	text.setFont(font);
	text.setCharacterSize(30);
	text.setPosition(20,20);
	text.setFillColor(sf::Color::White);

	sf::Text title_text;
	title_text.setFont(font);
	title_text.setString(title);
	title_text.setCharacterSize(40);
	title_text.setFillColor(sf::Color::White);
	title_text.setPosition(WIDTH - title_text.getLocalBounds().width - 40, 20);

	if(!record){
		gauge1.setPosition(20, 100);
		gauge2.setPosition(20, 200);
		gauge3.setPosition(20, 300);

		gauge1.setScale(0.2f, 0.2f);
		gauge2.setScale(0.2f, 0.2f);
		gauge3.setScale(0.2f, 0.2f);

		text.setCharacterSize(24);
		title_text.setCharacterSize(30);

	}else{
		gauge1.setPosition(20, 100);
		gauge2.setPosition(20, 300);
		gauge3.setPosition(20, 500);

		gauge1.setScale(0.4f, 0.4f);
		gauge2.setScale(0.4f, 0.4f);
		gauge3.setScale(0.4f, 0.4f);
	}

	int p = 0;

	while (window.isOpen())
	{
		frame++;
		float dist = (b2Vec2(target_x, target_y) - a[0] -> getArmLocation()).Length();

		sf::Vector2i pixelPos = sf::Mouse::getPosition(window);
		sf::Vector2f worldPos = window.mapPixelToCoords(pixelPos);

		if(sf::Mouse::isButtonPressed(sf::Mouse::Left)){
			if(window.hasFocus() && !record){
				target_x = worldPos.x;
				target_y = -worldPos.y;
			}
		}

		window.clear(sf::Color::Black);

		sf::Time delta = clock.restart();
		float d = delta.asSeconds();

		if(record){
			d = 1.0f/60;
		}

		vector<float> speeds = {0,0,0};
		float angles[3];

		for(int i = population - 1; i >= 0; i--){

			a[i] -> getAngles(angles);

			Matrix input(1, 5);
			input.setValue(0, 0, angles[0]);
			input.setValue(0, 1, angles[1]);
			input.setValue(0, 2, angles[2]);
			input.setValue(0, 3, target_x);
			input.setValue(0, 4, target_y);

			Matrix output = n[i] -> eval(&input);
			speeds[0] = output.getValue(0, 0);
			speeds[1] = output.getValue(0, 1);
			speeds[2] = output.getValue(0, 2);

			a[i] -> applySpeeds(speeds);
			a[i] -> physics(d);
		}

		if(sf::Mouse::isButtonPressed(sf::Mouse::Right)){
			if(window.hasFocus() && !record){
				speeds[0] = -5*(angles[0]);
				speeds[1] = -5*(angles[1]);
				speeds[2] = -5*(angles[2]);
				passed = 0;
			}
		}

		gauge1.value = speeds[0]*RAD2DEG;
		gauge2.value = speeds[1]*RAD2DEG;
		gauge3.value = speeds[2]*RAD2DEG;

		target.setPosition(target_x, -target_y);

		line.setSize(sf::Vector2f(0.02, dist));
		line.setPosition(a[0] -> getArmLocation().x, -a[0] -> getArmLocation().y);
		line.setRotation(-90 + 180.0f/3.145f * vector2angle(target_x - a[0] -> getArmLocation().x, -target_y + a[0] -> getArmLocation().y));

		char str[40];
		sprintf(str, "Distance: %4.2f m", dist);
		text.setString(str);


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

		for(int i = population - 1; i >= 0; i--){
			window.draw(*a[i]);
		}

		window.setView(default_view);
		window.draw(text);
		window.draw(title_text);

		window.draw(gauge1);
		window.draw(gauge2);
		window.draw(gauge3);
		window.setView(view);

		// end the current frame
		window.display();

		if(record){
			window.capture().saveToFile("render/" + to_string(frame) + ".png");

			if(max_time != 0 && max_time <= real_time){
				window.close();
				system("cd render/ && ./render.sh");
				return;
			}

			if(passed >= TIME + 1){
				passed = 0;
				p++;
				target_x = points[p][0];
				target_y = points[p][1];
			}

		}

		passed+=d;
		real_time+=d;

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
