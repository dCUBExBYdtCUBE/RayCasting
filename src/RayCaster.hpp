#pragma once
#include <SFML/Graphics.hpp>
#include "Player.hpp"
#include "Map.hpp"
#include <vector>

class RayCaster
{
public:
    RayCaster(int screenWidth, int screenHeight);
    
    void castRays(const Player& player, const Map& map);
    void draw(sf::RenderWindow& window);

private:
    sf::Image frameBuffer;
    sf::Texture frameTexture;
    sf::Sprite frameSprite;
    std::vector<sf::Color> wallColors;
    
    // Dash effect parameters
    float dashEffectIntensity;   // Controls the strength of the effect
    float dashEffectSpeed;       // Controls how quickly the effect animates
    float dashEffectTimer;       // Tracks time for dash animation
    
    // Apply dash effect to the rendered frame
    void applyDashEffect(const Player& player);
    void applyMotionBlur(float dirX, float dirY, float strength);
    void drawSlashEffect(int centerX, int centerY, float angle, const std::vector<sf::Color>& colors, float strength);
    void addSlashHighlights(const std::vector<std::pair<float, float>>& slashPath, float width, const std::vector<sf::Color>& colors);
    void drawColoredSlashSegment(float x1, float y1, float x2, float y2, float width, 
        const std::vector<sf::Color>& colors, float posRatio);
    void drawPlayerSword(const Player& player);
};

        
