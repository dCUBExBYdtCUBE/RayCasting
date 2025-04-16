#pragma once
#include <SFML/Graphics.hpp>
#include "Player.hpp"
#include "Map.hpp"
#include <vector>
#include "SwordRenderer.hpp"

class RayCaster {
private:
    // Existing members
    sf::Image frameBuffer;
    sf::Texture frameTexture;
    sf::Sprite frameSprite;
    std::vector<sf::Color> wallColors;
    std::vector<sf::Vector2f> previousPlayerPositions;
    
    // Modified dash effect properties
    float dashEffectIntensity;
    float dashEffectSpeed;
    float dashEffectTimer;
    float dashStartTime;         // Track when the dash started
    float dashDuration;          // How long the dash effect lasts
    bool dashActive;             // Is dash currently active

    
    // Methods for dash effects
    void applyDashEffect(float dashProgress, float playerDirX, float playerDirY);

    void applySimpleMotionBlur(float dirX, float dirY, float strength);

    void drawMovingSlash(float dashProgress, int screenWidth, int screenHeight, const sf::Vector2f& playerDir, bool isHorizontal = false);

    void drawPlayerSword(const Player& player);
    
    // Helper functions for animation
    float easeInOutCubic(float t);
    float easeOutQuart(float t);
    
public:
    // Your existing public methods
    RayCaster(int screenWidth, int screenHeight);
    void castRays(const Player& player, const Map& map);
    void draw(sf::RenderWindow& window);
    
    // Add method to start a dash effect
    void startDash() {
        dashStartTime = dashEffectTimer;
        dashActive = true;
    }
    
    // Add method to check if dash is active
    bool isDashActive() const {
        return dashActive && (dashEffectTimer - dashStartTime < dashDuration);
    }
};
