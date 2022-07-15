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

	void setValue(float value);

	void draw(sf::RenderTarget& target, sf::RenderStates states) const;


private:
	float value = 0.5;

	sf::Sprite sprite;
	sf::Texture texture;

	sf::Font font;
	sf::Text text;
};
