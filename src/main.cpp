#include "stdio.h"
#include <iostream>
#include <array>

#include <SFML/Graphics.hpp>
#include <SFML/Audio/Music.hpp>
#include <SFML/Audio/Sound.hpp>
#include <SFML/Audio/SoundBuffer.hpp>


const float WINDOW_WIDTH = 1024.;
const float WINDOW_HEIGHT = 768.;
const float PADDING = 15.;

const int LEVEL_WIDTH = 11;
const int LEVEL_HEIGHT = 5;

enum class GameState { beforeStart, gameRunning, gameLost, gameWon };
GameState gameState;
bool lootFalling = false;
int frameExplosionWillLast = 0;
int destroyableBricksInLevel = 0;

struct playerStats {
	int lives;
	int points;
	int bricks_destroyed;
};

playerStats stats{ 3, 0, 0 };

sf::Sprite powerUp;
sf::Sprite explosion;
sf::Sound brick_hit, brick_destroyed, pad_hit, exploding;


class Ball {
public:
	float m_size;
	sf::Vector2f m_position;
	sf::CircleShape m_shape;
	sf::Vector2f m_direction;
	int m_cooldown = 20;


private:
	float m_offset;

public:

	void init(const float& WINDOW_WIDTH, const float& WINDOW_HEIGHT) {
		m_size = 10.f;
		m_shape = sf::CircleShape(m_size);
		m_position = sf::Vector2f(WINDOW_WIDTH * .5, WINDOW_WIDTH * .5);
		m_shape.setPosition(m_position);
		m_shape.setFillColor(sf::Color::Magenta);
		m_direction = sf::Vector2f(3.f, 3.f);
	}

