// Map.cpp
#include "Map.hpp"
#include <fstream>
#include <iostream>

Map::Map(int width, int height)
    : width(width), height(height) {
    // Initialize with a simple maze-like structure
    grid.resize(height, std::vector<int>(width, 0));
    targets.clear(); // Initialize empty targets vector
    
    // Create walls around the map edges
    for (int x = 0; x < width; x++)
    {
        grid[0][x] = 1;
        grid[height - 1][x] = 1;
    }
    
    for (int y = 0; y < height; y++)
    {
        grid[y][0] = 1;
        grid[y][width - 1] = 1;
    }
    
    // Add some walls in the middle
    for (int x = 7; x < 12; x++)
    {
        grid[7][x] = 1;
    }
    
    for (int y = 12; y < 16; y++)
    {
        grid[y][12] = 1;
    }
    
    // Add a pillar
    grid[5][5] = 1;
    
    // Add some different wall types (represented by different integers)
    grid[10][5] = 2;
    grid[11][5] = 2;
    grid[12][5] = 2;
    
    grid[5][10] = 3;
    grid[5][11] = 3;
    grid[5][12] = 3;

    // Add some default targets to the practice range
    addTarget(8, 3, 10);  // x, y, points
    addTarget(15, 8, 20);
    addTarget(10, 15, 30);
}

void Map::loadFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open())
    {
        std::cerr << "Error: Could not open map file " << filename << std::endl;
        return;
    }
    
    file >> height >> width;
    grid.resize(height, std::vector<int>(width, 0));
    
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            file >> grid[y][x];
        }
    }
    
    // Load targets if they exist in the file
    int numTargets;
    if (file >> numTargets) {
        targets.clear();
        for (int i = 0; i < numTargets; i++) {
            int x, y, points;
            if (file >> x >> y >> points) {
                addTarget(x, y, points);
            }
        }
    }
    
    file.close();
}

void Map::saveToFile(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open())
    {
        std::cerr << "Error: Could not open map file for writing " << filename << std::endl;
        return;
    }
    
    file << height << " " << width << std::endl;
    
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            file << grid[y][x] << " ";
        }
        file << std::endl;
    }
    
    // Save targets
    file << targets.size() << std::endl;
    for (const auto& target : targets) {
        file << target.x << " " << target.y << " " << target.points << std::endl;
    }
    
    file.close();
}

int Map::getValueAt(int x, int y) const {
    if (x >= 0 && x < width && y >= 0 && y < height)
    {
        return grid[y][x];
    }
    return -1;  // Out of bounds
}

bool Map::isWall(int x, int y) const {
    int value = getValueAt(x, y);
    return value > 0;  // Anything greater than 0 is a wall
}

int Map::getWidth() const {
    return width;
}

int Map::getHeight() const {
    return height;
}

// Target-related methods
void Map::addTarget(int x, int y, int points) {
    // Only add target if position is valid (not a wall and within bounds)
    if (x >= 0 && x < width && y >= 0 && y < height && !isWall(x, y)) {
        Target newTarget{x, y, points, false};
        targets.push_back(newTarget);
    }
}

void Map::removeTarget(int x, int y) {
    for (auto it = targets.begin(); it != targets.end(); ++it) {
        if (it->x == x && it->y == y) {
            targets.erase(it);
            return;
        }
    }
}

const std::vector<Target>& Map::getTargets() const {
    return targets;
}

bool Map::hitTarget(int x, int y) {
    for (auto& target : targets) {
        if (target.x == x && target.y == y && !target.hit) {
            target.hit = true;
            return true;
        }
    }
    return false;
}

int Map::getTargetPoints(int x, int y) const {
    for (const auto& target : targets) {
        if (target.x == x && target.y == y) {
            return target.points;
        }
    }
    return 0;
}

void Map::resetTargets() {
    for (auto& target : targets) {
        target.hit = false;
    }
}

bool Map::isTarget(int x, int y) const {
    for (const auto& target : targets) {
        if (target.x == x && target.y == y) {
            return true;
        }
    }
    return false;
}

bool Map::isHitTarget(int x, int y) const {
    for (const auto& target : targets) {
        if (target.x == x && target.y == y) {
            return target.hit;
        }
    }
    return false;
}
