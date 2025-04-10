#include "RayCaster.hpp"
#include <cmath>
#include <cstdint>

RayCaster::RayCaster(int screenWidth, int screenHeight)
    : frameTexture(sf::Vector2u(static_cast<unsigned int>(screenWidth), 
                 static_cast<unsigned int>(screenHeight))),
      frameSprite(frameTexture),  // Initialize sprite with texture
      dashEffectIntensity(0.6f),  // Increased slightly for more pronounced effect
      dashEffectSpeed(6.0f),      // Slowed down a bit to reduce visual noise
      dashEffectTimer(0.0f)       // Timer for the dash effect animation
{
    // The rest of your constructor
    frameBuffer = sf::Image(sf::Vector2u(static_cast<unsigned int>(screenWidth), 
                          static_cast<unsigned int>(screenHeight)), 
                          sf::Color::Black);
    
    // Initialize wall colors
    wallColors = {
        sf::Color::White,         // Not used (wall type 0 is empty space)
        sf::Color(220, 100, 100), // Wall type 1 (red)
        sf::Color(100, 220, 100), // Wall type 2 (green)
        sf::Color(100, 100, 220), // Wall type 3 (blue)
        sf::Color(220, 220, 100)  // Wall type 4 (yellow)
    };
}

void RayCaster::castRays(const Player& player, const Map& map)
{
    int screenWidth = frameBuffer.getSize().x;
    int screenHeight = frameBuffer.getSize().y;
    
    sf::Vector2f pos = player.getPosition();
    sf::Vector2f dir = player.getDirection();
    sf::Vector2f plane = player.getPlane();
    
    // Clear the framebuffer
    for (int x = 0; x < screenWidth; x++)
    {
        for (int y = 0; y < screenHeight; y++)
        {
            frameBuffer.setPixel({static_cast<unsigned int>(x), static_cast<unsigned int>(y)}, sf::Color::Black);
        }
    }
    
    // Draw a simple floor/ceiling
    for (int y = 0; y < screenHeight; y++)
    {
        if (y < screenHeight / 2)
        {
            // Ceiling - darker
            sf::Color ceilingColor(50, 50, 70);
            for (int x = 0; x < screenWidth; x++)
            {
                frameBuffer.setPixel({static_cast<unsigned int>(x), static_cast<unsigned int>(y)}, ceilingColor);
            }
        }
        else
        {
            // Floor - lighter
            sf::Color floorColor(70, 70, 50);
            for (int x = 0; x < screenWidth; x++)
            {
                frameBuffer.setPixel({static_cast<unsigned int>(x), static_cast<unsigned int>(y)}, floorColor);
            }
        }
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
        
        // Perform DDA (Digital Differential Analysis)
        bool hit = false;   // Was a wall hit?
        int side;           // Was a NS or a EW wall hit?
        int wallType = 0;   // What type of wall was hit?
        
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
        
        // Draw the pixels of the stripe as a vertical line
        for (int y = drawStart; y < drawEnd; y++)
        {
            frameBuffer.setPixel({static_cast<unsigned int>(x), static_cast<unsigned int>(y)}, color);
        }
    }
    
    // Apply dash effect if player is dashing - using an optimized version
    // In your castRays method, after all effects
if (player.getIsDashing()) {
    applyDashEffect(player);
} else {
    drawPlayerSword(player); // Only draw sword when not dashing
}
    
    // Update the dash effect timer (even when not dashing, for smooth transitions)
    dashEffectTimer += 0.016f; // Assuming approximately 60 FPS
    
    // Update the texture with our pixel data
    frameTexture.update(frameBuffer);
}

