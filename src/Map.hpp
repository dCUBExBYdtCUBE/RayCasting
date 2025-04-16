// Map.hpp
#pragma once
#include <vector>
#include <string>

// Define a Target structure
struct Target {
    int x;
    int y;
    int points;  // Points awarded for hitting this target
    bool hit;    // Whether the target has been hit
};

class Map {
private:
    int width;
    int height;
    std::vector<std::vector<int>> grid;
    std::vector<Target> targets;  // Collection of targets
    // In Map.hpp, define constants for clarity
// In Map.hpp, define constants for clarity
    static const int EMPTY = 0;
    static const int STANDARD_WALL = 1;
    static const int ENERGY_WALL = 2;
    static const int DATA_STREAM = 3;
    static const int NEON_BARRIER = 4;
    static const int HOLOGRAM = 5;

public:
    Map(int width = 20, int height = 20);
    
    void loadFromFile(const std::string& filename);
    void saveToFile(const std::string& filename) const;
    
    int getValueAt(int x, int y) const;
    bool isWall(int x, int y) const;
    int getWidth() const;
    int getHeight() const;
    
    // Target-related methods
    void addTarget(int x, int y, int points = 10);
    void removeTarget(int x, int y);
    const std::vector<Target>& getTargets() const;
    bool hitTarget(int x, int y);  // Returns true if successfully hit a target
    int getTargetPoints(int x, int y) const;  // Get points value of a target
    void resetTargets();  // Reset all targets to unhit state
    bool isTarget(int x, int y) const;  // Check if location has a target
    bool isHitTarget(int x, int y) const;  // Check if target has been hit
};
