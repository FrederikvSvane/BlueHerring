#ifndef piece_t_hpp
#define piece_t_hpp
#include "move_t.hpp"
#include <array>
#include <iostream>
#include <vector>

using namespace std;

enum class PieceType {
    EMPTY,
    PAWN,
    KNIGHT,
    BISHOP,
    ROOK,
    QUEEN,
    KING
};

enum class Color {
    NONE,
    WHITE,
    BLACK
};

// Overload the ! operator for Color
Color operator!(Color color) {
    switch (color) {
        case Color::WHITE:
            return Color::BLACK;
        case Color::BLACK:
            return Color::WHITE;
        case Color::NONE:
            throw std::invalid_argument("Cannot negate Color::NONE");
    }
    throw std::invalid_argument("Invalid color value"); // Fallback for unexpected cases
}



struct piece_t {
    PieceType type;
    Color color;

    piece_t() : type(PieceType::EMPTY), color(Color::NONE) {}
    piece_t(PieceType typeval) : type(typeval), color(Color::NONE) {}
    piece_t(PieceType typeval, Color colorval) : type(typeval), color(colorval) {}
};

#endif