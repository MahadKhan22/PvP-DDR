#pragma once
#include <vector>
#include <string>

// The four possible directions a player can input
enum class Direction { UP, DOWN, LEFT, RIGHT };

// Direction to arrows eg: "[^]" for UP, "[?]" if no direction
char dirToChar(Direction d);
std::string dirToStr(Direction d);

// Converts a raw char input to a Direction.

// changes out to the Direction
// returns true if done, false if char doesnt match anything
bool charToDir(char c, Direction& out, int player);
