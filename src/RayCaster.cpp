#include "RayCaster.hpp"
#include <cmath>
#include <cstdint>
RayCaster::RayCaster(int screenWidth, int screenHeight)
    : frameTexture(sf::Vector2u(static_cast<unsigned int>(screenWidth), 
                 static_cast<unsigned int>(screenHeight))),
      frameSprite(frameTexture)  // Initialize sprite with texture
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
        
        // Draw the pixels of the stripe as a vertical line
        for (int y = drawStart; y < drawEnd; y++)
        {
            frameBuffer.setPixel({static_cast<unsigned int>(x), static_cast<unsigned int>(y)}, color);
        }
    }
    
    // Update the texture with our pixel data
    frameTexture.update(frameBuffer);
}

void RayCaster::draw(sf::RenderWindow& window)
{
    window.draw(frameSprite);
}