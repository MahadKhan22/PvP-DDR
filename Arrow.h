#pragma once
#include <vector>
#include <string>
#include <SFML/Window/Keyboard.hpp>

// The four possible directions a player can input
enum class Direction { UP, DOWN, LEFT, RIGHT };

// Direction to arrows eg: "[^]" for UP, "[?]" if no direction
char dirToChar(Direction d);
std::string dirToStr(Direction d);

// CLI version (kept for backward compatibility / unit tests).
// Player 0: W=up, S=down, A=left, D=right
// Player 1: i=up, k=down, j=left, l=right
// changes out to the Direction
// returns true if done, false if char doesnt match anything
bool charToDir(char c, Direction& out, int player);

// SFML version. Same key bindings as the README controls table:
// Player 0 (P1): W / A / S / D
// Player 1 (P2): I / J / K / L
// Returns true if the key matched a direction for the given player.
bool keyToDir(sf::Keyboard::Key key, Direction& out, int player);
