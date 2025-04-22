// WeaponRenderer.hpp
#pragma once
#include <SFML/Graphics.hpp>
#include "Player.hpp"

// The WeaponRenderer class defines an interface for rendering weapons
class WeaponRenderer {
public:
    virtual ~WeaponRenderer() = default; // Polymorphism: Ensures proper cleanup of derived classes

    // Abstract method for drawing the weapon
    // Interface Segregation Principle (ISP): Defines a minimal interface for weapon rendering
    // Dependency Inversion Principle (DIP): Depends on an abstraction, not a concrete implementation
    virtual void draw(sf::Image& frameBuffer, const Player& player) = 0; 

    // Open/Closed Principle (OCP): Open for extension by creating new subclasses
};
