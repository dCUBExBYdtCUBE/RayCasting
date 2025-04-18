// SwordRenderer.cpp
#include "SwordRenderer.hpp"

void SwordRenderer::draw(sf::Image& frameBuffer, const Player& player)
{
    int screenWidth = frameBuffer.getSize().x;
    int screenHeight = frameBuffer.getSize().y;

    int swordWidth = screenWidth * 0.3f;
    int swordHeight = screenHeight * 0.2f;
    int swordPosX = screenWidth - swordWidth - 10;
    int swordPosY = screenHeight - swordHeight - 10;

    int bladeHeight = swordHeight * 0.7f;
    int hiltHeight = swordHeight * 0.2f;
    int pommelHeight = swordHeight * 0.1f;

    int edgeThickness = 3; // thickness of the neon edge
    sf::Color neonCyan(0, 255, 255);

    auto drawHollowRect = [&](int x, int y, int w, int h) {
        // Top and Bottom edges
        for (int i = 0; i < edgeThickness; ++i) {
            for (int dx = 0; dx < w; ++dx) {
                // Top edge
                int pxTop = x + dx;
                int pyTop = y + i;
                if (pxTop >= 0 && pxTop < screenWidth && pyTop >= 0 && pyTop < screenHeight)
                    frameBuffer.setPixel({static_cast<unsigned>(pxTop), static_cast<unsigned>(pyTop)}, neonCyan);

                // Bottom edge
                int pxBot = x + dx;
                int pyBot = y + h - 1 - i;
                if (pxBot >= 0 && pxBot < screenWidth && pyBot >= 0 && pyBot < screenHeight)
                    frameBuffer.setPixel({static_cast<unsigned>(pxBot), static_cast<unsigned>(pyBot)}, neonCyan);
            }
        }

        // Left and Right edges
        for (int i = 0; i < edgeThickness; ++i) {
            for (int dy = 0; dy < h; ++dy) {
                // Left edge
                int pxLeft = x + i;
                int pyLeft = y + dy;
                if (pxLeft >= 0 && pxLeft < screenWidth && pyLeft >= 0 && pyLeft < screenHeight)
                    frameBuffer.setPixel({static_cast<unsigned>(pxLeft), static_cast<unsigned>(pyLeft)}, neonCyan);

                // Right edge
                int pxRight = x + w - 1 - i;
                int pyRight = y + dy;
                if (pxRight >= 0 && pxRight < screenWidth && pyRight >= 0 && pyRight < screenHeight)
                    frameBuffer.setPixel({static_cast<unsigned>(pxRight), static_cast<unsigned>(pyRight)}, neonCyan);
            }
        }
    };

    // Draw Blade (top part)
    drawHollowRect(swordPosX + swordWidth * 0.4f, swordPosY, swordWidth * 0.2f, bladeHeight);

    // Draw Hilt (middle part)
    drawHollowRect(swordPosX + swordWidth * 0.3f, swordPosY + bladeHeight, swordWidth * 0.4f, hiltHeight);

    // Draw Pommel (bottom part)
    drawHollowRect(swordPosX + swordWidth * 0.4f, swordPosY + bladeHeight + hiltHeight, swordWidth * 0.2f, pommelHeight);
}
