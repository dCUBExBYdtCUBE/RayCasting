#pragma once
#include <SFML/Graphics.hpp>
#include "Player.hpp"
#include "Map.hpp"
#include <vector>
#include "SwordRenderer.hpp"

struct RayHit {
    int mapX, mapY;      // Map coordinates where hit occurred
    float distance;      // Perpendicular distance to the hit point
    int side;            // Was it a NS or EW wall hit? (0 = x-side, 1 = y-side)
    int wallType;        // Type of wall that was hit
    bool isTarget;       // Is this a target?
    bool isNewHit;       // Is this a new target hit?
};

struct TargetHit {
    int x, y;            // Target coordinates
    bool isNewHit;       // Is this a new hit or already registered?
    int points;          // Points for this target
};

class RayCaster {
private:
    // Existing members
    sf::Image frameBuffer;
    sf::Texture frameTexture;
    sf::Sprite frameSprite;
    std::vector<sf::Color> wallColors;
    std::vector<sf::Vector2f> previousPlayerPositions;
    SwordRenderer swordRenderer;
    
    // Modified dash effect properties
    float dashEffectIntensity;
    float dashEffectSpeed;
    float dashEffectTimer;
    float dashStartTime;         // Track when the dash started
    float dashDuration;          // How long the dash effect lasts
    bool dashActive;             // Is dash currently active

    std::vector<TargetHit> hitTargets;

    
    // Methods for slash effects
    void applySimpleMotionBlur(float dirX, float dirY, float strength);
    void drawMovingSlash(float dashProgress, int screenWidth, int screenHeight, const sf::Vector2f& playerDir, bool isHorizontal = false);

    // Ray calculation
    sf::Vector2f calculateRayDirection(int x, int screenWidth, const Player& player);
    RayHit performRayCasting(const sf::Vector2f& rayDir, const sf::Vector2f& playerPos, const Map& map);

    // Rendering functions
    void clearFrameBuffer();
    void renderWalls(int x, const RayHit& hit, int screenWidth, int screenHeight, const Player& player);
    void renderTargets(int x, const RayHit& hit, int screenWidth, int screenHeight, const Map& map);
    void renderFloorAndCeiling(const sf::Vector2f& playerPos);
    void applyDashEffect(float dashProgress, float dirX, float dirY);
    void updateDashEffects(const Player& player);
    void updateAfterimages(const sf::Vector2f& playerPos);
    
    // Helper functions for animation
    float easeInOutCubic(float t);
    float easeOutQuart(float t);
    
public:
    // Your existing public methods
    RayCaster(int screenWidth, int screenHeight);
    void castRays(const Player& player, const Map& map);
    void draw(sf::RenderWindow& window);

    const std::vector<TargetHit>& getHitTargets() const { return hitTargets; }
    void clearHitTargets() { hitTargets.clear(); }
    
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
