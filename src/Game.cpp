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
   isRunning(true),
   score(0)
{
    raycaster = new RayCaster(width, height);
    textRenderer = new TextRenderer();
    // Reset targets to initial state
    window.setFramerateLimit(60);

    switchDimension();
 if (!textRenderer->initialize()) {
    // Handle font loading error
    std::cerr << "Failed to initialize text renderer" << std::endl;
}
score = 0;
map.resetTargets(); 

// Create UI text elements
// TRON-style cyan/blue for main title
textRenderer->createText("title", "RAYCASTER GAME", "default", 24,
    colorManager.getTitleTextColor(), sf::Vector2f(width / 2 - 100, 10));

// TRON-style white/blue for score
textRenderer->createText("score", "SCORE: 0", "default", 20,
    colorManager.getScoreTextColor(), sf::Vector2f(10, 10));

// TRON-style orange for controls (like the antagonist colors)
textRenderer->createText("controls", "WASD: MOVE | ARROWS: TURN", "default", 16,
    colorManager.getControlsTextColor(), sf::Vector2f(10, height - 30));
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

<<<<<<< Updated upstream
<<<<<<< Updated upstream
void Game::update(float deltaTime)
{

std::cout << "Current score: " << score << std::endl;

    player.update(deltaTime);

=======
=======
>>>>>>> Stashed changes
Game::~Game() {
    delete raycaster;
    delete textRenderer;
}


void Game::switchDimension() {
    inNegativeDimension = !inNegativeDimension;
    colorManager.setNegativeDimension(inNegativeDimension);
    // Other dimension switch logic
}

void Game::handlePortalInteraction(const Map& map) {
    // Get the player's current position
    int playerX = static_cast<int>(player.getPosition().x);
    int playerY = static_cast<int>(player.getPosition().y);

    

    // Check if the player is on a portal
    if (map.isPortal(playerX, playerY)) {
        // Switch dimensions
        switchDimension();

        player.setTeleporting(true);

        // Get the portal exit position
        sf::Vector2f exitPosition = map.getPortalExit(inNegativeDimension);
        std::cout << "Portal exit position: " << exitPosition.x << ", " << exitPosition.y << std::endl;

        // Update the player's position to the portal exit
        player.setPosition(exitPosition);

        player.setTeleporting(false);

        std::cout << "Player switched dimensions!" << std::endl;
    }
}

void Game::update(float deltaTime) {
    // Update player position
>>>>>>> Stashed changes
    player.update(deltaTime);

    handlePortalInteraction(map);
    
    // Cast rays and get scene information
    raycaster->castRays(player, map);
    
    // Check for target hits and update score
<<<<<<< Updated upstream
<<<<<<< Updated upstream
    const auto& hitTargets = raycaster.getHitTargets();
=======
=======
>>>>>>> Stashed changes
    const auto& hitTargets = raycaster->getTargetHits();
    
    // Debug info
    // std::cout << "Number of hit targets this frame: " << hitTargets.size() << std::endl;
    
>>>>>>> Stashed changes
    for (const auto& target : hitTargets) {
        if (target.isNewHit) {
            // Mark target as hit in the map
            map.hitTarget(target.x, target.y);
            
            // Increment score
            score += target.points;
            
            // Update score display
            textRenderer->updateText("score", "SCORE: " + std::to_string(score));
        }
    }
}

void Game::render()
{
    window.clear(sf::Color::Black);
    
    // Cast rays and render the view
    raycaster->castRays(player, map);
    
    raycaster->draw(window);
    textRenderer->draw(window);
    
    window.display();
}
