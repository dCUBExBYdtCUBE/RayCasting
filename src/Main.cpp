// Main.cpp
#include <SFML/Graphics.hpp>
#include "Game.hpp"

int main()
{
    Game game(800, 600, "Raycasting Game");
    game.run();
    return 0;
}


    void handleInput();
    void update(float deltaTime);
    void render();
};

