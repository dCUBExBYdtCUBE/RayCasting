// WeaponRenderer.hpp
#pragma once
#include <SFML/Graphics.hpp>
#include "Player.hpp"

class WeaponRenderer {
public:
    virtual ~WeaponRenderer() = default;
    virtual void draw(sf::Image& frameBuffer, const Player& player) = 0;
};
