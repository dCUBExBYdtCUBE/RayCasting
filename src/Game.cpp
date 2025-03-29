// Game.cpp
#include "Game.hpp"
#include <SFML/Window/Event.hpp>
#include <SFML/Graphics.hpp>

Game::Game(int width, int height, const std::string& title)
    : window(sf::VideoMode({static_cast<unsigned int>(width), static_cast<unsigned int>(height)}), title),
      player(),
      map(20, 20),
      raycaster(width, height),
      isRunning(true)
{
    window.setFramerateLimit(60);
}

void Game::run()
{
    while (isRunning && window.isOpen())
    {
        handleInput();
        
        float deltaTime = clock.restart().asSeconds();
        update(deltaTime);
        
        render();
    }
}

void Game::handleInput()
{
    while (auto event = window.pollEvent())  // pollEvent() returns std::optional<sf::Event>
    {
        if (event->is<sf::Event::Closed>())
        {
            window.close();
            isRunning = false;
        }
    }

    
    // Pass keyboard state to player
    sf::Keyboard::Key pressedKeys[] = {
        sf::Keyboard::Key::W, sf::Keyboard::Key::A,
        sf::Keyboard::Key::S, sf::Keyboard::Key::D,
        sf::Keyboard::Key::Left, sf::Keyboard::Key::Right
    };
    player.handleInput(clock.getElapsedTime().asSeconds(), pressedKeys, map);
}

void Game::update(float deltaTime)
{
    player.update(deltaTime);
}

void Game::render()
{
    window.clear(sf::Color::Black);
    
    // Cast rays and render the view
    raycaster.castRays(player, map);
    raycaster.draw(window);
    
    window.display();
}