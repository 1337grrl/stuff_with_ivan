#include "stdio.h"
#include <iostream>
#include <array>

#include <SFML/Graphics.hpp>
#include <SFML/Audio/Music.hpp>


const float WINDOW_WIDTH = 1024.;
const float WINDOW_HEIGHT = 768.;
const float PADDING = 15.;

const int LEVEL_WIDTH = 11;
const int LEVEL_HEIGHT = 5;

enum class GameState { beforeStart, gameRunning, gameLost, gameWon };
GameState gameState;


struct playerStats {
	int lives;
	int points;
};

playerStats stats{ 3, 0 };

class Ball {
public:
	float m_size;
	sf::Vector2f m_position;
	sf::CircleShape m_shape;
	sf::Vector2f m_direction;
	int m_cooldown = 5;


private:
	float m_offset;

public:

	void init(const float& WINDOW_WIDTH, const float& WINDOW_HEIGHT) {
		m_size = 10.f;
		m_shape = sf::CircleShape(m_size);
		m_position = sf::Vector2f(WINDOW_WIDTH * .5, WINDOW_WIDTH * .5);
		m_shape.setPosition(m_position);
		m_shape.setFillColor(sf::Color::Magenta);
		m_direction = sf::Vector2f(.05f, .05f);
	}

	void setOffset() {
		m_offset = rand() % 5 * .005f;
	}

	void accelerateDirection() {
		setOffset();
		if (m_direction.x < 0) {
			m_direction.x -= m_offset;
		}
		else {
			m_direction.x += m_offset;
		}		
		if (m_direction.y < 0) {
			m_direction.y -= m_offset;
		}
		else {
			m_direction.y += m_offset;
		}
		m_direction *= 1.001f;
	}

};



class Paddle {
public:
	sf::RectangleShape m_shape;
	sf::Vector2f m_size;
	sf::Vector2f m_position;

	void init(float WINDOW_WIDTH, float WINDOW_HEIGHT) {
		m_size = sf::Vector2f(120.f, 10.f);
		m_shape = sf::RectangleShape(m_size);
		m_shape.setFillColor(sf::Color::Green);
		m_position = sf::Vector2f((WINDOW_WIDTH + m_size.x) * .5, WINDOW_HEIGHT * .9);
	}

	void move(int mouseX) {
		m_position.x = float(mouseX) - m_size.x * .5;

		m_shape.setPosition(m_position);
	}
};



class Brick {
public:
	sf::Vector2f m_size;
	sf::RectangleShape m_shape;
	sf::Vector2f m_position;
	bool m_visible;
	bool m_destroyable;
	int m_hp;

	void init(const float& WINDOW_WIDTH, const float& WINDOW_HEIGHT) {
		m_size = sf::Vector2f(WINDOW_WIDTH / LEVEL_WIDTH - 20.f, WINDOW_HEIGHT * .03f);
		m_shape = sf::RectangleShape(m_size);
		m_hp = rand() % 3 + 1;
		if (m_hp == 3) {
			m_shape.setFillColor(sf::Color::Blue);
			m_shape.setOutlineColor(sf::Color::Cyan);
		}
		else if (m_hp == 2) {
			m_shape.setFillColor(sf::Color::White);
			m_shape.setOutlineColor(sf::Color::Yellow);
		}
		else {
			m_shape.setFillColor(sf::Color::Yellow);
			m_shape.setOutlineColor(sf::Color::Green);
		}
		m_shape.setOutlineThickness(5.f);
		m_visible = true;
		m_destroyable = true;
	}

	void destroy() {
		m_visible = false;
	}
};



Paddle pad = Paddle();
Ball ball = Ball();

Brick level[LEVEL_WIDTH * LEVEL_HEIGHT];


void ResetGame()
{
	ball.init(WINDOW_WIDTH, WINDOW_HEIGHT);
	pad.init(WINDOW_WIDTH, WINDOW_HEIGHT);
	if (gameState != GameState::gameRunning) {
		stats.lives = 3;
		stats.points = 0;
		for (int i = 0; i < LEVEL_HEIGHT * LEVEL_WIDTH; ++i) {
			level[i].m_visible = true;
		}
	}	
	gameState = GameState::beforeStart;

}


