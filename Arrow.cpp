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
// Player 1: i=up, k=down, j=left, l=right
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

// SFML keyboard mapping. Matches the README's controls table exactly:
//   P1: W=Up, S=Down, A=Left, D=Right
//   P2: I=Up, K=Down, J=Left, L=Right
bool keyToDir(sf::Keyboard::Key key, Direction& out, int player) {
    if (player == 0) {
        switch (key) {
            case sf::Keyboard::W: out = Direction::UP;    return true;
            case sf::Keyboard::S: out = Direction::DOWN;  return true;
            case sf::Keyboard::A: out = Direction::LEFT;  return true;
            case sf::Keyboard::D: out = Direction::RIGHT; return true;
            default: return false;
        }
    } else {
        switch (key) {
            case sf::Keyboard::I: out = Direction::UP;    return true;
            case sf::Keyboard::K: out = Direction::DOWN;  return true;
            case sf::Keyboard::J: out = Direction::LEFT;  return true;
            case sf::Keyboard::L: out = Direction::RIGHT; return true;
            default: return false;
        }
    }
}
