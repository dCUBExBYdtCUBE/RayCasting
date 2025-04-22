#pragma once
#include <SFML/Graphics.hpp>
#include "Player.hpp"
#include "Map.hpp"
#include <vector>
#include "SwordRenderer.hpp"
#include "RayCaster.hpp"
#include <SFML/System/Clock.hpp>
#include "TextRenderer.hpp"
#include "ColorManager.hpp"

// The Game class manages the main game loop and delegates specific tasks to other components
class Game;

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
    sf::Image frameBuffer;
    sf::Texture frameTexture;
    sf::Sprite frameSprite;
    ColorManager wallColors;
    std::vector<sf::Vector2f> previousPlayerPositions;
    SwordRenderer swordRenderer;

    // Dash effect properties
    float dashEffectIntensity;
    float dashEffectSpeed;
    float dashEffectTimer;
    float dashStartTime;         // Track when the dash started
    float dashDuration;          // How long the dash effect lasts
    bool dashActive;             // Is dash currently active

    std::vector<TargetHit> hitTargets;

    // Methods for rendering and effects
    void applySimpleMotionBlur(float dirX, float dirY, float strength);
    void drawMovingSlash(float dashProgress, int screenWidth, int screenHeight, const sf::Vector2f& playerDir, bool isHorizontal = false);
    sf::Vector2f calculateRayDirection(int x, int screenWidth, const Player& player);
    RayHit performRayCasting(const sf::Vector2f& rayDir, const sf::Vector2f& playerPos, const Map& map);
    void clearFrameBuffer();
    void renderWalls(int x, const RayHit& hit, int screenWidth, int screenHeight, const Player& player);
    void renderTargets(int x, const RayHit& hit, int screenWidth, int screenHeight, const Map& map);
    void renderFloorAndCeiling(const sf::Vector2f& playerPos);
    void applyDashEffect(float dashProgress, float dirX, float dirY);
    void updateDashEffects(const Player& player);
    void updateAfterimages(const sf::Vector2f& playerPos);
    float easeInOutCubic(float t);
    float easeOutQuart(float t);

    float distortionAmount = 0.0f;
    bool colorInversionEnabled = false;

public:
    RayCaster(int screenWidth, int screenHeight);
    void castRays(const Player& player, const Map& map);
    void draw(sf::RenderWindow& window);

    sf::Color invertColor(const sf::Color& color);
    const std::vector<TargetHit>& getTargetHits() const { return hitTargets; }
    void clearHitTargets() { hitTargets.clear(); }

    // Dash-related methods
    void startDash() {
        dashStartTime = dashEffectTimer;
        dashActive = true;
    }
    bool isDashActive() const {
        return dashActive && (dashEffectTimer - dashStartTime < dashDuration);
    }

    // Effects
    void setDistortionEffect(float amount);
    void setColorInversion(bool enabled);
};