void takeInput(const sf::RenderWindow& window)
{
	int mouseX = sf::Mouse::getPosition(window).x;

	if (mouseX > PADDING + pad.m_size.x*.5f && mouseX < (WINDOW_WIDTH - pad.m_size.x * .5f - PADDING)) {
		pad.move(mouseX);
	}
	
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


bool collisionDetected(Brick& brick) {
	if (!brick.m_visible) {
		return false;
	}
	ball.m_cooldown--;
	if (ball.m_position.x + ball.m_size*2 >= brick.m_position.x && ball.m_position.x <= brick.m_position.x + brick.m_size.x) {

		if (ball.m_position.y + ball.m_size*2 >= brick.m_position.y && ball.m_position.y <= brick.m_position.y + brick.m_size.y) {
			if (brick.m_destroyable) {
				brick.m_hp -= 1;
				if (brick.m_hp == 2) {
					brick.m_shape.setFillColor(sf::Color::White);
					brick.m_shape.setOutlineColor(sf::Color::Yellow);
				}
				else if (brick.m_hp == 1 ){
					brick.m_shape.setFillColor(sf::Color::Yellow);
					brick.m_shape.setOutlineColor(sf::Color::Green);
				}
				else {
					brick.destroy();
				}
			}
			ball.m_direction.y = -ball.m_direction.y;
			if (ball.m_cooldown <= 0) {
				ball.accelerateDirection();
				ball.m_cooldown = 5;
			}
			return true;
		}
		return false;
	}
	return false;
}


void moveBall(Brick level[]) {
		if (ball.m_position.y + ball.m_size >= pad.m_position.y - ball.m_size)
		{
			if ((ball.m_position.x <= (pad.m_position.x + pad.m_size.x)) && (ball.m_position.x + ball.m_size*2 >= (pad.m_position.x)))
			{
				ball.m_direction.y = -ball.m_direction.y;
				if (ball.m_cooldown <= 0) {
					ball.accelerateDirection();
					ball.m_cooldown = 10;
				}
				ball.m_shape.setRadius(ball.m_shape.getRadius() * .95f);
				ball.m_size *= .95f;
			}
		}
		if (ball.m_position.y <= 0) {
			ball.m_direction.y = -ball.m_direction.y;
			if (ball.m_cooldown <= 0) {
				ball.accelerateDirection();
				ball.m_cooldown = 10;
			}
		}
		if (ball.m_position.x <= 0 || ball.m_position.x >= WINDOW_WIDTH - ball.m_size) {
			ball.m_direction.x = -ball.m_direction.x;
			if (ball.m_cooldown <= 0) {
				ball.accelerateDirection();
				ball.m_cooldown = 10;
			}
		}

		for (int i = 0; i < LEVEL_HEIGHT * LEVEL_WIDTH; ++i) {
			if (collisionDetected(level[i])) {
				stats.points += 50;
			}
		}
	ball.m_position += ball.m_direction;
	ball.m_shape.setPosition(ball.m_position);
	if (ball.m_cooldown > 0) {
		ball.m_cooldown--;
	}
}


sf::Text displayLives(const sf::Font& font) {
	sf::Text lives;

	std::string msg = "LIVES: ";
	msg += std::to_string(stats.lives);
	msg += "\nPOINTS: ";
	msg += std::to_string(stats.points);
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

	for (int i = 0; i < LEVEL_WIDTH; ++i) {
		for (int j = 0; j < LEVEL_HEIGHT; ++j)
		{
			int index = (i * LEVEL_HEIGHT) + j;
			Brick brick = Brick();
			brick.init(WINDOW_WIDTH, WINDOW_HEIGHT);
			level[index] = brick;
			level[index].m_shape.setPosition(sf::Vector2f(i * (brick.m_size.x + 20.f) + PADDING, 80.f + j * (brick.m_size.y + 20.f)));
			level[index].m_position = sf::Vector2f(i * (brick.m_size.x + 20.f) + PADDING, 80.f + j * (brick.m_size.y + 20.f));
		}
	}
	level[18].m_destroyable = true;
	level[49].m_destroyable = true;
	level[4].m_destroyable = true;
	
		
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


	ResetGame();




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
			if (ball.m_position.y >= WINDOW_HEIGHT) {
				stats.lives--;
				ResetGame();
				if (stats.lives <= 0) {
					gameState = GameState::gameLost;
				}
			}
		}

		// --- Render ---

		sf::Color clearClr = sf::Color(0, 0, 0, 255);


		window.clear(clearClr);			
		window.draw(ball.m_shape);

		for (int i = 0; i < LEVEL_HEIGHT; ++i) {
			for (int j = 0; j < LEVEL_WIDTH; ++j) {
				if (level[i * LEVEL_WIDTH + j].m_visible) {
					window.draw(level[i*LEVEL_WIDTH + j].m_shape);
				}
			}
		}


		window.draw(pad.m_shape);
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