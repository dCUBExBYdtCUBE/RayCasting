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
class Game {
private:
    sf::RenderWindow window;      // Encapsulation: Hides rendering details
    Player player;                // Composition: Player object handles movement and camera
    Map map;                      // Composition: Map object contains the level grid
    RayCaster* raycaster;         // Composition: RayCaster handles 3D rendering
    sf::Clock clock;              // Encapsulation: Manages timing and delta time calculation
    bool isRunning;               // Encapsulation: Controls the game loop
    sf::Clock targetRespawnClock; // Encapsulation: Manages target respawn timing
    int score;                    // Encapsulation: Tracks the player's score
    TextRenderer* textRenderer;   // Composition: Handles UI text rendering
    
    bool inNegativeDimension = false; // State Management: Tracks the current dimension
    Map normalMap;               // Composition: Stores the normal dimension map
    Map negativeMap;             // Composition: Stores the negative dimension map
    Map* currentMap;             // State Management: Pointer to the active map

    // Portal handling
    bool portalTransitionActive = false; // Encapsulation: Tracks if a portal transition is active
    float portalTransitionTimer = 0.0f;  // Encapsulation: Timer for portal transitions
    float portalTransitionDuration = 1.0f; // Encapsulation: Duration of portal transitions
    ColorManager colorManager; // Composition: Manages colors for UI and game elements

public:
    // Constructor initializes the game with window dimensions and title
    Game(int width, int height, const std::string& title);
    ~Game();
    
    // Main game loop
    void run(); // Single Responsibility Principle: Manages the game loop
    
    // Process input events from keyboard/mouse
    void handleInput(); // SRP: Handles input events
    
    // Update game state (player position, etc.) based on elapsed time
    void update(float deltaTime); // SRP: Updates the game state
    
    // Render the current frame
    void render(); // SRP: Handles rendering logic

    void switchDimension(); // State Management: Switches between normal and negative dimensions
    void handlePortalInteraction(const Map& map); // Dependency Injection: Interacts with the map
};

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
    ColorManager wallColors;
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

    float distortionAmount = 0.0f;
    bool colorInversionEnabled = false;
    
    
public:
    // Your existing public methods
    RayCaster(int screenWidth, int screenHeight);
    void castRays(const Player& player, const Map& map);
    void draw(sf::RenderWindow& window);

<<<<<<< Updated upstream
<<<<<<< Updated upstream
    const std::vector<TargetHit>& getHitTargets() const { return hitTargets; }
=======
=======
>>>>>>> Stashed changes
    sf::Color invertColor(const sf::Color& color);

    const std::vector<TargetHit>& getTargetHits() const { return hitTargets; }
>>>>>>> Stashed changes
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
<<<<<<< Updated upstream
<<<<<<< Updated upstream
};
=======

    void setDistortionEffect(float amount);
    void setColorInversion(bool enabled);
};
>>>>>>> Stashed changes
=======

    void setDistortionEffect(float amount);
    void setColorInversion(bool enabled);
};
>>>>>>> Stashed changes
