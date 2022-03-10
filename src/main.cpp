#include "stdio.h"
#include <iostream>


#include <SFML/Graphics.hpp>
#include <SFML/Audio/Music.hpp>

const float WINDOW_WIDTH = 1024.;
const float WINDOW_HEIGHT = 768.;
const float PADDING = 15.;

enum class GameState { beforeStart, gameRunning, gameLost, gameWon };
GameState gameState;

int mousePosX;

int playerLives;
int points;

sf::Vector2f padPos;
float padSpeed = 0.1f;
const sf::Vector2f padSize = sf::Vector2f(120.f, 10.f);
const sf::Vector2f padHalfSize = padSize * 0.5f;


sf::Vector2f ballPosition;
sf::Vector2f ballDirection;
float ballSize;

sf::RectangleShape pad;
sf::CircleShape ball;

const int LEVEL_WIDTH = 11;
const int LEVEL_HEIGHT = 5;
sf::Sprite* level;
sf::Vector2f brickSize = sf::Vector2f((WINDOW_WIDTH / LEVEL_WIDTH), 30.f);
sf::Texture brickTexture;


sf::Sprite* buildLevel(const sf::Texture& brickTexture) {

	sf::Sprite* sprites = new sf::Sprite[LEVEL_HEIGHT*LEVEL_WIDTH];
	sf::Vector2f brickSize = sf::Vector2f((WINDOW_WIDTH / LEVEL_WIDTH), 30.f);

	for (int i = 0; i < LEVEL_WIDTH; ++i) {
		for (int j = 0; j < LEVEL_HEIGHT; ++j)
		{
			int index = (i*LEVEL_HEIGHT) + j;
			sprites[index] = sf::Sprite(brickTexture);
			sprites[index].setPosition(sf::Vector2f(i*brickSize.x, 80.f + j*brickSize.y));
			sprites[index].setScale(0.15f, 0.13f);
		}
	}

	return sprites;
}

