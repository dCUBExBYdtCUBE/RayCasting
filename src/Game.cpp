// Game.cpp
#include "Game.hpp"

Game::Game(int width, int height, const std::string& title)
    : window(sf::VideoMode(width, height), title),
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
    sf::Event event;
    while (window.pollEvent(event))
    {
        if (event.type == sf::Event::Closed)
        {
            window.close();
            isRunning = false;
        }
    }
    
    // Pass keyboard state to player
    sf::Keyboard::Key pressedKeys[] = {
        sf::Keyboard::W, sf::Keyboard::A,   
        sf::Keyboard::S, sf::Keyboard::D,
        sf::Keyboard::Left, sf::Keyboard::Right
    };
    player.handleInput(clock.getElapsedTime().asSeconds(), pressedKeys);
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