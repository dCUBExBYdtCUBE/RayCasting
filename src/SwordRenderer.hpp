// SwordRenderer.hpp
#pragma once
#include "WeaponRenderer.hpp"
#include "ColorManager.hpp"

// The SwordRenderer class provides a specific implementation for rendering a sword
class SwordRenderer : public WeaponRenderer {
public:
    // Overrides the draw method from WeaponRenderer
    // Polymorphism: Allows dynamic behavior for rendering weapons
    // Open/Closed Principle (OCP): Extends WeaponRenderer without modifying it
    void draw(sf::Image& frameBuffer, const Player& player) override;

    // Composition: Uses ColorManager to manage colors for the sword
    ColorManager colorManager; // Dependency Inversion Principle (DIP): Depends on an abstraction for color management
    
void updateColor(const sf::Color& newColor);
};
