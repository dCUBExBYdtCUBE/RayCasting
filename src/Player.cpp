// Player.cpp
#include "Player.hpp"
#include <SFML/Window/Keyboard.hpp>
#include <cmath>
#include "Map.hpp"

Player::Player()
    : position(5.0f, 5.0f),
      direction(1.0f, 0.0f),  // Initially facing right (positive X)
      plane(0.0f, 0.66f),     // Field of view is 2 * atan(0.66/1.0) ~= 66°
      moveSpeed(2.5f),
      rotSpeed(2.0f)
{
}

void Player::handleInput(float deltaTime, const sf::Keyboard::Key pressedKeys[], const Map& map) {
    float moveStep = moveSpeed * deltaTime;
    sf::Vector2f newPosition = position;
    
    // Move forward
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W)) {
        newPosition.x += direction.x * moveStep;
        newPosition.y += direction.y * moveStep;
    }

    // Move backward
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)) {
        newPosition.x -= direction.x * moveStep;
        newPosition.y -= direction.y * moveStep;
    }

    // Strafe left (move sideways to the left)
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) {
        newPosition.x -= plane.x * moveStep;
        newPosition.y -= plane.y * moveStep;
    }

    // Strafe right (move sideways to the right)
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) {
        newPosition.x += plane.x * moveStep;
        newPosition.y += plane.y * moveStep;
    }

    // Check for collisions and apply sliding behavior
    applyCollisionWithSliding(newPosition, map);

    // Rotate left
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right)) {
        float rotSpeed = this->rotSpeed * deltaTime;
        float oldDirX = direction.x;
        direction.x = direction.x * cos(rotSpeed) - direction.y * sin(rotSpeed);
        direction.y = oldDirX * sin(rotSpeed) + direction.y * cos(rotSpeed);

        float oldPlaneX = plane.x;
        plane.x = plane.x * cos(rotSpeed) - plane.y * sin(rotSpeed);
        plane.y = oldPlaneX * sin(rotSpeed) + plane.y * cos(rotSpeed);
    }

    // Rotate right
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left)) {
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

void Player::applyCollisionWithSliding(const sf::Vector2f& newPosition, const Map& map)
{
    // Collision buffer (keeps player from getting too close to walls)
    const float collisionBuffer = 0.2f;
    
    // Check X-axis movement independently
    sf::Vector2f xMovement = position;
    xMovement.x = newPosition.x;
    
    // Check if there's a wall in the X direction
    int gridPosX = static_cast<int>(xMovement.x + (xMovement.x > position.x ? collisionBuffer : -collisionBuffer));
    int gridPosY = static_cast<int>(position.y);
    
    // If no wall in X direction, allow X movement
    if (!map.isWall(gridPosX, gridPosY)) {
        position.x = xMovement.x;
    }
    
    // Check Y-axis movement independently
    sf::Vector2f yMovement = position;
    yMovement.y = newPosition.y;
    
    // Check if there's a wall in the Y direction
    gridPosX = static_cast<int>(position.x);
    gridPosY = static_cast<int>(yMovement.y + (yMovement.y > position.y ? collisionBuffer : -collisionBuffer));
    
    // If no wall in Y direction, allow Y movement
    if (!map.isWall(gridPosX, gridPosY)) {
        position.y = yMovement.y;
    }
    
    // Additional corner checking to prevent clipping
    // Check corners where player might be close to walls
    for (int checkX = -1; checkX <= 1; checkX += 2) {
        for (int checkY = -1; checkY <= 1; checkY += 2) {
            float cornerX = position.x + checkX * 0.2f;
            float cornerY = position.y + checkY * 0.2f;
            
            int cornerGridX = static_cast<int>(cornerX);
            int cornerGridY = static_cast<int>(cornerY);
            
            if (map.isWall(cornerGridX, cornerGridY)) {
                // Push player away from wall corners
                if (cornerGridX != static_cast<int>(position.x)) {
                    position.x += (position.x < cornerGridX) ? -0.1f : 0.1f;
                }
                if (cornerGridY != static_cast<int>(position.y)) {
                    position.y += (position.y < cornerGridY) ? -0.1f : 0.1f;
                }
            }
        }
    }
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