#include "Arrow.h"

char dirToChar(Direction d) {
    switch (d) {
        case Direction::UP:    return '^';
        case Direction::DOWN:  return 'v';
        case Direction::LEFT:  return '<';
        case Direction::RIGHT: return '>';
    }
    return '?';
}

std::string dirToStr(Direction d) {
    switch (d) {
        case Direction::UP:    return "[^]";
        case Direction::DOWN:  return "[v]";
        case Direction::LEFT:  return "[<]";
        case Direction::RIGHT: return "[>]";
    }
    return "[?]";
}

// Player 0: W=up, S=down, A=left, D=right
// Player 1 (CLI substitute): i=up, k=down, j=left, l=right
//   (In SFML you'll swap this to sf::Keyboard arrow keys)
bool charToDir(char c, Direction& out, int player) {
    if (player == 0) {
        switch (c) {
            case 'w': out = Direction::UP;    return true;
            case 's': out = Direction::DOWN;  return true;
            case 'a': out = Direction::LEFT;  return true;
            case 'd': out = Direction::RIGHT; return true;
        }
    } else {
        switch (c) {
            case 'i': out = Direction::UP;    return true;
            case 'k': out = Direction::DOWN;  return true;
            case 'j': out = Direction::LEFT;  return true;
            case 'l': out = Direction::RIGHT; return true;
        }
    }
    return false;
}
