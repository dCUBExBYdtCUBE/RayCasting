// Main.cpp
#include <SFML/Graphics.hpp>
#include "Game.hpp"

int main()
{
    Game game(800, 600, "Raycasting Game");
    game.run();
    return 0;
}

// Game.hpp
#pragma once
#include <SFML/Graphics.hpp>
#include "Player.hpp"
#include "Map.hpp"
#include "RayCaster.hpp"

class Game
{
private:
    sf::RenderWindow window;
    Player player;
    Map map;
    RayCaster raycaster;
    sf::Clock clock;
    bool isRunning;

public:
    Game(int width, int height, const std::string& title);
    void run();
    void handleInput();
    void update(float deltaTime);
    void render();
};