// Add this method to RayCaster class
// Add this method to RayCaster class
void RayCaster::drawPlayerSword(const Player& player)
{
    int screenWidth = frameBuffer.getSize().x;
    int screenHeight = frameBuffer.getSize().y;
    
    // Draw sword in bottom right corner
    int swordWidth = screenWidth * 0.3f;  // Size of sword on screen
    int swordHeight = screenHeight * 0.2f;
    int swordPosX = screenWidth - swordWidth - 10;
    int swordPosY = screenHeight - swordHeight - 10;
    
    // Draw simple sword shape
    // Blade
    for (int y = 0; y < swordHeight * 0.7f; y++) {
        for (int x = swordWidth * 0.4f; x < swordWidth * 0.6f; x++) {
            int px = swordPosX + x;
            int py = swordPosY + y;
            if (px >= 0 && px < screenWidth && py >= 0 && py < screenHeight) {
                frameBuffer.setPixel({static_cast<unsigned int>(px), static_cast<unsigned int>(py)}, 
                    sf::Color(220, 220, 220)); // Silver color
            }
        }
    }
    
    // Hilt
    for (int y = swordHeight * 0.7f; y < swordHeight * 0.9f; y++) {
        for (int x = swordWidth * 0.3f; x < swordWidth * 0.7f; x++) {
            int px = swordPosX + x;
            int py = swordPosY + y;
            if (px >= 0 && px < screenWidth && py >= 0 && py < screenHeight) {
                frameBuffer.setPixel({static_cast<unsigned int>(px), static_cast<unsigned int>(py)}, 
                    sf::Color(180, 130, 40)); // Gold-ish color
            }
        }
    }
    
    // Pommel
    for (int y = swordHeight * 0.9f; y < swordHeight; y++) {
        for (int x = swordWidth * 0.4f; x < swordWidth * 0.6f; x++) {
            int px = swordPosX + x;
            int py = swordPosY + y;
            if (px >= 0 && px < screenWidth && py >= 0 && py < screenHeight) {
                frameBuffer.setPixel({static_cast<unsigned int>(px), static_cast<unsigned int>(py)}, 
                    sf::Color(200, 150, 50)); // Dark gold color
            }
        }
    }
}

void RayCaster::applyDashEffect(const Player& player)
{
    int screenWidth = frameBuffer.getSize().x;
    int screenHeight = frameBuffer.getSize().y;
    
    // Center point for the slash effect
    int centerX = screenWidth / 2;
    int centerY = screenHeight / 2;
    
    // Get player's movement direction for slash orientation
    sf::Vector2f playerDir = player.getDirection();
    float playerDirX = playerDir.x;
    float playerDirY = playerDir.y;
    float playerAngle = std::atan2(playerDirY, playerDirX);
    
    // Adjust effect strength based on dash intensity
    float effectStrength = dashEffectIntensity * 2.0f * (0.8f + 0.4f * std::sin(dashEffectTimer * dashEffectSpeed * 1.5f));
    
    // Apply motion blur first (subtle streaking in the direction of movement)
    applyMotionBlur(playerDirX, playerDirY, effectStrength);
    
    // Define colors for the slash - green palette similar to Genji's dragon blade
    // Modify the slashColors in applyDashEffect
const std::vector<sf::Color> slashColors = {
    sf::Color(255, 255, 255),  // White core
    sf::Color(200, 255, 230),  // Light teal 
    sf::Color(140, 255, 180),  // Bright teal green
    sf::Color(80, 230, 120)    // Medium green
};
    
    // Calculate slash angle - perpendicular to movement for a slashing effect
    // Add oscillation for dynamic slash angle
    float slashAngle = playerAngle + 1.57f + 0.2f * std::sin(dashEffectTimer * 3.0f);
    
    // Draw the main slash
    drawSlashEffect(centerX, centerY, slashAngle, slashColors, effectStrength);
}

// Add this method to create afterimages during dash


