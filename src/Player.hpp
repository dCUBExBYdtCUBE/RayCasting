// Player.hpp
#pragma once
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Keyboard.hpp>

class Map; // Forward declaration

class Player
{
private:
    sf::Vector2f position;
    sf::Vector2f direction;
    sf::Vector2f plane;
    float moveSpeed;
    float rotSpeed;

    // New method to handle collisions with sliding
    void applyCollisionWithSliding(const sf::Vector2f& newPosition, const Map& map);

public:
    Player();
    void handleInput(float deltaTime, const sf::Keyboard::Key pressedKeys[], const Map& map);
    void update(float deltaTime);
    
    // Getters
    sf::Vector2f getPosition() const;
    sf::Vector2f getDirection() const;
    sf::Vector2f getPlane() const;
};