	void setOffset() {
		m_offset = rand() % 10* .01f;
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

	void destroy() {
		m_size = 0;
		delete &m_shape;
		delete &m_position;
		delete &m_direction;
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
		m_position = sf::Vector2f((WINDOW_WIDTH + m_size.x) * .5f, WINDOW_HEIGHT * .9f);
		m_shape.setPosition(m_position);
	}

	void move(int mouseX) {
		m_position.x = float(mouseX) - m_size.x * .5f;

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
	int m_explosive;
	int m_hp;
	int m_index;

	void init(const float& WINDOW_WIDTH, const float& WINDOW_HEIGHT) {
		m_size = sf::Vector2f(WINDOW_WIDTH / LEVEL_WIDTH - 20.f, WINDOW_HEIGHT * .03f);
		m_shape = sf::RectangleShape(m_size);
		m_hp = rand() % 3 + 1;
		if (rand() % 10 == 1) {
			m_explosive = true;
		}
		else {
			m_explosive = false;
		}

		if (!m_explosive && (rand() % 25 == 1)) {
			m_destroyable = false;
		}
		else {
			m_destroyable = true;
			destroyableBricksInLevel++;
		}
		m_shape.setOutlineThickness(5.f);
		m_visible = true;
		update();
	}

	void update() {

		if (m_hp == 3) {
			m_shape.setFillColor(sf::Color::Blue);
			m_shape.setOutlineColor(sf::Color::Cyan);
		}
		else if (m_hp == 2) {
			m_shape.setFillColor(sf::Color::White);
			m_shape.setOutlineColor(sf::Color::Yellow);
		}
		else if (m_hp == 1) {
			m_shape.setFillColor(sf::Color::Yellow);
			m_shape.setOutlineColor(sf::Color::Green);
		}
		else if (m_hp == 0) {
			destroy();
		}
		if (!m_destroyable) {
			m_shape.setFillColor(sf::Color(128, 0, 255, 255));
		}
		if (m_explosive) {
			m_shape.setFillColor(sf::Color::Red);
		}
	}

	void dropLoot() {
		lootFalling = true;
		powerUp.setPosition(m_position);
	}

	void destroy() {
		brick_destroyed.play();
		stats.bricks_destroyed++;
		if (rand() % 10 == 1) {
			dropLoot();
		}
		
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
		stats.bricks_destroyed = 0;
		destroyableBricksInLevel = 0;
		for (int i = 0; i < LEVEL_WIDTH; ++i) {
			for (int j = 0; j < LEVEL_HEIGHT; ++j)
			{
				int index = (i * LEVEL_HEIGHT) + j;
				Brick brick = Brick();
				brick.init(WINDOW_WIDTH, WINDOW_HEIGHT);
				level[index] = brick;
				level[index].m_shape.setPosition(sf::Vector2f(i * (brick.m_size.x + 20.f) + PADDING, 80.f + j * (brick.m_size.y + 20.f)));
				level[index].m_position = sf::Vector2f(i * (brick.m_size.x + 20.f) + PADDING, 80.f + j * (brick.m_size.y + 20.f));
				level[index].m_index = index;
			}
		}
	}	
	gameState = GameState::beforeStart;

}


void takeInput(const sf::RenderWindow& window)
{
	if (gameState == GameState::beforeStart) {
		if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
			ball.init(WINDOW_WIDTH, WINDOW_HEIGHT);
			gameState = GameState::gameRunning;
		}
	}
	else if (gameState == GameState::gameRunning) {
		int mouseX = sf::Mouse::getPosition(window).x;

		if (mouseX > pad.m_size.x*.5f && mouseX < (WINDOW_WIDTH - pad.m_size.x * .5f)) {
			pad.move(mouseX);
		}
	
		if (lootFalling) {
			if (powerUp.getPosition().x + powerUp.getTexture()->getSize().x*.2f >= pad.m_position.x && powerUp.getPosition().x <= pad.m_position.x + pad.m_size.x) {
				if (powerUp.getPosition().y + powerUp.getTexture()->getSize().y*.2f >= pad.m_position.y && powerUp.getPosition().y + powerUp.getTexture()->getSize().y*.2f <= pad.m_position.y + pad.m_size.y) {
					if (stats.lives < 5)
						stats.lives++;
					lootFalling = false;
				}
			}
			if (powerUp.getPosition().y >= WINDOW_HEIGHT) {
				lootFalling = false;
			}
		}
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


void explode(const Brick& brick) {
	for (int i = 0; i < LEVEL_HEIGHT * LEVEL_WIDTH; ++i) {
		if (level[i].m_destroyable &&
			(level[i].m_index == brick.m_index - 1
				|| level[i].m_index == brick.m_index + 1
				|| level[i].m_index == brick.m_index - LEVEL_HEIGHT
				|| level[i].m_index == brick.m_index + LEVEL_HEIGHT)) {
			level[i].m_hp--;
			level[i].update();
		}
	}
	frameExplosionWillLast = 20;
	explosion.setPosition(brick.m_position - sf::Vector2f(20.f, 20.f));
	exploding.play();
}


bool collideWithBrick(Brick& brick) {

	if (ball.m_position.x + ball.m_size * 2 >= brick.m_position.x && 
		ball.m_position.x <= brick.m_position.x + brick.m_size.x && 
		ball.m_position.y + ball.m_size * 2 >= brick.m_position.y && 
		ball.m_position.y <= brick.m_position.y + brick.m_size.y)
	{
		if (!brick.m_visible) {
			return false;
		}
		if (brick.m_explosive) {
			explode(brick);
		}
		if (brick.m_destroyable) {
			brick.m_hp -= 1;
		}
		brick_hit.play();
		brick.update();

		if (((ball.m_position.x - brick.m_position.x) / brick.m_size.x <= (ball.m_position.y - brick.m_position.y) / brick.m_size.y) && 
			((ball.m_position.x - brick.m_position.x) / brick.m_size.x <= (brick.m_size.y - (ball.m_position.y - brick.m_position.y) / brick.m_size.y))) 
		{
			ball.m_position.y += 5.f;
			ball.m_direction.y = -ball.m_direction.y;
		} else if (((ball.m_position.x - brick.m_position.x) / brick.m_size.x >= (ball.m_position.y - brick.m_position.y) / brick.m_size.y) && 
			((ball.m_position.x - brick.m_position.x) / brick.m_size.x >= (brick.m_size.y - (ball.m_position.y - brick.m_position.y) / brick.m_size.y)))
		{
			ball.m_position.y -= 5.f;
			ball.m_direction.y = -ball.m_direction.y;
		}
		else if (((ball.m_position.x - brick.m_position.x) / brick.m_size.x <= (ball.m_position.y - brick.m_position.y) / brick.m_size.y) && 
			((ball.m_position.x - brick.m_position.x) / brick.m_size.x >= (brick.m_size.y - (ball.m_position.y - brick.m_position.y) / brick.m_size.y))) {
			ball.m_position.x -= 5.f;
			ball.m_direction.x = -ball.m_direction.x;
		}
		else {
			ball.m_position.x += 5.f;
			ball.m_direction.x = -ball.m_direction.x;
		}

		if (ball.m_cooldown <= 0) {
			ball.accelerateDirection();
			ball.m_cooldown = 10;
		}
		return true;
	}
	return false;
}

void collideWithPad() {
	ball.m_direction.y = -ball.m_direction.y;
	ball.m_position.y -= 3.f;
	if (ball.m_cooldown <= 0) {
		ball.accelerateDirection();
		ball.m_cooldown = 10;
	}
	ball.m_shape.setRadius(ball.m_shape.getRadius() * .98f);
	ball.m_size *= .98f;
	brick_hit.play();
}


void moveBall(Brick level[]) {
	if (ball.m_cooldown > 0) {
		ball.m_cooldown--;
	}
	if (ball.m_position.y + ball.m_size * 2 >= pad.m_position.y &&
		ball.m_position.y <= pad.m_position.y &&
		(ball.m_position.x <= (pad.m_position.x + pad.m_size.x)) &&
		(ball.m_position.x + ball.m_size * 2 >= (pad.m_position.x)))
	{
		if (((ball.m_position.x - pad.m_position.x) / pad.m_size.x >= (ball.m_position.y - pad.m_position.y) / pad.m_size.y) &&
			((ball.m_position.x - pad.m_position.x) / pad.m_size.x <= (pad.m_size.y - (ball.m_position.y - pad.m_position.y) / pad.m_size.y))) {
		collideWithPad();
		}
	}
	if (ball.m_position.y >= WINDOW_HEIGHT) {
		ball.destroy();
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
		if (ball.m_cooldown == 0) {
			ball.accelerateDirection();
			ball.m_cooldown = 10;
		}
	}

	for (int i = 0; i < LEVEL_HEIGHT * LEVEL_WIDTH; ++i) {
		if (collideWithBrick(level[i])) {
			stats.points += 50;
		}
	}
	ball.m_position += ball.m_direction;
	ball.m_shape.setPosition(ball.m_position);

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
	// ----- Graphics -----
	sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "###ARCANOID###");
	window.setFramerateLimit(60);
	window.setMouseCursorVisible(false);

	sf::Texture heart;
	if (!heart.loadFromFile("content/heart.png")) {
		return -1;
	}
	powerUp = sf::Sprite(heart);
	powerUp.setScale(sf::Vector2f(.2f, .2f));

	sf::Texture boom;
	if (!boom.loadFromFile("content/explosion.png")) {
		return -1;
	}
	explosion.setTexture(boom);
	explosion.setScale(sf::Vector2f(.4f, .4f));

	for (int i = 0; i < LEVEL_WIDTH; ++i) {
		for (int j = 0; j < LEVEL_HEIGHT; ++j)
		{
			int index = (i * LEVEL_HEIGHT) + j;
			Brick brick = Brick();
			brick.init(WINDOW_WIDTH, WINDOW_HEIGHT);
			level[index] = brick;
			level[index].m_shape.setPosition(sf::Vector2f(i * (brick.m_size.x + 20.f) + PADDING, 80.f + j * (brick.m_size.y + 20.f)));
			level[index].m_position = sf::Vector2f(i * (brick.m_size.x + 20.f) + PADDING, 80.f + j * (brick.m_size.y + 20.f));
			level[index].m_index = index;
		}
	}
	
		
	static sf::Font inGameFont;
	if (!inGameFont.loadFromFile("content/font_ingame.ttf"))
	{
		// error...
	}
	sf::Text startMsg = inGameMsg("START GAME\nWITH CLICK!", inGameFont);


	// ----- Music & Sound -----

	sf::Music music;
	
	if (!music.openFromFile("content/music.wav"))
		return -1; // error
	
	music.play();
	music.setLoop(true);


	sf::SoundBuffer brick_hit_b;
	if (!brick_hit_b.loadFromFile("content/brick_hit.wav")) {
		return -1;
	}
	brick_hit.setBuffer(brick_hit_b);
	
	sf::SoundBuffer brick_destroyed_b;
	if (!brick_destroyed_b.loadFromFile("content/brick_destroyed.wav")) {
		return -1;
	}
	brick_destroyed.setBuffer(brick_destroyed_b);

	sf::SoundBuffer pad_hit_b;
	if (!pad_hit_b.loadFromFile("content/pad_hit.wav")) {
		return -1;
	}
	pad_hit.setBuffer(pad_hit_b);

	sf::SoundBuffer exploding_b;
	if (!exploding_b.loadFromFile("content/explosion.wav")) {
		return -1;
	}
	exploding.setBuffer(exploding_b);

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

		if (lootFalling) {
			powerUp.setPosition(powerUp.getPosition() + sf::Vector2f(0.f, 6.f));
		}

		if (stats.bricks_destroyed == destroyableBricksInLevel) {
			gameState = GameState::gameWon;
		}

		if (frameExplosionWillLast > 0) {
			frameExplosionWillLast--;
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

		if (lootFalling) {
			window.draw(powerUp);
		}

		if (frameExplosionWillLast > 0) {
			window.draw(explosion);
		}
		if (gameState == GameState::beforeStart) {
			window.draw(startMsg);
		} else if (gameState == GameState::gameLost) {
			window.draw(inGameMsg("GAME OVER!\nReset with Esc", inGameFont));
		} else if (gameState == GameState::gameWon) {
			window.draw(inGameMsg("YOU WON!\nReset with Esc", inGameFont));
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