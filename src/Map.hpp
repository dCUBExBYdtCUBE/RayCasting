// Map.hpp
#pragma once
#include <vector>
#include <string>
#include <SFML/Graphics.hpp>

// Define a Target structure
struct Target {
    int x;
    int y;
    int points;  // Points awarded for hitting this target
    bool hit;    // Whether the target has been hit
};

class Map {
private:
    int width;  // Encapsulation: Hides the map's dimensions
    int height; // Encapsulation: Hides the map's dimensions
    std::vector<std::vector<int>> grid; // Encapsulation: Stores the map's grid structure
    std::vector<Target> targets;  // Encapsulation: Manages targets on the map

    // Constants for map elements
    static const int EMPTY = 0;           // Open/Closed Principle: Easy to extend with new types
    static const int STANDARD_WALL = 1;
    static const int ENERGY_WALL = 2;
    static const int DATA_STREAM = 3;
    static const int NEON_BARRIER = 4;
    static const int HOLOGRAM = 5;
    static const int PORTAL = 9;

    std::vector<sf::Vector2f> portalLocations; // Encapsulation: Stores portal locations
    sf::Vector2f portalExitNormal;            // Encapsulation: Exit for normal dimension
    sf::Vector2f portalExitNegative;          // Encapsulation: Exit for negative dimension

public:
    // Constructor initializes the map with default dimensions
    Map(int width = 20, int height = 20); // Single Responsibility Principle: Initializes the map
    
    // File operations for saving and loading maps
    void loadFromFile(const std::string& filename); // SRP: Handles map loading
    void saveToFile(const std::string& filename) const; // SRP: Handles map saving
    
    // Accessors for map grid values
    int getValueAt(int x, int y) const; // Encapsulation: Provides access to grid values
    bool isWall(int x, int y) const;    // Encapsulation: Checks if a cell is a wall
    int getWidth() const;               // Encapsulation: Provides map width
    int getHeight() const;              // Encapsulation: Provides map height
    
    // Target-related methods
    void addTarget(int x, int y, int points = 10); // Open/Closed Principle: Easy to add new targets
    void removeTarget(int x, int y);              // Encapsulation: Removes a target
    const std::vector<Target>& getTargets() const; // Encapsulation: Provides access to targets
    bool hitTarget(int x, int y);                 // Dependency Inversion Principle: Abstracts target hit logic
    int getTargetPoints(int x, int y) const;      // Encapsulation: Gets points for a target
    void resetTargets();                          // SRP: Resets all targets
    bool isTarget(int x, int y) const;            // Encapsulation: Checks if a cell has a target
    bool isHitTarget(int x, int y) const;         // Encapsulation: Checks if a target is hit

    // Portal-related methods
    bool isPortal(int x, int y) const; // Encapsulation: Checks if a cell is a portal
    void addPortal(int x, int y, sf::Vector2f normalExit, sf::Vector2f negativeExit); // Factory Method: Adds a portal
    sf::Vector2f getPortalExit(bool toNegativeDimension) const; // Dependency Inversion Principle: Abstracts portal exit logic
    
    // For map initialization
    void setupPortals(); // Factory Method: Initializes portals
};

