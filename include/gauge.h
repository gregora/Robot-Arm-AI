#include <SFML/Graphics.hpp>
#include <stdio.h>
#include <string>

class Gauge : public sf::Drawable, public sf::Transformable {
public:
	Gauge(std::string name = "", float min = 0, float max = 1);
	~Gauge();

	std::string name = "";
	std::string unit = "";
	float min = 0;
	float max = 1;
	float value = 0.5;

	void draw(sf::RenderTarget& target, sf::RenderStates states) const;


private:

	sf::Sprite sprite;
	sf::Texture texture;

	sf::Font font;
};
