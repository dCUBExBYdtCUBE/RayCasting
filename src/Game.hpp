#pragma once
#include <SFML/Graphics.hpp>
#include "Player.hpp"
#include "Map.hpp"
#include "RayCaster.hpp"
#include "TextRenderer.hpp"
#include "ColorManager.hpp"

// Forward declarations
class RayCaster;
class TextRenderer;

// The Game class manages the main game loop and delegates specific tasks to other components
class Game {
private:
    sf::RenderWindow window;      // Encapsulation: Hides rendering details
    Player player;                // Composition: Player object handles movement and camera
    Map map;                      // Composition: Map object contains the level grid
    RayCaster* raycaster;         // Composition: RayCaster handles 3D rendering
    sf::Clock clock;              // Encapsulation: Manages timing and delta time calculation
    bool isRunning;               // Encapsulation: Controls the game loop
    sf::Clock targetRespawnClock; // Encapsulation: Manages target respawn timing
    int score;                    // Encapsulation: Tracks the player's score
    TextRenderer* textRenderer;   // Composition: Handles UI text rendering
    
    bool inNegativeDimension = false; // State Management: Tracks the current dimension
    Map normalMap;               // Composition: Stores the normal dimension map
    Map negativeMap;             // Composition: Stores the negative dimension map
    Map* currentMap;             // State Management: Pointer to the active map

    // Portal handling
    bool portalTransitionActive = false; // Encapsulation: Tracks if a portal transition is active
    float portalTransitionTimer = 0.0f;  // Encapsulation: Timer for portal transitions
    float portalTransitionDuration = 1.0f; // Encapsulation: Duration of portal transitions
    ColorManager colorManager; // Composition: Manages colors for UI and game elements

public:
    Game(int width, int height, const std::string& title);
    ~Game();

    void run(); // Single Responsibility Principle: Manages the game loop
    void handleInput(); // SRP: Handles input events
    void update(float deltaTime); // SRP: Updates the game state
    void render(); // SRP: Handles rendering logic
    void switchDimension(); // State Management: Switches between normal and negative dimensions
    void handlePortalInteraction(const Map& map); // Dependency Injection: Interacts with the map
};


