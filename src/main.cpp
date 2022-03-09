#include "stdio.h"
#include <iostream>


#include <SFML/Graphics.hpp>

const float WINDOW_WIDTH = 1024.;
const float WINDOW_HEIGHT = 768.;
const float PADDING = 15.;


int mousePosX;


sf::Vector2f padPos;
float padSpeed = 0.1f;
const sf::Vector2f padSize = sf::Vector2f(120.f, 10.f);
const sf::Vector2f padHalfSize = padSize * 0.5f;


sf::Vector2f ballPosition;
float ballSpeed;
float ballSize;

sf::RectangleShape pad;
sf::CircleShape ball;

void ResetGame()
{
	ballPosition = sf::Vector2f(WINDOW_WIDTH*.5, 500.f);
	padPos = sf::Vector2f(WINDOW_WIDTH*.5 - padHalfSize.x, WINDOW_HEIGHT - padSize.y - PADDING);
	ballSpeed = 0.05f;
	ballSize = 10.f;
}

void Input(const sf::RenderWindow& window)
{
	mousePosX = sf::Mouse::getPosition(window).x;

	if (mousePosX > PADDING && mousePosX < (WINDOW_WIDTH - padSize.x - PADDING)) {
		padPos.x = mousePosX - padHalfSize.x;
	}

	pad.setPosition(padPos);

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
	{
		ResetGame();
	}
}

int main()
{
	ResetGame();

	sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "SFML works!");
	
	sf::Texture brickTexture;
	if (!brickTexture.loadFromFile("content/brick_blue.png"))
	{
		std::cout << "Error: unable to load texture";
	}

	const int count = 5;
	sf::Sprite sprites[count];
	
	for(int i = 0; i < count; ++i)
	{
		sprites[i] = sf::Sprite(brickTexture);
		sprites[i].setPosition(sf::Vector2f(80.f + i * 150.f, 80.f));
		sprites[i].setScale(0.2f, 0.2f);
	}
	

	pad = sf::RectangleShape(padSize);
	pad.setFillColor(sf::Color::Green);
	//pad.setOrigin(sf::Vector2f(0.5f, 0.f));

	ball = sf::CircleShape(ballSize);
	ball.setFillColor(sf::Color::Magenta);
	//ball.setOrigin(sf::Vector2f(0.5f, 0.f));
	

	sf::Font font;
	if (!font.loadFromFile("content/font.ttf"))
	{
		// error...
	}

	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
		}

		// --- Update ---

		ballPosition.y += ballSpeed;
		ball.setPosition(ballPosition);

		if(ballPosition.y + ballSize >= (padPos.y - ballSize))
		{
			if ((ballPosition.x <= (padPos.x + padSize.x)) && (ballPosition.x >= (padPos.x)))
			{
				ballSpeed = -ballSpeed;
			}
		}

		Input(window);


		// --- Render ---
		sf::Color clearClr = sf::Color(0, 0, 0, 255);
		//clearClr = sf::Color(128, 128, 128, 255);
		window.clear(clearClr);
		
		for (int i = 0; i < count; ++i)
		{
			window.draw(sprites[i]);
		}

		window.draw(pad);
		window.draw(ball);


		// --- Debug ---

		sf::Text text;

		std::string debugText;
		debugText = "Ball pos: ";
		debugText += std::to_string(ballPosition.x);
		debugText += "\n";
		debugText +="Pad pos: ";
		debugText += std::to_string(padPos.x);

		text.setFont(font); // font is a sf::Font
		text.setString(debugText.c_str());
		text.setCharacterSize(24); // in pixels, not points!
		text.setFillColor(sf::Color::Red);
		text.setStyle(sf::Text::Bold | sf::Text::Underlined);
		window.draw(text);




		window.display();
	}
}