#include "RayCaster.hpp"
#include <cmath>
#include <cstdint>
#include <iostream>

RayCaster::RayCaster(int screenWidth, int screenHeight)
    : frameTexture(sf::Vector2u(static_cast<unsigned int>(screenWidth), 
                 static_cast<unsigned int>(screenHeight))),
      frameSprite(frameTexture),
      dashEffectIntensity(0.8f),       // Increased for stronger effect
      dashEffectSpeed(8.0f),           // Faster animation
      dashEffectTimer(0.0f),
      dashStartTime(0.0f),
      dashDuration(0.4f),              // Total duration of dash effect
      dashActive(false)
{
    // The rest of your constructor
    frameBuffer = sf::Image(sf::Vector2u(static_cast<unsigned int>(screenWidth), 
                          static_cast<unsigned int>(screenHeight)), 
                          sf::Color::Black);
    
    // Initialize wall colors
    // Cyberpunk/Tron color scheme
wallColors = {
    sf::Color(10, 10, 30),      // Type 0: Dark blue-black for floor (usually not used)
    sf::Color(0, 210, 255),     // Type 1: Bright cyan for standard walls
    sf::Color(255, 0, 150),     // Type 2: Neon pink for energy walls
    sf::Color(0, 255, 120),     // Type 3: Electric green for data streams
    sf::Color(255, 230, 0)      // Type 4: Bright yellow (if you add another wall type)
};
    
    // Initialize array for previous positions (for afterimages)
    for (int i = 0; i < 5; i++) {
        previousPlayerPositions.push_back(sf::Vector2f(0, 0));
    }
}

SwordRenderer swordRenderer;
// Add missing easing functions
float RayCaster::easeInOutCubic(float t)
{
    return t < 0.5f ? 4.0f * t * t * t : 1.0f - std::pow(-2.0f * t + 2.0f, 3.0f) / 2.0f;
}

float RayCaster::easeOutQuart(float t) {
    return 1.0f - std::pow(1.0f - t, 4.0f);
}