// Apply motion blur in the direction of movement
void RayCaster::applyMotionBlur(float dirX, float dirY, float strength)
{
    int screenWidth = frameBuffer.getSize().x;
    int screenHeight = frameBuffer.getSize().y;
    
    // Create a copy of the framebuffer for reading (to avoid artifacts)
    sf::Image frameBufferCopy = frameBuffer;
    
    // Parameters for blur amount
    const int maxBlurDistance = std::max(4, static_cast<int>(strength * 12.0f));
    const float blurStrength = std::min(0.7f, strength * 0.4f);
    
    // Apply directional blur
    for (int y = 0; y < screenHeight; y++) {
        for (int x = 0; x < screenWidth; x++) {
            // Get original color
            sf::Color originalColor = frameBufferCopy.getPixel({static_cast<unsigned int>(x), static_cast<unsigned int>(y)});
            sf::Color blurredColor = originalColor;
            float totalWeight = 1.0f;
            
            // Sample in the direction of movement
            for (int i = 1; i <= maxBlurDistance; i++) {
                // Calculate sample position in direction of movement
                int sampleX = x - static_cast<int>(dirX * i);
                int sampleY = y - static_cast<int>(dirY * i);
                
                // Check bounds
                if (sampleX >= 0 && sampleX < screenWidth && 
                    sampleY >= 0 && sampleY < screenHeight) {
                    
                    // Weight decreases with distance
                    float weight = blurStrength * (1.0f - static_cast<float>(i) / maxBlurDistance);
                    totalWeight += weight;
                    
                    // Get sample color and blend
                    sf::Color sampleColor = frameBufferCopy.getPixel({static_cast<unsigned int>(sampleX), static_cast<unsigned int>(sampleY)});
                    blurredColor.r += static_cast<std::uint8_t>(sampleColor.r * weight);
                    blurredColor.g += static_cast<std::uint8_t>(sampleColor.g * weight);
                    blurredColor.b += static_cast<std::uint8_t>(sampleColor.b * weight);
                }
            }
            
            // Normalize colors
            blurredColor.r = static_cast<std::uint8_t>(blurredColor.r / totalWeight);
            blurredColor.g = static_cast<std::uint8_t>(blurredColor.g / totalWeight);
            blurredColor.b = static_cast<std::uint8_t>(blurredColor.b / totalWeight);
            
            // Write back to buffer
            frameBuffer.setPixel({static_cast<unsigned int>(x), static_cast<unsigned int>(y)}, blurredColor);
        }
    }
}

// Draw the Genji-style slash effect
void RayCaster::drawSlashEffect(int centerX, int centerY, float angle, const std::vector<sf::Color>& colors, float strength)
{
    int screenWidth = frameBuffer.getSize().x;
    int screenHeight = frameBuffer.getSize().y;
    
    // Slash parameters
    float slashLength = std::min(screenWidth, screenHeight) * 0.9f * strength;
    float slashWidth = std::min(screenWidth, screenHeight) * 0.15f * strength;
    
    // Calculate the slash direction vectors
    float slashDirX = std::cos(angle);
    float slashDirY = std::sin(angle);
    
    // Calculate perpendicular vectors for slash width
    float perpX = -slashDirY;
    float perpY = slashDirX;
    
    // Create a dynamic slash shape with curve
    const int slashSegments = 20;
    
    // Calculate slash animation progress (0 to 1)
    float animProgress = std::min(1.0f, dashEffectTimer / 0.2f);
    
    // Dynamic slash path parameters
    std::vector<std::pair<float, float>> slashPath;
    for (int i = 0; i <= slashSegments; i++) {
        float t = static_cast<float>(i) / slashSegments;
        
        // Apply animation timing - quick slash appearance
        float animatedT = t;
        if (animProgress < 1.0f) {
            animatedT = t * animProgress;
        }
        
        // Curved slash path - sweep from right to left (or based on angle)
        float pathX = centerX + (slashDirX * slashLength * (animatedT - 0.5f));
        float pathY = centerY + (slashDirY * slashLength * (animatedT - 0.5f));
        
        // Add some curve to the slash path
        float curveAmount = slashWidth * 0.6f * std::sin(t * 3.14159f);
        pathX += perpX * curveAmount;
        pathY += perpY * curveAmount;
        
        slashPath.push_back({pathX, pathY});
    }
    
    // Draw the slash with dynamic thickness
    for (size_t i = 0; i < slashPath.size() - 1; i++) {
        float x1 = slashPath[i].first;
        float y1 = slashPath[i].second;
        float x2 = slashPath[i + 1].first;
        float y2 = slashPath[i + 1].second;
        
        // Calculate position along slash (0 to 1)
        float posRatio = static_cast<float>(i) / (slashPath.size() - 2);
        
        // Vary thickness - thicker in middle, thinner at ends
        float thicknessVariation = std::sin(posRatio * 3.14159f);
        float segmentWidth = slashWidth * (0.2f + 0.8f * thicknessVariation);
        
        // Draw thick line segment with colored gradient
        drawColoredSlashSegment(x1, y1, x2, y2, segmentWidth, colors, posRatio);
    }
    
    // Add highlight details to make it pop
    addSlashHighlights(slashPath, slashWidth, colors);
}

