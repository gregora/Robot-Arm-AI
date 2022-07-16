#include "box2d/box2d.h"
#include <stdio.h>
#include <vector>

#include <SFML/Graphics.hpp>

#define RAD2DEG 57.325

class PhysicsArm {
	public:
		PhysicsArm(std::vector<float> lengths, float max_torque = 1);
		~PhysicsArm();

		b2World* world;

		void applySpeeds(std::vector<float> speeds);
		void getAngles(float* angles) const;

		b2Vec2 getArmLocation() const;

		void physics(float delta, bool debug = false);

		std::vector<float> lengths;
	private:

};


class Arm : public PhysicsArm, public sf::Drawable, public sf::Transformable {

public:
	Arm(std::vector<float> lengths, float max_torque = 1);
	~Arm();

	uint8 opacity = 255;

	void draw(sf::RenderTarget& target, sf::RenderStates states) const;
	bool draw_arrow = false;

private:
	sf::Texture texture;
};


float vector2angle(float x, float y);
