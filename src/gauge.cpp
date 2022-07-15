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

	text.setFont(font);
	text.setPosition(20, 70);
	text.setFillColor(sf::Color(255, 255, 255));
	text.setCharacterSize(12);


}

Gauge::~Gauge(){}

void Gauge::setValue(float value){
	char string[40];
	this -> value = value;
	sprintf(string, "%4.2f %s", value, unit.c_str());
	text.setString(string);
}

void Gauge::draw(sf::RenderTarget& target, sf::RenderStates states) const {


	float angle = 180 * (value - min) / (max - min);
	sf::RectangleShape pointer;
	pointer.setSize(sf::Vector2f(50, 5));
	pointer.setOrigin(45, 2.5);
	pointer.setFillColor(sf::Color(150, 150, 150));
	pointer.setRotation(angle);
	pointer.setPosition(50, 50);

	target.draw(sprite, states.transform*getTransform());
	target.draw(pointer, states.transform*getTransform());
	target.draw(text, states.transform*getTransform());

}
