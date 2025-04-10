// Player.hpp
#pragma once
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Keyboard.hpp>

class Map;

class Player
{
private:
    sf::Vector2f position;
    sf::Vector2f direction;
    sf::Vector2f plane;
    float moveSpeed;
    float rotSpeed;
    
    // Dash related variables
    bool isDashing;
    float dashDistance;
    float dashDuration;
    float dashTimer;
    float dashCooldown;
    float dashCooldownTimer;
    sf::Vector2f dashDirection;
    bool lastDashTriggered;  // Used to detect single press vs. hold
    
    void applyCollisionWithSliding(const sf::Vector2f& newPosition, const Map& map);

public:
    Player();
    void handleInput(float deltaTime, const sf::Keyboard::Key pressedKeys[], const Map& map);
    void update(float deltaTime);
    sf::Vector2f getPosition() const;
    sf::Vector2f getDirection() const;
    sf::Vector2f getPlane() const;
    
    // Dash-related public methods
    bool getIsDashing() const;
    float getDashCooldownPercent() const;  // Returns a value from 0 to 1 for UI display
};
