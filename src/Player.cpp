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
      rotSpeed(2.0f),
      isDashing(false),
      dashDistance(5.0f),     // How far the dash will move the player
      dashDuration(0.3f),     // How long the dash takes in seconds
      dashTimer(0.0f),
      dashCooldown(1.5f),     // Cooldown time between dashes in seconds
      dashCooldownTimer(0.0f),
      lastDashTriggered(false)
{
}

void Player::handleInput(float deltaTime, const sf::Keyboard::Key pressedKeys[], const Map& map) {
    float moveStep = moveSpeed * deltaTime;
    sf::Vector2f newPosition = position;
    
    // Check for dash input
    bool shiftPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LShift) || 
                        sf::Keyboard::isKeyPressed(sf::Keyboard::Key::RShift);
    
    // Trigger dash on key press (not held)
    if (shiftPressed && !lastDashTriggered && dashCooldownTimer <= 0.0f && !isDashing) {
        isDashing = true;
        dashTimer = dashDuration;
        // Store the current direction for the dash
        dashDirection = direction;
    }
    lastDashTriggered = shiftPressed;
    
    // Regular movement if not dashing
    if (!isDashing) {
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
    }

    // Apply dash movement if dashing
    if (isDashing) {
        float dashStep = (dashDistance / dashDuration) * deltaTime;
        newPosition.x += dashDirection.x * dashStep;
        newPosition.y += dashDirection.y * dashStep;
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
    // Update dash timer and state
    if (isDashing) {
        dashTimer -= deltaTime;
        if (dashTimer <= 0.0f) {
            isDashing = false;
            dashCooldownTimer = dashCooldown;
        }
    }
    
    // Update cooldown timer
    if (dashCooldownTimer > 0.0f) {
        dashCooldownTimer -= deltaTime;
    }
}

void Player::applyCollisionWithSliding(const sf::Vector2f& newPosition, const Map& map)
{
    const float collisionBuffer = 0.2f;

    sf::Vector2f xMovement = position;
    xMovement.x = newPosition.x;

    int gridPosX = static_cast<int>(xMovement.x + (xMovement.x > position.x ? collisionBuffer : -collisionBuffer));
    int gridPosY = static_cast<int>(position.y);

    if (!map.isWall(gridPosX, gridPosY)) {
        position.x = xMovement.x;
    } else if (isDashing) {
        // End dash early if hitting a wall
        isDashing = false;
        dashTimer = 0.0f;
        dashCooldownTimer = dashCooldown;
    }

    sf::Vector2f yMovement = position;
    yMovement.y = newPosition.y;

    gridPosX = static_cast<int>(position.x);
    gridPosY = static_cast<int>(yMovement.y + (yMovement.y > position.y ? collisionBuffer : -collisionBuffer));

    if (!map.isWall(gridPosX, gridPosY)) {
        position.y = yMovement.y;
    } else if (isDashing) {
        // End dash early if hitting a wall
        isDashing = false;
        dashTimer = 0.0f;
        dashCooldownTimer = dashCooldown;
    }
    
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
                
                // End dash if hitting a corner during dash
                if (isDashing) {
                    isDashing = false;
                    dashTimer = 0.0f;
                    dashCooldownTimer = dashCooldown;
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

bool Player::getIsDashing() const
{
    return isDashing;
}

float Player::getDashCooldownPercent() const
{
    return dashCooldownTimer / dashCooldown;
}
