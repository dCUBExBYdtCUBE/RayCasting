#pragma once

#include <SFML/Graphics.hpp>
#include <string>
#include <unordered_map>
#include <memory>
#include "Game.hpp"

// The TextRenderer class manages text rendering for the game
class TextRenderer {
public:
    TextRenderer();
    ~TextRenderer() = default;

    // Initialize with default font
    bool initialize(); // Single Responsibility Principle (SRP): Initializes the text renderer
    
    // Add a font to the collection
    bool addFont(const std::string& id, const std::string& filePath); // Encapsulation: Manages font resources
    
    // Create a text element with given attributes
    // Factory Method: Abstracts the creation of text elements
    void createText(const std::string& id, const std::string& content, 
                   const std::string& fontId, unsigned int fontSize, 
                   const sf::Color& color, const sf::Vector2f& position);
    
    // Update the content of an existing text element
    void updateText(const std::string& id, const std::string& newContent); // SRP: Updates text content
    
    // Update the position of an existing text element
    void setPosition(const std::string& id, const sf::Vector2f& position); // SRP: Updates text position
    
    // Draw all active text elements
    void draw(sf::RenderWindow& window); // SRP: Handles rendering of text elements

private:
    std::unordered_map<std::string, std::unique_ptr<sf::Font>> fonts; // Encapsulation: Manages font resources
    std::unordered_map<std::string, std::unique_ptr<sf::Text>> texts; // Encapsulation: Manages text elements

    struct TextObject {
        std::unique_ptr<sf::Text> text;
        sf::Color originalColor;  // Encapsulation: Stores the original color for reverting after inversion
    };
    std::string defaultFontId; // Encapsulation: Tracks the default font
    bool invertColors = false; // Encapsulation: Tracks whether colors are inverted
};