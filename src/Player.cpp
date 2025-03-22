// Player.cpp
#include "Player.hpp"
#include <SFML/Window/Keyboard.hpp>
#include <cmath>

Player::Player()
    : position(5.0f, 5.0f),
      direction(1.0f, 0.0f),  // Initially facing right (positive X)
      plane(0.0f, 0.66f),     // Field of view is 2 * atan(0.66/1.0) ~= 66°
      moveSpeed(2.5f),
      rotSpeed(2.0f)
{
}

void Player::handleInput(float deltaTime, const sf::Keyboard::Key pressedKeys[])
{
    // Move forward
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W))
    {
        position.x += direction.x * moveSpeed * deltaTime;
        position.y += direction.y * moveSpeed * deltaTime;
    }
    
    // Move backward
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S))
    {
        position.x -= direction.x * moveSpeed * deltaTime;
        position.y -= direction.y * moveSpeed * deltaTime;
    }
    
    // Rotate left
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A) || 
        sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left))
    {
        float rotSpeed = this->rotSpeed * deltaTime;
        float oldDirX = direction.x;
        direction.x = direction.x * cos(rotSpeed) - direction.y * sin(rotSpeed);
        direction.y = oldDirX * sin(rotSpeed) + direction.y * cos(rotSpeed);
        
        float oldPlaneX = plane.x;
        plane.x = plane.x * cos(rotSpeed) - plane.y * sin(rotSpeed);
        plane.y = oldPlaneX * sin(rotSpeed) + plane.y * cos(rotSpeed);
    }
    
    // Rotate right
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D) || 
        sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right))
    {
        float rotSpeed = -this->rotSpeed * deltaTime;
        float oldDirX = direction.x;
        direction.x = direction.x * cos(rotSpeed) - direction.y * sin(rotSpeed);
        direction.y = oldDirX * sin(rotSpeed) + direction.y * cos(rotSpeed);
        
        float oldPlaneX = plane.x;
        plane.x = plane.x * cos(rotSpeed) - plane.y * sin(rotSpeed);
        plane.y = oldPlaneX * sin(rotSpeed) + plane.y * cos(rotSpeed);
    }
}

void Player::update(float deltaTime)
{
    // Additional player updates can go here
}

sf::Vector2f Player::getPosition() const
{
    return position;
}

sf::Vector2f Player::getDirection() const
{
    return direction;
}

sf::Vector2f Player::getPlane() const
{
    return plane;
}