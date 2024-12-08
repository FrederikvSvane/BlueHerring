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

struct piece_t {
    PieceType type;
    Color color;

    piece_t() : type(PieceType::EMPTY), color(Color::NONE) {}
    piece_t(PieceType typeval) : type(typeval), color(Color::NONE) {}
    piece_t(PieceType typeval, Color colorval) : type(typeval), color(colorval) {}
};

#endif