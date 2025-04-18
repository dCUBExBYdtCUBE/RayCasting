#include <SFML/Graphics.hpp>
#include "Game.hpp"
#include <iostream>

int main() {
    try {
        // Create the game with window dimensions and title
        Game game(800, 600, "Raycasting Game");
        
        
        // Run the game
        game.run();
    }
    catch (const std::exception& e) {
        // Catch and log any exceptions that might occur
        std::cerr << "An error occurred: " << e.what() << std::endl;
        return -1;
    }
    catch (...) {
        // Catch any other unexpected errors
        std::cerr << "An unknown error occurred" << std::endl;
        return -1;
    }

    return 0;
}

// C:\SFML-3.0.0\include
// C:\SFML-3.0.0\lib
// g++ -o game Main.cpp Game.cpp Player.cpp Map.cpp RayCaster.cpp -I"C:\SFML-3.0.0\include" -L"C:\SFML-3.0.0\lib" -lsfml-graphics -lsfml-window -lsfml-system