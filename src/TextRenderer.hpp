#pragma once

#include <SFML/Graphics.hpp>
#include <string>
#include <unordered_map>
#include <memory>

class TextRenderer {
public:
    TextRenderer();
    ~TextRenderer() = default;

    // Initialize with default font
    bool initialize();
    
    // Add a font to the collection
    bool addFont(const std::string& id, const std::string& filePath);
    
    // Create a text element with given attributes
    void createText(const std::string& id, const std::string& content, 
                   const std::string& fontId, unsigned int fontSize, 
                   const sf::Color& color, const sf::Vector2f& position);
    
    // Update the content of an existing text element
    void updateText(const std::string& id, const std::string& newContent);
    
    // Update the position of an existing text element
    void setPosition(const std::string& id, const sf::Vector2f& position);
    
    // Draw all active text elements
    void draw(sf::RenderWindow& window);

private:
    std::unordered_map<std::string, std::unique_ptr<sf::Font>> fonts;
    std::unordered_map<std::string, std::unique_ptr<sf::Text>> texts;
    
    // Default font id
    std::string defaultFontId;
};