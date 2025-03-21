// Player.hpp
#pragma once
#include <SFML/System/Vector2.hpp>

class Player
{
private:
    sf::Vector2f position;
    sf::Vector2f direction;
    sf::Vector2f plane;
    float moveSpeed;
    float rotSpeed;

public:
    Player();
    void handleInput(float deltaTime, const sf::Keyboard::Key pressedKeys[]);
    void update(float deltaTime);
    
    // Getters
    sf::Vector2f getPosition() const;
    sf::Vector2f getDirection() const;
    sf::Vector2f getPlane() const;
};