// Draw a single segment of the slash with color gradient
void RayCaster::drawColoredSlashSegment(float x1, float y1, float x2, float y2, float width, 
                                    const std::vector<sf::Color>& colors, float posRatio)
{
    int screenWidth = frameBuffer.getSize().x;
    int screenHeight = frameBuffer.getSize().y;
    
    // Direction vector of this segment
    float dirX = x2 - x1;
    float dirY = y2 - y1;
    float segmentLength = std::sqrt(dirX * dirX + dirY * dirY);
    
    // Normalize direction
    if (segmentLength > 0) {
        dirX /= segmentLength;
        dirY /= segmentLength;
    }
    
    // Perpendicular vector for width
    float perpX = -dirY;
    float perpY = dirX;
    
    // Draw the line with Bresenham's algorithm
    int ix1 = static_cast<int>(x1);
    int iy1 = static_cast<int>(y1);
    int ix2 = static_cast<int>(x2);
    int iy2 = static_cast<int>(y2);
    
    int deltaX = std::abs(ix2 - ix1);
    int deltaY = std::abs(iy2 - iy1);
    int signX = ix1 < ix2 ? 1 : -1;
    int signY = iy1 < iy2 ? 1 : -1;
    int error = deltaX - deltaY;
    int x = ix1;
    int y = iy1;
    
    while (x != ix2 || y != iy2) {
        // Calculate gradient colors based on position
        sf::Color innerColor = colors[0]; // Brightest inner color
        sf::Color outerColor = colors[colors.size() - 1]; // Darkest outer color
        
        // Draw perpendicular line to create thickness
        for (float w = -width; w <= width; w += 0.5f) {
            int px = static_cast<int>(x + perpX * w);
            int py = static_cast<int>(y + perpY * w);
            
            if (px >= 0 && px < screenWidth && py >= 0 && py < screenHeight) {
                // Calculate fade based on distance from center of slash
                float widthRatio = std::abs(w) / width;
                float colorIndex = widthRatio * (colors.size() - 1);
                int colorIdx1 = static_cast<int>(colorIndex);
                int colorIdx2 = std::min(colorIdx1 + 1, static_cast<int>(colors.size() - 1));
                float colorBlend = colorIndex - colorIdx1;
                
                // Blend between adjacent colors
                sf::Color segmentColor(
                    static_cast<std::uint8_t>(colors[colorIdx1].r * (1 - colorBlend) + colors[colorIdx2].r * colorBlend),
                    static_cast<std::uint8_t>(colors[colorIdx1].g * (1 - colorBlend) + colors[colorIdx2].g * colorBlend),
                    static_cast<std::uint8_t>(colors[colorIdx1].b * (1 - colorBlend) + colors[colorIdx2].b * colorBlend)
                );
                
                // Add brightness variation along the length of slash for shine effect
                float shineFactor = 1.0f + 0.3f * std::sin(posRatio * 6.28f + dashEffectTimer * 5.0f);
                segmentColor.r = static_cast<std::uint8_t>(std::min(255, static_cast<int>(segmentColor.r * shineFactor)));
                segmentColor.g = static_cast<std::uint8_t>(std::min(255, static_cast<int>(segmentColor.g * shineFactor)));
                segmentColor.b = static_cast<std::uint8_t>(std::min(255, static_cast<int>(segmentColor.b * shineFactor)));
                
                // Apply fade based on width position - smoother edges
                float fade = 1.0f - std::pow(widthRatio, 0.7f);
                
                // Get current pixel color and blend
                sf::Color currentColor = frameBuffer.getPixel({static_cast<unsigned int>(px), static_cast<unsigned int>(py)});
                
                sf::Color finalColor(
                    static_cast<std::uint8_t>(currentColor.r * (1.0f - fade) + segmentColor.r * fade),
                    static_cast<std::uint8_t>(currentColor.g * (1.0f - fade) + segmentColor.g * fade),
                    static_cast<std::uint8_t>(currentColor.b * (1.0f - fade) + segmentColor.b * fade)
                );
                
                frameBuffer.setPixel({static_cast<unsigned int>(px), static_cast<unsigned int>(py)}, finalColor);
            }
        }
        
        // Bresenham's algorithm step
        int e2 = 2 * error;
        if (e2 > -deltaY) {
            error -= deltaY;
            x += signX;
        }
        if (e2 < deltaX) {
            error += deltaX;
            y += signY;
        }
    }
}

