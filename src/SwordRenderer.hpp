// SwordRenderer.hpp
#pragma once
#include "WeaponRenderer.hpp"

class SwordRenderer : public WeaponRenderer {
public:
    void draw(sf::Image& frameBuffer, const Player& player) override;
};
