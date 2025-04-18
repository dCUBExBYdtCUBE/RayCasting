#pragma once
#include <SFML/Graphics.hpp>
#include "Player.hpp"
#include "Map.hpp"
#include "RayCaster.hpp"
#include <SFML/System/Clock.hpp>
#include "TextRenderer.hpp"

class Game {
private:
    sf::RenderWindow window;      // The SFML window for rendering
    Player player;                // Player object that handles movement and camera
    Map map;                      // Map object containing the level grid
    RayCaster raycaster;          // RayCaster object for rendering the 3D view
    sf::Clock clock;              // Clock for timing and delta time calculation
    bool isRunning;               // Flag to control the game loop
    sf::Clock targetRespawnClock; 
    int score;                    // Player's score
    TextRenderer textRenderer;    // Text rendering system for UI elements

public:
    // Constructor initializes the game with window dimensions and title
    Game(int width, int height, const std::string& title);
    
    // Main game loop
    void run();
    
    // Process input events from keyboard/mouse
    void handleInput();
    
    // Update game state (player position, etc.) based on elapsed time
    void update(float deltaTime);
    
    void updateScore(int points);
    
    // Update UI elements
    void updateUI();
    
    // Render the current frame
    void render();
};
