// Map.cpp
#include "Map.hpp"
#include <fstream>
#include <iostream>

Map::Map(int width, int height)
    : width(width), height(height)
{
    // Initialize with a simple maze-like structure
    grid.resize(height, std::vector<int>(width, 0));
    
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
}

void Map::loadFromFile(const std::string& filename)
{
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
    
    file.close();
}

int Map::getValueAt(int x, int y) const
{
    if (x >= 0 && x < width && y >= 0 && y < height)
    {
        return grid[y][x];
    }
    return -1;  // Out of bounds
}

bool Map::isWall(int x, int y) const
{
    int value = getValueAt(x, y);
    return value > 0;  // Anything greater than 0 is a wall
}

int Map::getWidth() const
{
    return width;
}

int Map::getHeight() const
{
    return height;
}