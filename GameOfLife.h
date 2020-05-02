#pragma once
#include "SFML/Graphics.hpp"
#include <vector>
#include <iostream>
#include <future>
#include <thread>

class GameOfLife : public sf::Drawable
{
private:

	struct cell
	{
		cell(sf::Vector2i coords, sf::Color color);
		sf::Vector2i coords;
		sf::Color color;
	};

	using Cell = struct cell;

	std::vector<int> map;
	std::vector<int> mapBuffer;

	const int threadsAmount;
	sf::Vector2i mapSize;
	std::vector<Cell> aliveCells;

public:
	GameOfLife(sf::Vector2i size);
	~GameOfLife() = default;

	void update();
	std::vector<Cell> updatePart(int begin, int end, int threadNum);

	void setCell(int x, int y, bool alive);
	int& getCell(int x, int y);
	sf::Vector2i get2dPositon(int index);
	sf::Color getThreadColor(int threadNum);

	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

	static int generationsCounter;

};

