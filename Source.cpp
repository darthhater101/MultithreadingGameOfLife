#include <iostream>
#include <SFML/Graphics.hpp>
#include "GameOfLife.h"
#include <chrono>

int main()
{ 
    const int size = 256;
    const int scale = 3;

    sf::RenderWindow window(sf::VideoMode(size, size), "Game of Life");
    window.setSize(sf::Vector2u(scale * size, scale * size));
    window.setVerticalSyncEnabled(false);
    window.setFramerateLimit(0);

    GameOfLife game(sf::Vector2i(size, size));
    
    bool mouseHeld = false;

    auto start = std::chrono::system_clock::now();
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
            if (event.type == sf::Event::MouseButtonPressed)
            {
                if (event.mouseButton.button == sf::Mouse::Left)
                    mouseHeld = true;
            }
            else if (event.type == sf::Event::MouseButtonReleased)
            {
                if (event.mouseButton.button == sf::Mouse::Left)
                    mouseHeld = false;
            }
        }

        window.clear();

        if (mouseHeld) {
            auto mousePosition = sf::Mouse::getPosition(window);

            int x = (mousePosition.x / static_cast<float>(size * scale)) * size;
            int y = (mousePosition.y / static_cast<float>(size * scale)) * size;

            game.setCell(x, y, true);
        }
        else {
            game.update();
        }

        window.draw(game);

        window.display();

        if (std::chrono::system_clock::now() - start > std::chrono::seconds(10))
        {
            window.close();
            break;
        }
    }

    std::cout << "Number of generations: " << game.generationsCounter << std::endl;

    return 0;
}