// Modified drawMovingSlash to accept the isHorizontal parameter
void RayCaster::drawMovingSlash(float dashProgress, int screenWidth, int screenHeight, 
    const sf::Vector2f& playerDir, bool isHorizontal)
{
    // Calculate start and end points for the slash trajectory
    int startX, startY, endX, endY;

    if (isHorizontal) {
        // Horizontal slash - extend fully across the screen
        startY = screenHeight / 2;
        endY = screenHeight / 2;

        // Start from left or right based on player direction
        if (playerDir.x > 0) {
            startX = screenWidth * 0.05f;
            endX = screenWidth * 0.95f;
        } else {
            startX = screenWidth * 0.95f;
            endX = screenWidth * 0.05f;
        }
    } else {
        // Diagonal slash - extend further toward the corners
        startX = screenWidth * 0.05f;
        startY = screenHeight * 0.05f;
        endX = screenWidth * 0.95f;
        endY = screenHeight * 0.95f;

        // Calculate angle to determine diagonal direction
        float angle = std::atan2(playerDir.y, playerDir.x);

        // If player is facing more to the upper-right or lower-left
        if ((angle > -3.14f/4 && angle < 3.14f/4) || 
            (angle > 3.14f*3/4 || angle < -3.14f*3/4)) {
            std::swap(startY, endY);
        }
    }

    // Current position of slash based on progress
    float fCurrentX = startX + (endX - startX) * dashProgress;
    float fCurrentY = startY + (endY - startY) * dashProgress;
    int currentX = static_cast<int>(fCurrentX);
    int currentY = static_cast<int>(fCurrentY);

    // Calculate trail position for disc effect
    float trailLength = 0.1f; // Shorter trail for disc-like effect
    float trailStartProgress = std::max(0.0f, dashProgress - trailLength);
    int trailX = startX + static_cast<int>((endX - startX) * trailStartProgress);
    int trailY = startY + static_cast<int>((endY - startY) * trailStartProgress);

    // Tron colors - blue/cyan theme
    std::vector<sf::Color> discColors = {
        sf::Color(255, 255, 255),    // White core
        sf::Color(150, 220, 255),    // Light blue
        sf::Color(30, 150, 255),     // Medium blue
        sf::Color(5, 50, 150)        // Dark blue edge
    };

    // Calculate slash direction vector
    float slashDirX = static_cast<float>(currentX - trailX);
    float slashDirY = static_cast<float>(currentY - trailY);
    float length = std::sqrt(slashDirX*slashDirX + slashDirY*slashDirY);

    if (length > 0) {
        slashDirX /= length;
        slashDirY /= length;
    }

    // Perpendicular vector for disc width
    float perpX = -slashDirY;
    float perpY = slashDirX;

    // Disc width - consistent for more disc-like appearance
    float discThickness = 30.0f;

    // Draw the main disc shape
    for (float t = 0; t <= 1.0f; t += 0.01f) {
        // Position along the slash line
        float x = trailX + (currentX - trailX) * t;
        float y = trailY + (currentY - trailY) * t;

        // Create circular disc shape
        for (float w = -discThickness; w <= discThickness; w += 0.5f) {
            int drawX = static_cast<int>(x + perpX * w);
            int drawY = static_cast<int>(y + perpY * w);

            if (drawX >= 0 && drawX < screenWidth && drawY >= 0 && drawY < screenHeight) {
                // Distance from center of disc line
                float dist = std::abs(w) / discThickness;
                dist = std::max(0.0f, std::min(1.0f, dist));

                // Select color based on distance from center
                int colorIdx = std::min(static_cast<int>(dist * discColors.size()), 
                            static_cast<int>(discColors.size() - 1));
                sf::Color discColor = discColors[colorIdx];

                // Create sharp edge for Tron disc effect
                float fade = 1.0f;
                if (dist > 0.8f) {
                    // Sharper fall-off at the edges
                    fade = (1.0f - dist) * 5.0f;
                }

                // Get current color and blend
                sf::Color currentColor = frameBuffer.getPixel({static_cast<unsigned int>(drawX), 
                                          static_cast<unsigned int>(drawY)});

                // Core is more additive for brighter effect
                sf::Color finalColor;
                if (dist < 0.4f) {
                    finalColor = sf::Color(
                        static_cast<std::uint8_t>(std::min(255, currentColor.r + static_cast<int>(discColor.r * fade))),
                        static_cast<std::uint8_t>(std::min(255, currentColor.g + static_cast<int>(discColor.g * fade))),
                        static_cast<std::uint8_t>(std::min(255, currentColor.b + static_cast<int>(discColor.b * fade)))
                    );
                } else {
                    finalColor = sf::Color(
                        static_cast<std::uint8_t>(currentColor.r * (1.0f - fade) + discColor.r * fade),
                        static_cast<std::uint8_t>(currentColor.g * (1.0f - fade) + discColor.g * fade),
                        static_cast<std::uint8_t>(currentColor.b * (1.0f - fade) + discColor.b * fade)
                    );
                }

                frameBuffer.setPixel({static_cast<unsigned int>(drawX), static_cast<unsigned int>(drawY)}, finalColor);
            }
        }
    }

    // Add bright flash at the leading edge of the disc
    float flashSize = 35.0f;
    for (float px = -flashSize; px <= flashSize; px += 0.5f) {
        for (float py = -flashSize; py <= flashSize; py += 0.5f) {
            int drawX = static_cast<int>(currentX + px);
            int drawY = static_cast<int>(currentY + py);

            if (drawX >= 0 && drawX < screenWidth && drawY >= 0 && drawY < screenHeight) {
                float dist = std::sqrt(px*px + py*py);
                if (dist <= flashSize) {
                    // Create glowing edge with sharper falloff
                    float fade = 0.0f;
                    if (dist > flashSize * 0.7f) {
                        // Create ring effect at the edge
                        fade = (1.0f - (dist - flashSize * 0.7f) / (flashSize * 0.3f)) * 0.8f;
                    } else if (dist > flashSize * 0.5f) {
                        fade = 0.2f;
                    }

                    sf::Color flashColor(100, 200, 255); // Tron blue glow
                    sf::Color currentColor = frameBuffer.getPixel({static_cast<unsigned int>(drawX), 
                                                          static_cast<unsigned int>(drawY)});

                    // Additive blend for glow
                    sf::Color finalColor = sf::Color(
                        static_cast<std::uint8_t>(std::min(255, currentColor.r + static_cast<int>(flashColor.r * fade))),
                        static_cast<std::uint8_t>(std::min(255, currentColor.g + static_cast<int>(flashColor.g * fade))),
                        static_cast<std::uint8_t>(std::min(255, currentColor.b + static_cast<int>(flashColor.b * fade)))
                    );

                    frameBuffer.setPixel({static_cast<unsigned int>(drawX), 
                                  static_cast<unsigned int>(drawY)}, finalColor);
                }
            }
        }
    }

    // Add a simple trail of blue light behind the disc (simplified particles)
    int trailPoints = 15; // Fewer trail points for simplicity
    for (int i = 0; i < trailPoints; i++) {
        float t = static_cast<float>(i) / trailPoints;
        float fadeT = 1.0f - t; // Fade out as we go back along the trail
        
        // Position along the trail
        float trailPointX = currentX - slashDirX * length * t;
        float trailPointY = currentY - slashDirY * length * t;
        
        // Trail size
        float trailSize = 12.0f * fadeT;
        
        // Draw trail point
        for (float px = -trailSize; px <= trailSize; px += 1.0f) {
            for (float py = -trailSize; py <= trailSize; py += 1.0f) {
                int drawX = static_cast<int>(trailPointX + px);
                int drawY = static_cast<int>(trailPointY + py);
                
                if (drawX >= 0 && drawX < screenWidth && drawY >= 0 && drawY < screenHeight) {
                    float dist = std::sqrt(px*px + py*py);
                    if (dist <= trailSize) {
                        // Fade based on distance from center and position in trail
                        float fade = (1.0f - dist/trailSize) * fadeT * 0.5f;
                        
                        // Trail color - blue for Tron effect
                        sf::Color trailColor(30, 150, 255);
                        sf::Color currentColor = frameBuffer.getPixel({static_cast<unsigned int>(drawX), 
                                                              static_cast<unsigned int>(drawY)});
                        
                        // Blend
                        sf::Color finalColor = sf::Color(
                            static_cast<std::uint8_t>(std::min(255, currentColor.r + static_cast<int>(trailColor.r * fade))),
                            static_cast<std::uint8_t>(std::min(255, currentColor.g + static_cast<int>(trailColor.g * fade))),
                            static_cast<std::uint8_t>(std::min(255, currentColor.b + static_cast<int>(trailColor.b * fade)))
                        );
                        
                        frameBuffer.setPixel({static_cast<unsigned int>(drawX), 
                                      static_cast<unsigned int>(drawY)}, finalColor);
                    }
                }
            }
        }
    }
}

