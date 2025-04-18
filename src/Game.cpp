// Game.cpp
#include "Game.hpp"
#include <SFML/Window/Event.hpp>
#include <SFML/Graphics.hpp>
#include <iostream>
 // add this to Game class

 Game::Game(int width, int height, const std::string& title)
 : window(sf::VideoMode({static_cast<unsigned int>(width), static_cast<unsigned int>(height)}), title),
   player(),
   map(20, 20),
   raycaster(width, height),
   isRunning(true),
   score(0)
{
// Reset targets to initial state
 window.setFramerateLimit(60);

 if (!textRenderer.initialize()) {
    // Handle font loading error
    std::cerr << "Failed to initialize text renderer" << std::endl;
}
score = 0;
map.resetTargets(); 

// Create UI text elements
// TRON-style cyan/blue for main title
textRenderer.createText("title", "RAYCASTER GAME", "default", 24,
    sf::Color(0, 255, 255), sf::Vector2f(width / 2 - 100, 10));

// TRON-style white/blue for score
textRenderer.createText("score", "SCORE: 0", "default", 20,
    sf::Color(170, 230, 255), sf::Vector2f(10, 10));

// TRON-style orange for controls (like the antagonist colors)
textRenderer.createText("controls", "WASD: MOVE | ARROWS: TURN", "default", 16,
    sf::Color(255, 150, 0), sf::Vector2f(10, height - 30));
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
        sf::Keyboard::Key::Left, sf::Keyboard::Key::Right,
        sf::Keyboard::Key::LShift, sf::Keyboard::Key::RShift
    };
    player.handleInput(clock.getElapsedTime().asSeconds(), pressedKeys, map);
}

void Game::update(float deltaTime) {
    // Update player position
    player.update(deltaTime);
    
    // Cast rays and get scene information
    raycaster.castRays(player, map);
    
    // Check for target hits and update score
    const auto& hitTargets = raycaster.getTargetHits();
    
    // Debug info
    std::cout << "Number of hit targets this frame: " << hitTargets.size() << std::endl;
    
    for (const auto& target : hitTargets) {
        std::cout << "Target at (" << target.x << ", " << target.y << "), isNewHit: " << (target.isNewHit ? "true" : "false") << std::endl;
        
        if (target.isNewHit) {
            // Mark target as hit in the map
            bool hitSuccess = map.hitTarget(target.x, target.y);
            std::cout << "Map.hitTarget result: " << (hitSuccess ? "success" : "failed") << std::endl;
            
            // Increment score
            int pointsToAdd = target.points;
            score += pointsToAdd;
            std::cout << "Added " << pointsToAdd << " points, new score: " << score << std::endl;
            
            // Update score display
            textRenderer.updateText("score", "SCORE: " + std::to_string(score));
        }
    }
}
void Game::render()
{
    window.clear(sf::Color::Black);
    
    // Cast rays and render the view
    raycaster.castRays(player, map);
    
    raycaster.draw(window);
    textRenderer.draw(window);
    
    window.display();
}
