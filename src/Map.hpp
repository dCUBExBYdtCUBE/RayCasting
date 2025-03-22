// Map.hpp
#pragma once
#include <vector>
#include <string>

class Map
{
private:
    std::vector<std::vector<int>> grid;
    int width;
    int height;

public:
    Map(int width = 10, int height = 10);
    void loadFromFile(const std::string& filename);
    int getValueAt(int x, int y) const;
    bool isWall(int x, int y) const;
    int getWidth() const;
    int getHeight() const;
};

