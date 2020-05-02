#include "GameOfLife.h"

int GameOfLife::generationsCounter = 0;

GameOfLife::cell::cell(sf::Vector2i coords, sf::Color color)
{
	this->coords.x = coords.x;
	this->coords.y = coords.y;
	this->color = color;
}

GameOfLife::GameOfLife(sf::Vector2i size) : mapSize(size), map(size.x* size.y), mapBuffer(map), threadsAmount(std::thread::hardware_concurrency())
{
	aliveCells.reserve(size.x * size.y);

	sf::Vector2i middlePoint(size.x / 2, size.y / 2);

	getCell(middlePoint.x,     middlePoint.y    ) = 1;
	getCell(middlePoint.x + 1, middlePoint.y + 0) = 1;
	getCell(middlePoint.x + 4, middlePoint.y - 0) = 1;
	getCell(middlePoint.x + 5, middlePoint.y + 0) = 1;
	getCell(middlePoint.x + 6, middlePoint.y + 0) = 1;
	getCell(middlePoint.x + 3, middlePoint.y + 1) = 1;
	getCell(middlePoint.x + 1, middlePoint.y + 2) = 1;
}

void GameOfLife::update()
{
	aliveCells.clear();

	int partSize = (mapSize.x * mapSize.y) / threadsAmount;

	std::vector<std::future<std::vector<Cell>>> tasks;

	for (int i = 0; i < threadsAmount; i++)
	{
		int begin = i * partSize;

		int end;
		if (i == threadsAmount - 1)
			end = mapSize.x * mapSize.y;
		else
			end = (i + 1) * partSize;

		tasks.push_back(std::async(std::launch::async, [this, begin, end, i] {
			return this->updatePart(begin, end, i);
			}
		));
	}

	for (auto&& task : tasks)
	{
		auto alivePart = task.get();
		aliveCells.insert(std::end(aliveCells), std::begin(alivePart), std::end(alivePart));
	}

	//uncomment that and commemt two "for"s above to enable singlethread 
	/*auto alivePart = updatePart(0, mapSize.x * mapSize.y, 1);
	aliveCells.insert(std::end(aliveCells), std::begin(alivePart), std::end(alivePart));*/

	generationsCounter++;

	map.swap(mapBuffer);
}

std::vector<GameOfLife::Cell> GameOfLife::updatePart(int begin, int end, int threadNum)
{
	std::vector<Cell> aliveCells;
	aliveCells.reserve(end - begin);

	for (int i = begin; i < end; i++)
	{
		sf::Vector2i coords = get2dPositon(i);

		int aliveNeighbors = 0;

		for (int neighborX = -1; neighborX <= 1; neighborX++)
		{
			for (int neighborY = -1; neighborY <= 1; neighborY++)
			{
				if (neighborX == 0 && neighborY == 0)
					continue;

				int newX = (neighborX + coords.x + mapSize.x) % mapSize.x;
				int newY = (neighborY + coords.y + mapSize.y) % mapSize.y;

				aliveNeighbors += getCell(newX, newY);
			}
		}

		bool dies = aliveNeighbors == 2 || aliveNeighbors == 3;
		bool lives = aliveNeighbors == 3;
		mapBuffer[i] = map[i] ? dies : lives;

		if (mapBuffer[i])
			aliveCells.push_back(Cell(coords, getThreadColor(threadNum)));
	}

	return aliveCells;
}


void GameOfLife::setCell(int x, int y, bool alive)
{
	x = std::max(std::min(x, static_cast<int>(mapSize.x) - 1), 0);
	y = std::max(std::min(y, static_cast<int>(mapSize.y) - 1), 0);
	this->getCell(x, y) = alive;
	aliveCells.push_back(Cell(sf::Vector2i(x, y), sf::Color::White));
}

int& GameOfLife::getCell(int x, int y)
{
	return map[y * mapSize.x + x];
}

sf::Vector2i GameOfLife::get2dPositon(int index)
{
	return sf::Vector2i(static_cast<int>(index % mapSize.x), static_cast<int>(index / mapSize.x));
}

sf::Color GameOfLife::getThreadColor(int threadNum)
{
	switch (threadNum % 4)
	{
	case 0:
		return sf::Color::Red;
		break;
	case 1:
		return sf::Color::Green;
		break;
	case 2:
		return sf::Color::Blue;
		break;
	case 3:
		return sf::Color::Yellow;
		break;
	default:
		return sf::Color::White;
		break;
	}
}

void GameOfLife::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	sf::RectangleShape temp;
	temp.setSize(sf::Vector2f(1, 1));
	for (const auto& it : aliveCells)
	{
		temp.setFillColor(it.color);
		temp.setPosition(it.coords.x, it.coords.y);
		target.draw(temp, states);
	}
}