// Make sure to update the applyDashEffect function's slash positions to match too
void RayCaster::applyDashEffect(float dashProgress, float playerDirX, float playerDirY)
{
    applySimpleMotionBlur(playerDirX, playerDirY, 0.3f);

    int screenWidth = frameBuffer.getSize().x;
    int screenHeight = frameBuffer.getSize().y;

    // Create a direction vector for the slash
    sf::Vector2f playerDir(playerDirX, playerDirY);

    // Determine slash orientation based on player direction
    bool isHorizontal = false;

    // Calculate the absolute angle of player direction
    float playerAngle = std::atan2(playerDirY, playerDirX);

    // If player is facing more horizontally than vertically, use horizontal slash
    if (std::abs(std::cos(playerAngle)) > 0.7f) {
        isHorizontal = true;
    }

    // Define slash phases based on progress
    if (dashProgress < 0.15f) {
        float intensity = dashProgress / 0.15f;

        // Edge glow code - blue for Tron effect
        for (int x = 0; x < screenWidth; x++) {
            for (int y = 0; y < screenHeight; y++) {
                // Only affect pixels near the edges
                float edgeDistance = std::min(
                    std::min(static_cast<float>(x), static_cast<float>(y)),
                    std::min(static_cast<float>(screenWidth - x), static_cast<float>(screenHeight - y))
                );

                if (edgeDistance < 40) {
                    float fade = (1.0f - edgeDistance / 40.0f) * intensity * 0.5f;
                    sf::Color currentColor = frameBuffer.getPixel({static_cast<unsigned int>(x), static_cast<unsigned int>(y)});

                    // Blue Tron-like glow instead of green
                    sf::Color newColor(
                        static_cast<std::uint8_t>(std::min(255, currentColor.r + static_cast<int>(30 * fade))),
                        static_cast<std::uint8_t>(std::min(255, currentColor.g + static_cast<int>(100 * fade))),
                        static_cast<std::uint8_t>(std::min(255, currentColor.b + static_cast<int>(200 * fade)))
                    );

                    frameBuffer.setPixel({static_cast<unsigned int>(x), static_cast<unsigned int>(y)}, newColor);
                }
            }
        }

        // Start a faint moving slash - limit how far it goes
        float adjustedProgress = dashProgress / 0.15f * 0.2f; // Only move 20% into the animation
        drawMovingSlash(adjustedProgress, screenWidth, screenHeight, playerDir, isHorizontal);
    }
    else if (dashProgress < 0.85f) {
        // Main slash phase - draw the moving slash
        float adjustedProgress = 0.2f + ((dashProgress - 0.15f) / 0.7f) * 0.8f;
        drawMovingSlash(adjustedProgress, screenWidth, screenHeight, playerDir, isHorizontal);
    }
    else {
        // Fade out phase
        float fadeOutProgress = (dashProgress - 0.85f) / 0.15f;
        float fadeOutIntensity = 1.0f - fadeOutProgress;

        drawMovingSlash(1.0f, screenWidth, screenHeight, playerDir, isHorizontal);

        // Simpler fade-out with fewer particles
        int particleCount = static_cast<int>(10 * fadeOutIntensity);

        int startX, startY, endX, endY;

        if (isHorizontal) {
            // For horizontal slash
            startY = screenHeight / 2;
            endY = screenHeight / 2;

            // Start from left or right based on player direction
            if (playerDirX > 0) {
                startX = screenWidth * 0.05f;
                endX = screenWidth * 0.95f;
            } else {
                startX = screenWidth * 0.95f;
                endX = screenWidth * 0.05f;
            }
        } else {
            // For diagonal slash
            startX = screenWidth * 0.05f;
            startY = screenHeight * 0.05f;
            endX = screenWidth * 0.95f;
            endY = screenHeight * 0.95f;

            // Adjust diagonal direction based on player direction
            float angle = std::atan2(playerDirY, playerDirX);

            // If player is facing more to the upper-right or lower-left
            if ((angle > -3.14f/4 && angle < 3.14f/4) || 
                (angle > 3.14f*3/4 || angle < -3.14f*3/4)) {
                std::swap(startY, endY);
            }
        }

        for (int i = 0; i < particleCount; i++) {
            // Position particles along the slash path
            float t = static_cast<float>(rand() % 100) / 100.0f; // 0 to 1

            // Interpolate between start and end positions
            int particleX = startX + static_cast<int>((endX - startX) * t) + (rand() % 40 - 20);
            int particleY = startY + static_cast<int>((endY - startY) * t) + (rand() % 40 - 20);

            // Particle size varies
            float sizeFactor = 0.7f + 0.3f * t;
            int particleSize = static_cast<int>(6 * fadeOutIntensity * sizeFactor);

            for (int px = -particleSize; px <= particleSize; px++) {
                for (int py = -particleSize; py <= particleSize; py++) {
                    int x = particleX + px;
                    int y = particleY + py;

                    if (x >= 0 && x < screenWidth && y >= 0 && y < screenHeight) {
                        float dist = std::sqrt(px*px + py*py);
                        if (dist <= particleSize) {
                            float brightness = (1.0f - dist/particleSize) * fadeOutIntensity;
                            
                            sf::Color currentColor = frameBuffer.getPixel({static_cast<unsigned int>(x), static_cast<unsigned int>(y)});
                            // Blue Tron colors instead of green
                            sf::Color particleColor(
                                static_cast<std::uint8_t>(std::min(255, currentColor.r + static_cast<int>(30 * brightness))),
                                static_cast<std::uint8_t>(std::min(255, currentColor.g + static_cast<int>(100 * brightness))),
                                static_cast<std::uint8_t>(std::min(255, currentColor.b + static_cast<int>(200 * brightness)))
                            );
                            
                            frameBuffer.setPixel({static_cast<unsigned int>(x), static_cast<unsigned int>(y)}, particleColor);
                        }
                    }
                }
            }
        }
    }
}