// Add additional highlight details to make slash more dynamic
void RayCaster::addSlashHighlights(const std::vector<std::pair<float, float>>& slashPath, float width, const std::vector<sf::Color>& colors)
{
    int screenWidth = frameBuffer.getSize().x;
    int screenHeight = frameBuffer.getSize().y;
    
    // Number of highlight points to add
    const int numHighlights = 5;
    
    // Add some streaking light particles for dramatic effect
    for (int h = 0; h < numHighlights; h++) {
        // Pick random position along slash path
        int pathIdx = rand() % (slashPath.size() - 1);
        float centerX = slashPath[pathIdx].first;
        float centerY = slashPath[pathIdx].second;
        
        // Create highlight glow
        float glowSize = width * (0.3f + 0.7f * static_cast<float>(rand()) / RAND_MAX);
        sf::Color glowColor = colors[0]; // Brightest color
        
        // Add some variation to glow color
        float colorVar = 0.9f + 0.2f * static_cast<float>(rand()) / RAND_MAX;
        glowColor.r = static_cast<std::uint8_t>(std::min(255, static_cast<int>(glowColor.r * colorVar)));
        glowColor.g = static_cast<std::uint8_t>(std::min(255, static_cast<int>(glowColor.g * colorVar)));
        glowColor.b = static_cast<std::uint8_t>(std::min(255, static_cast<int>(glowColor.b * colorVar)));
        
        // Draw the glow
        for (int x = static_cast<int>(centerX - glowSize); x <= static_cast<int>(centerX + glowSize); x++) {
            for (int y = static_cast<int>(centerY - glowSize); y <= static_cast<int>(centerY + glowSize); y++) {
                if (x >= 0 && x < screenWidth && y >= 0 && y < screenHeight) {
                    float dx = x - centerX;
                    float dy = y - centerY;
                    float distSq = dx*dx + dy*dy;
                    
                    if (distSq <= glowSize * glowSize) {
                        float fade = 1.0f - std::sqrt(distSq) / glowSize;
                        fade = std::pow(fade, 0.5f) * 0.9f;
                        
                        sf::Color currentColor = frameBuffer.getPixel({static_cast<unsigned int>(x), static_cast<unsigned int>(y)});
                        
                        sf::Color finalColor(
                            static_cast<std::uint8_t>(currentColor.r * (1.0f - fade) + glowColor.r * fade),
                            static_cast<std::uint8_t>(currentColor.g * (1.0f - fade) + glowColor.g * fade),
                            static_cast<std::uint8_t>(currentColor.b * (1.0f - fade) + glowColor.b * fade)
                        );
                        
                        frameBuffer.setPixel({static_cast<unsigned int>(x), static_cast<unsigned int>(y)}, finalColor);
                    }
                }
            }
        }
    }
}

void RayCaster::draw(sf::RenderWindow& window)
{
    window.draw(frameSprite);
}