void ResetGame()
{
	ballPosition = sf::Vector2f(WINDOW_WIDTH*.5f, 500.f);
	ball.setPosition(ballPosition);
	padPos = sf::Vector2f(WINDOW_WIDTH*.5f - padHalfSize.x, WINDOW_HEIGHT - padSize.y - PADDING);
	pad.setPosition(padPos);
	ballDirection = sf::Vector2f(0.05f, 0.08f);
	ballSize = 10.f;
	if (gameState != GameState::gameRunning) {
		playerLives = 3;
		points = 0;
	}
	gameState = GameState::beforeStart;
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


sf::Text inGameMsg(std::string msg, const sf::Font& font) {
	sf::Text inGameMsg;

	std::string message = msg;
	inGameMsg.setFont(font); // font is a sf::Font
	inGameMsg.setString(msg.c_str());
	inGameMsg.setCharacterSize(120); // in pixels, not points!
	inGameMsg.setFillColor(sf::Color::Red);
	inGameMsg.setStyle(sf::Text::Bold);
	inGameMsg.setPosition(sf::Vector2f(0.f, WINDOW_HEIGHT*.4f));

	return inGameMsg;
}

inline sf::Vector2f accelerate(sf::Vector2f& direction) {
	return direction * 1.05f;
}

bool collisionDetected(sf::Sprite& brick) {
	if (brick.getColor() == sf::Color(0, 0, 0, 0)) {
		return false;
	}
	if (ballPosition.x >= brick.getPosition().x && ballPosition.x + ballSize <= brick.getPosition().x + brickSize.x) {
		if (ballPosition.y + ballSize >= brick.getPosition().y && ballPosition.y <= brick.getPosition().y + brickSize.y) {
			brick.setColor(sf::Color(0, 0, 0, 0));
			ballDirection.y = -ballDirection.y;
			ballDirection = accelerate(ballDirection);
			return true;
		}
		return false;
	}
	return false;
}

void moveBall(sf::Sprite bricks[]) {
	
	if (ballPosition.y + ballSize >= (padPos.y - ballSize))
	{
		if ((ballPosition.x <= (padPos.x + padSize.x)) && (ballPosition.x >= (padPos.x)))
		{
			ballDirection.y = -ballDirection.y;
			ballDirection = accelerate(ballDirection);
		}
	}
	if (ballPosition.y <= 0) {
		ballDirection.y = -ballDirection.y;
		ballDirection = accelerate(ballDirection);
	}
	if (ballPosition.x <= 0 || ballPosition.x >= WINDOW_WIDTH - ballSize) {
		ballDirection.x = -ballDirection.x;
		ballDirection = accelerate(ballDirection);
	}
	ballPosition += ballDirection;
	ball.setPosition(ballPosition);

	for (int i = 0; i < LEVEL_HEIGHT * LEVEL_WIDTH; ++i) {
		if (collisionDetected(bricks[i])) {
			points += 50;
		}
	}
}


sf::Text displayLives(const sf::Font& font) {
	sf::Text lives;

	std::string msg = "LIVES: ";
	msg += std::to_string(playerLives);
	msg += "\nPOINTS: ";
	msg += std::to_string(points);
	lives.setFont(font); // font is a sf::Font
	lives.setString(msg.c_str());
	lives.setCharacterSize(20); // in pixels, not points!
	lives.setFillColor(sf::Color::Red);
	lives.setStyle(sf::Text::Bold);

	return lives;
}


int main()
{
	sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "###ARCANOID###");
	window.setMouseCursorVisible(false);

	static sf::Texture brickTexture;
	if (!brickTexture.loadFromFile("content/brick_blue.png"))
	{
		std::cout << "Error: unable to load texture";
	}
	level = buildLevel(brickTexture);
	
		
	static sf::Font inGameFont;
	if (!inGameFont.loadFromFile("content/font_ingame.ttf"))
	{
		// error...
	}
	sf::Text startMsg = inGameMsg("START GAME\nWITH CLICK!", inGameFont);


	sf::Music music;
	if (!music.openFromFile("content/music.wav"))
		return -1; // error
	music.play();


	sf::Color clearClr = sf::Color(0, 0, 0, 255);


	ResetGame();

	pad = sf::RectangleShape(padSize);
	pad.setFillColor(sf::Color::Green);
	pad.setPosition(padPos);

	ball = sf::CircleShape(ballSize);
	ball.setFillColor(sf::Color::Magenta);
	ball.setPosition(ballPosition);


	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
		}

		// --- Update ---
		if (gameState == GameState::beforeStart) {
			if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
				gameState = GameState::gameRunning;
			}
		}			
		
		takeInput(window);

		if (gameState == GameState::gameRunning) {
			moveBall(level); 
			if (ballPosition.y >= WINDOW_HEIGHT) {
				playerLives--;
				ResetGame();
				if (playerLives <= 0) {
					gameState = GameState::gameLost;
				}
			}
		}

		// --- Render ---
		
		window.clear(clearClr);			
		window.draw(ball);

		for (int i = 0; i < LEVEL_HEIGHT; ++i) {
			for (int j = 0; j < LEVEL_WIDTH; ++j) {
				window.draw(level[i*LEVEL_WIDTH + j]);

			}
		}


		window.draw(pad);
		window.draw(displayLives(inGameFont));

		if (gameState == GameState::beforeStart) {
			window.draw(startMsg);
		} else if (gameState == GameState::gameLost) {
			window.draw(inGameMsg("GAME OVER!\nReset with Esc", inGameFont));
		} else if (gameState == GameState::gameWon) {
			window.draw(inGameMsg("YOU WON!", inGameFont));
		}

		/* --- Debug ---
		sf::Font font;
		if (!font.loadFromFile("content/font_debug.ttf"))
		{
			// error...
		}
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

		*/


		window.display();
	}
}