// Simple motion blur that's less intensive
void RayCaster::applySimpleMotionBlur(float dirX, float dirY, float strength)
{
    int screenWidth = frameBuffer.getSize().x;
    int screenHeight = frameBuffer.getSize().y;
    
    // Create a copy of the framebuffer to read from
    sf::Image frameBufferCopy = frameBuffer;
    
    // Apply a simple directional blur (less samples, simpler math)
    for (int y = 0; y < screenHeight; y += 2) { // Process every other line for performance
        for (int x = 0; x < screenWidth; x += 2) { // Process every other pixel for performance
            sf::Color originalColor = frameBufferCopy.getPixel({static_cast<unsigned int>(x), static_cast<unsigned int>(y)});
            
            // Sample just one point in the direction of movement
            int blurX = x - static_cast<int>(dirX * 3.0f * strength);
            int blurY = y - static_cast<int>(dirY * 3.0f * strength);
            
            if (blurX >= 0 && blurX < screenWidth && blurY >= 0 && blurY < screenHeight) {
                sf::Color blurColor = frameBufferCopy.getPixel({static_cast<unsigned int>(blurX), static_cast<unsigned int>(blurY)});
                
                // Simple blend
                sf::Color finalColor(
                    static_cast<std::uint8_t>((originalColor.r * 0.7f) + (blurColor.r * 0.3f)),
                    static_cast<std::uint8_t>((originalColor.g * 0.7f) + (blurColor.g * 0.3f)),
                    static_cast<std::uint8_t>((originalColor.b * 0.7f) + (blurColor.b * 0.3f))
                );
                
                frameBuffer.setPixel({static_cast<unsigned int>(x), static_cast<unsigned int>(y)}, finalColor);
                
                // Also set the neighboring pixels to the same color (optimization)
                if (x + 1 < screenWidth) {
                    frameBuffer.setPixel({static_cast<unsigned int>(x + 1), static_cast<unsigned int>(y)}, finalColor);
                }
                if (y + 1 < screenHeight) {
                    frameBuffer.setPixel({static_cast<unsigned int>(x), static_cast<unsigned int>(y + 1)}, finalColor);
                }
                if (x + 1 < screenWidth && y + 1 < screenHeight) {
                    frameBuffer.setPixel({static_cast<unsigned int>(x + 1), static_cast<unsigned int>(y + 1)}, finalColor);
                }
            }
        }
    }
}
void RayCaster::castRays(const Player& player, const Map& map)
{
    int screenWidth = frameBuffer.getSize().x;
    int screenHeight = frameBuffer.getSize().y;
    
    sf::Vector2f pos = player.getPosition();
    sf::Vector2f dir = player.getDirection();
    sf::Vector2f plane = player.getPlane();

    hitTargets.clear();

    static float pulseTimer = 0.0f;
    pulseTimer += 0.016f; // Assuming approximately 60 FPS
    
    // Store player position for afterimages
    // MODIFIED: Separate timer for position tracking
    static float positionTrackTimer = 0.0f;
    static float lastPositionTime = 0.0f;
    
    positionTrackTimer += 0.016f; // Update at 60 FPS
    if (positionTrackTimer - lastPositionTime >= 0.05f) { // Store position every 50ms
        // Shift previous positions
        for (size_t i = previousPlayerPositions.size() - 1; i > 0; i--) {
            previousPlayerPositions[i] = previousPlayerPositions[i - 1];
        }
        previousPlayerPositions[0] = pos;
        lastPositionTime = positionTrackTimer; // Reset only the position tracking timer
    }
    
    // Clear the framebuffer
    for (int x = 0; x < screenWidth; x++) {
        for (int y = 0; y < screenHeight; y++) {
            frameBuffer.setPixel({static_cast<unsigned int>(x), static_cast<unsigned int>(y)}, sf::Color::Black);
        }
    }
    
    // Cyberpunk ceiling - dark with grid effect
    sf::Color ceilingColor(5, 10, 25); // Very dark blue
    if ((int)(pos.x * 2 + 0.5) % 2 == 0 || (int)(pos.y * 2 + 0.5) % 2 == 0) {
        ceilingColor = sf::Color(10, 20, 40); // Slightly lighter for grid effect
    }

    // Cyberpunk floor - dark with grid lines
    sf::Color floorColor(10, 15, 30); // Dark blue
    if ((int)(pos.x * 2 + 0.5) % 2 == 0 || (int)(pos.y * 2 + 0.5) % 2 == 0) {
        floorColor = sf::Color(0, 50, 80); // Brighter blue for grid lines
    }
    
    // Cast rays for each vertical column
    for (int x = 0; x < screenWidth; x++)
    {
        // Calculate ray position and direction
        float cameraX = 2 * x / static_cast<float>(screenWidth) - 1; // x-coordinate in camera space
        sf::Vector2f rayDir(
            dir.x + plane.x * cameraX,
            dir.y + plane.y * cameraX
        );
        
        // Which box of the map we're in
        int mapX = static_cast<int>(pos.x);
        int mapY = static_cast<int>(pos.y);
        
        // Length of ray from current position to next x or y-side
        sf::Vector2f sideDist;
        
        // Length of ray from one x or y-side to next x or y-side
        sf::Vector2f deltaDist(
            std::abs(1 / rayDir.x),
            std::abs(1 / rayDir.y)
        );
        
        // What direction to step in x or y direction (either +1 or -1)
        int stepX, stepY;
        
        // Calculate step and initial sideDist
        if (rayDir.x < 0)
        {
            stepX = -1;
            sideDist.x = (pos.x - mapX) * deltaDist.x;
        }
        else
        {
            stepX = 1;
            sideDist.x = (mapX + 1.0f - pos.x) * deltaDist.x;
        }
        
        if (rayDir.y < 0)
        {
            stepY = -1;
            sideDist.y = (pos.y - mapY) * deltaDist.y;
        }
        else
        {
            stepY = 1;
            sideDist.y = (mapY + 1.0f - pos.y) * deltaDist.y;
        }
        
        bool hit = false;      // Was a wall hit?
        bool targetHit = false; // Was a target hit?
        int side;              // Was a NS or a EW wall hit?
        int wallType = 0;      // What type of wall was hit?
        float targetDist = 0;  // Distance to target if hit
        int targetX = 0, targetY = 0; 
        
        while (!hit)
        {
            // Jump to next map square, either in x-direction, or in y-direction
            if (sideDist.x < sideDist.y)
            {
                sideDist.x += deltaDist.x;
                mapX += stepX;
                side = 0;
            }
            else
            {
                sideDist.y += deltaDist.y;
                mapY += stepY;
                side = 1;
            }
            
            // Check if ray has hit a wall
            wallType = map.getValueAt(mapX, mapY);
            if (wallType > 0)
            {
                hit = true;
            }
            if (map.isTarget(mapX, mapY) && !targetHit) {
                targetHit = true;
                targetX = mapX;
                targetY = mapY;
                
                // Calculate distance to target
                if (side == 0) {
                    targetDist = (mapX - pos.x + (1 - stepX) / 2) / rayDir.x;
                } else {
                    targetDist = (mapY - pos.y + (1 - stepY) / 2) / rayDir.y;
                }
            }
        }
        
        // Calculate distance projected on camera direction
        float perpWallDist;
        if (side == 0)
        {
            perpWallDist = (mapX - pos.x + (1 - stepX) / 2) / rayDir.x;
        }
        else
        {
            perpWallDist = (mapY - pos.y + (1 - stepY) / 2) / rayDir.y;
        }
        
        // Calculate height of line to draw on screen
        int lineHeight = static_cast<int>(screenHeight / perpWallDist);
        
        // Calculate lowest and highest pixel to fill in current stripe
        int drawStart = -lineHeight / 2 + screenHeight / 2;
        if (drawStart < 0) drawStart = 0;
        
        int drawEnd = lineHeight / 2 + screenHeight / 2;
        if (drawEnd >= screenHeight) drawEnd = screenHeight - 1;
        
        // Choose wall color based on wall type
        sf::Color color;
        if (wallType < static_cast<int>(wallColors.size()))
        {
            color = wallColors[wallType];
        }
        else
        {
            color = sf::Color::Magenta; // Default for unknown wall types
        }
        
        // Make color darker for y-sides
        if (side == 1)
        {
            color.r = static_cast<std::uint8_t>(static_cast<float>(color.r) * 0.7f);
            color.g = static_cast<std::uint8_t>(static_cast<float>(color.g) * 0.7f);
            color.b = static_cast<std::uint8_t>(static_cast<float>(color.b) * 0.7f);

        float pulseEffect = 0.15f * sin(pulseTimer * 2.0f) + 0.85f;
        color.r = static_cast<uint8_t>(std::min(255, int(color.r * pulseEffect)));
        color.g = static_cast<uint8_t>(std::min(255, int(color.g * pulseEffect)));
        color.b = static_cast<uint8_t>(std::min(255, int(color.b * pulseEffect)));
                }

        // Apply a light green tinge if player is dashing (optimized - only modify the wall rendering)
        if (player.getIsDashing()) {
            // Calculate a pulsing effect for the dash
            float pulse = 0.5f + 0.5f * std::sin(dashEffectTimer * dashEffectSpeed);
            
            // Add a green tinge to the wall color directly during rendering
            color.g = static_cast<std::uint8_t>(std::min(255, color.g + static_cast<int>(40 * pulse)));
            
            // Add a bit of brightness for a glow effect too
            color.r = static_cast<std::uint8_t>(std::min(255, color.r + static_cast<int>(20 * pulse)));
            color.b = static_cast<std::uint8_t>(std::min(255, color.b + static_cast<int>(20 * pulse)));
        }

    for (int y = drawStart; y < drawEnd; y++) {
        // Original wall color
        sf::Color pixelColor = color;   
        // Add glow effect based on distance from center of wall
        float distFromCenter = std::abs((y - (drawStart + (drawEnd - drawStart) / 2.0f)) / (drawEnd - drawStart));
        float glowIntensity = 0.3f * (1.0f - distFromCenter * distFromCenter);
        
        // Enhance the color's brightness for glow effect
        pixelColor.r = static_cast<uint8_t>(std::min(255, int(pixelColor.r * (1 + glowIntensity))));
        pixelColor.g = static_cast<uint8_t>(std::min(255, int(pixelColor.g * (1 + glowIntensity))));
        pixelColor.b = static_cast<uint8_t>(std::min(255, int(pixelColor.b * (1 + glowIntensity))));
        
        frameBuffer.setPixel({static_cast<unsigned int>(x), static_cast<unsigned int>(y)}, pixelColor);
    }
    if (targetHit) {
        // Calculate height of target to draw on screen
        int targetHeight = static_cast<int>(screenHeight / targetDist);
        
        // Calculate lowest and highest pixel to fill for target
        int targetDrawStart = -targetHeight / 2 + screenHeight / 2;
        if (targetDrawStart < 0) targetDrawStart = 0;
        
        int targetDrawEnd = targetHeight / 2 + screenHeight / 2;
        if (targetDrawEnd >= screenHeight) targetDrawEnd = screenHeight - 1;
        
        // Choose target color - use something eye-catching
        sf::Color targetColor;
        if (map.isHitTarget(targetX, targetY)) {
            targetColor = sf::Color(100, 100, 100); // Gray for hit targets
        } else {
            // Pulsing effect for active targets
            float targetPulse = 0.5f + 0.5f * sin(pulseTimer * 3.0f);
            targetColor = sf::Color(
                255, 
                static_cast<uint8_t>(100 + 155 * targetPulse), 
                0
            ); // Orange/yellow glow
        }
        
        // Make target appear as a vertical cylinder/column
        for (int y = targetDrawStart; y < targetDrawEnd; y++) {
            // Calculate vertical position on the target (0 to 1)
            float targetVPos = (y - targetDrawStart) / static_cast<float>(targetDrawEnd - targetDrawStart);
            
            // Create a circular pattern on the target
            float distFromCenter = std::abs(targetVPos - 0.5f) * 2.0f;
            float circleEffect = 1.0f - distFromCenter * distFromCenter;
            circleEffect = std::max(0.0f, circleEffect);
            
            // Apply circular pattern to color
            sf::Color pixelColor = targetColor;
            pixelColor.r = static_cast<uint8_t>(std::min(255, int(pixelColor.r * circleEffect)));
            pixelColor.g = static_cast<uint8_t>(std::min(255, int(pixelColor.g * circleEffect)));
            pixelColor.b = static_cast<uint8_t>(std::min(255, int(pixelColor.b * circleEffect)));
            
            // Only draw if the target is in front of the wall at this position
            float wallDist = perpWallDist;
            if (targetDist < wallDist || (wallDist == 0)) {

                if (!map.isHitTarget(targetX, targetY) && targetDist < 1.0f) {  // 1.0 unit is the hit distance
                    // Avoid adding duplicates in the same frame
                    bool alreadyAdded = false;
                    for (const auto& existingHit : hitTargets) {
                        if (existingHit.x == targetX && existingHit.y == targetY) {
                            alreadyAdded = true;
                            break;
                        }
                    }
                    
                    if (!alreadyAdded) {
                        TargetHit hit;
                        hit.x = targetX;
                        hit.y = targetY;
                        hit.isNewHit = true;
                        hit.points = map.getTargetPoints(targetX, targetY);
                        hitTargets.push_back(hit);
                        std::cout << "Detected hit for target at: " << targetX << ", " << targetY << std::endl;
                    }
                }
                // Add point value number in the center of the target
                if (targetVPos > 0.45f && targetVPos < 0.55f && circleEffect > 0.8f) {
                    // Get point value
                    int points = map.getTargetPoints(targetX, targetY);
                    // Simple way to show the value - alternating colors based on point value
                    if ((points / 10) % 2 == 0) {
                        pixelColor = sf::Color::White;
                    } else {
                        pixelColor = sf::Color::Black;
                    }
                }
                
                frameBuffer.setPixel({static_cast<unsigned int>(x), static_cast<unsigned int>(y)}, pixelColor);
            }
        }
    }

    }
    
    // Apply dash effect if player is dashing
    if (player.getIsDashing()) {
        // Start a new dash if not already active
        if (!dashActive) {
            dashActive = true;
            dashStartTime = dashEffectTimer;
        }     
        
        // Calculate dash progress
        float dashProgress = (dashEffectTimer - dashStartTime) / dashDuration*1.5f;
        dashProgress = std::min(1.0f, dashProgress);
        
        // Get player direction for the dash effect
        sf::Vector2f playerDir = player.getDirection();
        
        // Apply dash effects with the new signature
        applyDashEffect(dashProgress, playerDir.x, playerDir.y);
        
        // ADDED: Debug visualization for dash progress
        dashProgress = (dashEffectTimer - dashStartTime) / (dashDuration * 1.5f); // Multiply dashDuration by 1.5 to slow it down
        dashProgress = std::min(1.0f, dashProgress); // Ensure it doesn't exceed 1.0
        
        // Draw debug bar
        int debugX = 10;
        int debugY = 10;
        int debugWidth = 100;
        int debugHeight = 10;
        
        // Draw progress bar background
        for (int x = debugX; x < debugX + debugWidth; x++) {
            for (int y = debugY; y < debugY + debugHeight; y++) {
                frameBuffer.setPixel({static_cast<unsigned int>(x), static_cast<unsigned int>(y)}, sf::Color(50, 50, 50));
            }
        }
        
        // Draw progress bar fill
        int fillWidth = static_cast<int>(dashProgress * debugWidth);
        for (int x = debugX; x < debugX + fillWidth; x++) {
            for (int y = debugY; y < debugY + debugHeight; y++) {
                // Color based on phase
                sf::Color phaseColor;
                if (dashProgress < 0.2f) phaseColor = sf::Color::Red;
                else if (dashProgress < 0.7f) phaseColor = sf::Color::Green;
                else phaseColor = sf::Color::Blue;
                
                frameBuffer.setPixel({static_cast<unsigned int>(x), static_cast<unsigned int>(y)}, phaseColor);
            }
        }
    } else {
        // Reset dash status if player stops dashing
        dashActive = false;
        
        // Only draw sword when not dashing
        swordRenderer.draw(frameBuffer, player);
    }
    
    // Update the dash effect timer regardless of dash state
    dashEffectTimer += 0.016f; // Assuming approximately 60 FPS
    
    // Update the texture with our pixel data
    frameTexture.update(frameBuffer);
}

void RayCaster::draw(sf::RenderWindow& window)
{
    window.draw(frameSprite);
}