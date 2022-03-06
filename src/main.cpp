#include "stdio.h"
#include <iostream>


#include <SFML/Graphics.hpp>



int main()
{

	sf::RenderWindow window(sf::VideoMode(640, 480), "SFML works!");
	sf::CircleShape shape(100.f);
	shape.setFillColor(sf::Color::Green);

	sf::Texture texture;
	if (!texture.loadFromFile("./content/brick_blue.png"))
	{
		std::cout << "Error: unable to load texture";
	}

	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
		}

		window.clear();
		window.draw(shape);
		window.display();
	}
}