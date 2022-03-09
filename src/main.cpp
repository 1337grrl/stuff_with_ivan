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
sf::Vector2f ballDirection;
float ballSize;

sf::RectangleShape pad;
sf::CircleShape ball;

void ResetGame()
{
	ballPosition = sf::Vector2f(WINDOW_WIDTH*.5, 500.f);
	padPos = sf::Vector2f(WINDOW_WIDTH*.5 - padHalfSize.x, WINDOW_HEIGHT - padSize.y - PADDING);
	ballDirection = sf::Vector2f(0.05f, 0.08f);
	ballSize = 10.f;
}

void takeInput(const sf::RenderWindow& window)
{
	mousePosX = sf::Mouse::getPosition(window).x;

	if (mousePosX > PADDING + padHalfSize.x && mousePosX < (WINDOW_WIDTH - padHalfSize.x - PADDING)) {
		padPos.x = mousePosX - padHalfSize.x;
	}

	pad.setPosition(padPos);

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
	{
		ResetGame();
	}
}

inline sf::Vector2f accelerate(sf::Vector2f& direction) {
	return direction * 1.05f;
}

sf::Text gameOverMsg(const sf::Font& font) {
	sf::Text gameOverMsg;

	std::string msg = "GAME OVER!";
	gameOverMsg.setFont(font); // font is a sf::Font
	gameOverMsg.setString(msg.c_str());
	gameOverMsg.setCharacterSize(120); // in pixels, not points!
	gameOverMsg.setFillColor(sf::Color::Red);
	gameOverMsg.setStyle(sf::Text::Bold);
	gameOverMsg.setPosition(sf::Vector2f(0.f, WINDOW_HEIGHT*.4));

	return gameOverMsg;
}

void moveBall() {
	
	if (ballPosition.y + ballSize >= (padPos.y - ballSize))
	{
		if ((ballPosition.x <= (padPos.x + padSize.x)) && (ballPosition.x >= (padPos.x)))
		{
			ballDirection = accelerate(ballDirection);
			ballDirection.y = -ballDirection.y;
		}
	}
	if (ballPosition.y <= 0) {
		ballDirection = accelerate(ballDirection);
		ballDirection.y = -ballDirection.y;
	}
	if (ballPosition.x <= 0 || ballPosition.x >= WINDOW_WIDTH - ballSize) {
		ballDirection = accelerate(ballDirection);
		ballDirection.x = -ballDirection.x;
	}
	ballPosition += ballDirection;
	ball.setPosition(ballPosition);
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
	if (!font.loadFromFile("content/font_debug.ttf"))
	{
		// error...
	}

	sf::Font gameOverFont;
	if (!gameOverFont.loadFromFile("content/font_game_over.ttf"))
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

		takeInput(window);
		moveBall();

		// --- Render ---
		sf::Color clearClr = sf::Color(0, 0, 0, 255);
		//clearClr = sf::Color(128, 128, 128, 255);
		window.clear(clearClr);
		
		
		if (ballPosition.y >= WINDOW_HEIGHT) {
			window.draw(gameOverMsg(gameOverFont));
		}
		else {
			window.draw(ball);

			for (int i = 0; i < count; ++i)
			{
				window.draw(sprites[i]);
			}

			window.draw(pad);

		}

		// --- Debug ---

		sf::Text text;

		std::string debugText;
		debugText = "Ball pos: ";
		debugText += std::to_string(ballPosition.x);
		debugText += "\n";
		debugText +="Pad pos: ";
		debugText += std::to_string(padPos.x);
		debugText += "\n";
		debugText += "Ball speed: ";
		debugText += std::to_string(abs(ballDirection.x));

		text.setFont(font); // font is a sf::Font
		text.setString(debugText.c_str());
		text.setCharacterSize(24); // in pixels, not points!
		text.setFillColor(sf::Color::Red);
		text.setStyle(sf::Text::Bold | sf::Text::Underlined);
		window.draw(text);




		window.display();
	}
}