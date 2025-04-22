#include "TextRenderer.hpp"
#include <iostream>
#include <fstream>

TextRenderer::TextRenderer() : defaultFontId("default") {
}

bool TextRenderer::initialize() {
    // Try to load a default font with absolute path
    return addFont(defaultFontId, "D:\\6th Sem\\OOAD\\RayCasting\\assets\\fonts\\Tron-JOAa.ttf");
    // return addFont(defaultFontId, "C:/Windows/Fonts/Arial.ttf");
}

bool TextRenderer::addFont(const std::string& id, const std::string& filePath) {
    auto font = std::make_unique<sf::Font>();

    std::ifstream fileTest(filePath, std::ios::binary);
    if (!fileTest) {
        std::cerr << "Cannot open font file: " << filePath << std::endl;
        return false;
    }
    fileTest.close();
    
    if (!font->openFromFile(filePath)) {
        std::cerr << "Failed to load font: " << filePath << std::endl;
        return false;
    }
    
    fonts[id] = std::move(font);
    return true;
}

void TextRenderer::createText(const std::string& id, const std::string& content, 
                             const std::string& fontId, unsigned int fontSize, 
                             const sf::Color& color, const sf::Vector2f& position) {
    // Check if the font exists, use default if not found
    auto fontIt = fonts.find(fontId);
    if (fontIt == fonts.end()) {
        fontIt = fonts.find(defaultFontId);
        if (fontIt == fonts.end()) {
            std::cerr << "No fonts available for TextRenderer" << std::endl;
            return;
        }
    }
    
    // For SFML 3.0.0, we need to create the text object differently
    // First create the text with a reference to the font
    auto text = new sf::Text(*fontIt->second);
    text->setString(content);
    text->setCharacterSize(fontSize);
    text->setFillColor(color);
    text->setPosition(position);
    
    // Store the text in our map
    texts[id] = std::unique_ptr<sf::Text>(text);
}

void TextRenderer::updateText(const std::string& id, const std::string& newContent) {
    auto it = texts.find(id);
    if (it != texts.end()) {
        it->second->setString(newContent);
    }
    else {
        std::cerr << "Text element not found: " << id << std::endl;
    }
}

void TextRenderer::setPosition(const std::string& id, const sf::Vector2f& position) {
    auto it = texts.find(id);
    if (it != texts.end()) {
        it->second->setPosition(position);
    }
    else {
        std::cerr << "Text element not found: " << id << std::endl;
    }
}

void TextRenderer::draw(sf::RenderWindow& window) {
    for (const auto& [id, text] : texts) {
        window.draw(*text);
    }
}