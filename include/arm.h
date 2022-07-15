#include "box2d/box2d.h"
#include <stdio.h>
#include <vector>

#include <SFML/Graphics.hpp>

#define RAD2DEG 57.325

class Arm : public sf::Drawable, public sf::Transformable {

public:
	b2World* world;
	Arm(std::vector<float> lengths, float max_torque = 1);
	~Arm();

	void applySpeeds(std::vector<float> speeds);
	void applySpeed(uint index, float speed);
	void getAngles(float* angles);
	float getAngle(uint index);

	b2Vec2 getArmLocation();

	void physics(float delta, bool debug = false);

	uint8 opacity = 255;

	void draw(sf::RenderTarget& target, sf::RenderStates states) const;
	bool draw_arrow = false;

private:
	std::vector<float> lengths;
};


float vector2angle(float x, float y);
