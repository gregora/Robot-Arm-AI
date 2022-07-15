#include "../include/gauge.h"

Gauge::Gauge(std::string name, float min, float max){
	this -> name = name;
	this -> min = min;
	this -> max = max;

	texture.loadFromFile("textures/gauge.png");
	sprite.setTexture(texture);
	sprite.setScale(sf::Vector2f(0.2, 0.2));

	if(!font.loadFromFile("fonts/Prototype.ttf")){
		printf("WARNING: Gauge object could not load \"fonts/Prototype.ttf\"");
	}



}

Gauge::~Gauge(){}

void Gauge::draw(sf::RenderTarget& target, sf::RenderStates states) const {


	float angle = 180 * (value - min) / (max - min);
	sf::RectangleShape pointer;
	pointer.setSize(sf::Vector2f(50, 5));
	pointer.setOrigin(45, 2.5);
	pointer.setFillColor(sf::Color(150, 150, 150));
	pointer.setRotation(angle);
	pointer.setPosition(50, 50);

	sf::Text text;
	text.setFont(font);
	text.setFillColor(sf::Color(255, 255, 255));
	text.setCharacterSize(12);
	char string[40];
	sprintf(string, "%4.2f %s", value, unit.c_str());
	text.setString(string);
	float t_width = text.getLocalBounds().width;
	text.setPosition((100 - t_width) / 2, 70);


	sf::Text name_text;
	name_text.setFont(font);
	name_text.setFillColor(sf::Color(150, 150, 150));
	name_text.setCharacterSize(9);
	name_text.setString(name);
	float nt_width = name_text.getLocalBounds().width;
	name_text.setPosition((100 - nt_width) / 2, 60);

	target.draw(sprite, states.transform*getTransform());
	target.draw(pointer, states.transform*getTransform());
	target.draw(text, states.transform*getTransform());
	target.draw(name_text, states.transform*getTransform());

}
