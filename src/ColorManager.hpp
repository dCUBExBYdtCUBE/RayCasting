#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>

/*
* SOLID Principles:
* - Single Responsibility Principle (SRP): Manages only color-related operations
* - Open/Closed Principle (OCP): New color schemes can be added without modification
* - Interface Segregation Principle (ISP): Provides focused color management interfaces
*
* GRASP Patterns:
* - Information Expert: Manages all color-related data and operations
* - Creator: Creates and manages color palettes
* - Pure Fabrication: Provides specialized color management service
* - Protected Variations: Encapsulates color implementation details
*/

// Add this to a header file, perhaps a new ColorManager.hpp
class ColorManager {
    private:
        struct ColorPalette {
            // Wall colors
            sf::Color wallColors[10]; // Assuming you have up to 10 wall types
            
            // UI colors
            sf::Color titleText;
            sf::Color scoreText;
            sf::Color controlsText;
            sf::Color floorColor;
            sf::Color ceilingColor;
            sf::Color targetActiveColor;
            sf::Color targetHitColor;
            sf::Color swordColor;
            // Add other colors as needed
        };
        
        ColorPalette normalPalette; // Encapsulation: Hides palette details
        ColorPalette negativePalette; // Encapsulation: Hides palette details
        bool useNegativePalette = false; // Strategy Pattern: Determines which palette to use
        
    public:
        ColorManager() {
            initPalettes(); // Factory Method: Initializes palettes
        }
        
        void initPalettes() {
            // Factory Method: Abstracts the creation of palettes
            normalPalette.wallColors[0] = sf::Color(10, 10, 30);    // Dark blue-black for floor (usually not used)
            normalPalette.wallColors[1] = sf::Color(0, 210, 255);   // Bright cyan for standard walls
            normalPalette.wallColors[2] = sf::Color(255, 0, 150);   // Neon pink for energy walls
            normalPalette.wallColors[3] = sf::Color(0, 255, 120);   // Electric green for data streams   
            normalPalette.wallColors[4] = sf::Color(255, 230, 0);   // Bright yellow (if you add another wall type)
            normalPalette.wallColors[9] = sf::Color(255, 255, 255);  
            
            normalPalette.titleText = sf::Color(0, 255, 255);     // Cyan
            normalPalette.scoreText = sf::Color(170, 230, 255);   // Light blue
            normalPalette.controlsText = sf::Color(255, 150, 0);  // Orange
            normalPalette.floorColor = sf::Color(10, 15, 30);     // Dark blue
            normalPalette.ceilingColor = sf::Color(5, 10, 25);    // Very dark blue
            normalPalette.targetActiveColor = sf::Color(255, 100, 0); // Orange
            normalPalette.targetHitColor = sf::Color(100, 100, 100);  // Gray
            normalPalette.swordColor = sf::Color(0, 220, 255);      // Light blue-white
            
            // Generate negative palette by inverting normal colors
            for (int i = 0; i < 10; i++) {
                negativePalette.wallColors[i] = invertColor(normalPalette.wallColors[i]);
            }
            
            negativePalette.titleText = invertColor(normalPalette.titleText);
            negativePalette.scoreText = invertColor(normalPalette.scoreText);
            negativePalette.controlsText = invertColor(normalPalette.controlsText);
            negativePalette.floorColor = invertColor(normalPalette.floorColor);
            negativePalette.ceilingColor = invertColor(normalPalette.ceilingColor);
            negativePalette.targetActiveColor = invertColor(normalPalette.targetActiveColor);
            negativePalette.targetHitColor = invertColor(normalPalette.targetHitColor);
            negativePalette.swordColor = invertColor(normalPalette.swordColor);
        }
        
        sf::Color invertColor(const sf::Color& color) const {
            // DRY(Don't Repeat Yourself) Principle: Reuses logic for inverting colors
            return sf::Color(255 - color.r, 255 - color.g, 255 - color.b, color.a);
        }
        
        void setNegativeDimension(bool negative) {
            // Strategy Pattern: Switches between normal and negative palettes
            useNegativePalette = negative;
            std::cout << "Using negative palette: " << (negative ? "true" : "false") << std::endl;
        }
        
        sf::Color getWallColor(int wallType) const {
            // Encapsulation: Hides how wall colors are stored
            if (wallType >= 0 && wallType < 10) {
                return useNegativePalette ? 
                    negativePalette.wallColors[wallType] : 
                    normalPalette.wallColors[wallType];
            }
            return sf::Color::Magenta; // Default for invalid types
        }
        
        sf::Color getTitleTextColor() const {
            return useNegativePalette ? negativePalette.titleText : normalPalette.titleText;
        }
        
        sf::Color getScoreTextColor() const {
            return useNegativePalette ? negativePalette.scoreText : normalPalette.scoreText;
        }
        
        sf::Color getControlsTextColor() const {
            return useNegativePalette ? negativePalette.controlsText : normalPalette.controlsText;
        }
        
        sf::Color getFloorColor() const {
            return useNegativePalette ? negativePalette.floorColor : normalPalette.floorColor;
        }
        
        sf::Color getCeilingColor() const {
            return useNegativePalette ? negativePalette.ceilingColor : normalPalette.ceilingColor;
        }
        
        sf::Color getTargetActiveColor() const {
            return useNegativePalette ? negativePalette.targetActiveColor : normalPalette.targetActiveColor;
        }
        
        sf::Color getTargetHitColor() const {
            return useNegativePalette ? negativePalette.targetHitColor : normalPalette.targetHitColor;
        }
        
        sf::Color getSwordColor() const {
            return useNegativePalette ? negativePalette.swordColor : normalPalette.swordColor;
        }
        
        int getNumberOfWallColors() const {
            // Open/Closed Principle: Allows extension without modifying existing logic
            return sizeof(normalPalette.wallColors) / sizeof(normalPalette.wallColors[0]);
        }
    };
