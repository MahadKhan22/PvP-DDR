#include "GameState.h"
#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>

// CLI TEST HARNESS
// -----------------
// Since two players can't truly type simultaneously in a terminal,
// we alternate: each "round" asks P1 for one char then P2 for one char.
// This is enough to verify all game logic before moving to SFML.
//
// Controls:
//   Player 1: w=up  s=down  a=left  d=right
//   Player 2: i=up  k=down  j=left  l=right
//   Both:     q = quit

int main() {
    srand(static_cast<unsigned>(time(nullptr)));

    GameState game;

    std::cout << "=== PvP Arrow Game - CLI Test ===\n";
    std::cout << "P1: W/A/S/D   P2: I/J/K/L   Q = quit\n\n";

    while (!game.isOver()) {
        game.render();

        // --- Player 1 input ---
        std::cout << "P1 input: ";
        char c1;
        std::cin >> c1;
        if (c1 == 'q') break;

        Direction d1;
        if (charToDir(c1, d1, 0)) {
            game.feedInput(0, d1);
        } else {
            std::cout << "[P1] Invalid key.\n";
        }

        // Tick once after P1
        if (game.tick()) break;

        // --- Player 2 input ---
        std::cout << "P2 input: ";
        char c2;
        std::cin >> c2;
        if (c2 == 'q') break;

        Direction d2;
        if (charToDir(c2, d2, 1)) {
            game.feedInput(1, d2);
        } else {
            std::cout << "[P2] Invalid key.\n";
        }

        // Tick once after P2
        if (game.tick()) break;
    }

    std::cout << "\n=== GAME OVER ===\n";
    if (game.winner() == 0)      std::cout << "Player 1 wins!\n";
    else if (game.winner() == 1) std::cout << "Player 2 wins!\n";
    else                         std::cout << "Draw!\n";

    return 0;
}
