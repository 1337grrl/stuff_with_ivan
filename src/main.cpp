#include "stdio.h"
#include <iostream>


#include <SFML/Graphics.hpp>



int main()
{
	sf::RenderWindow window(sf::VideoMode(1024, 768), "SFML works!");
	
	sf::Texture texture;
	if (!texture.loadFromFile("content/brick_blue.png"))
	{
		std::cout << "Error: unable to load texture";
	}

	sf::Sprite sprite = sf::Sprite(texture);
	sprite.setPosition(sf::Vector2f(80.f, 80.f));

	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
		}

		sf::Color clearClr = sf::Color(0, 0, 0, 255);
		//clearClr = sf::Color(128, 128, 128, 255);
		window.clear(clearClr);
		
		window.draw(sprite);

		window.display();
	}
}