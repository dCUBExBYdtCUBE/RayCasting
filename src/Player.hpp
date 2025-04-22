// Player.hpp
#pragma once
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Keyboard.hpp>

class Map;

class Player {
private:
    sf::Vector2f position;       // Encapsulation: Hides the player's position
    sf::Vector2f direction;      // Encapsulation: Hides the player's direction
    sf::Vector2f plane;          // Encapsulation: Hides the player's camera plane
    float moveSpeed;             // Encapsulation: Controls the player's movement speed
    float rotSpeed;              // Encapsulation: Controls the player's rotation speed

    // Dash-related variables
    bool isDashing;              // State Management: Tracks whether the player is dashing
    float dashDistance;          // Encapsulation: Stores the dash distance
    float dashDuration;          // Encapsulation: Stores the dash duration
    float dashTimer;             // Encapsulation: Tracks the dash timer
    float dashCooldown;          // Encapsulation: Stores the dash cooldown duration
    float dashCooldownTimer;     // Encapsulation: Tracks the cooldown timer
    sf::Vector2f dashDirection;  // Encapsulation: Stores the dash direction
    bool lastDashTriggered;      // Encapsulation: Tracks single press vs. hold for dashing

    // Teleportation-related variables
    bool isTeleporting = false;  // State Management: Tracks whether the player is teleporting

    int score = 0;               // Encapsulation: Tracks the player's score

    // Collision handling
    void applyCollisionWithSliding(const sf::Vector2f& newPosition, const Map& map); 
    // Dependency Inversion Principle (DIP): Abstracts collision handling with the map

public:
    Player(); // Single Responsibility Principle (SRP): Initializes the player's state

    // Handles player input
    void handleInput(float deltaTime, const sf::Keyboard::Key pressedKeys[], const Map& map); 
    // SRP: Processes player input

    
    // Checks for target hits
    int checkTargetHits(Player& player, Map& map); 
    // Dependency Inversion Principle (DIP): Abstracts target hit detection

    // Updates the player's state
    void update(float deltaTime); // SRP: Updates the player's state (e.g., movement, dashing)

    // Accessors for position, direction, and camera plane
    sf::Vector2f getPosition() const; // Encapsulation: Provides access to the player's position
    sf::Vector2f getDirection() const; // Encapsulation: Provides access to the player's direction
    sf::Vector2f getPlane() const; // Encapsulation: Provides access to the player's camera plane

    // Updates the player's position
    void setPosition(const sf::Vector2f& position); 
    // Open/Closed Principle (OCP): Allows position updates without modifying collision logic

    // Adds points to the player's score
    void addScore(int points) { 
        score += points; 
        // Optional: add visual feedback when score changes
    }

    // Sets the teleporting state
    void setTeleporting(bool teleporting); // State Management: Adjusts behavior during teleportation
    
    // Accessor for the player's score
    int getScore() const { 
        return score; 
    }

    // Dash-related public methods
    bool getIsDashing() const;
    float getDashCooldownPercent() const;  // Returns a value from 0 to 1 for UI display
};



