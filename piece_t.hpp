#ifndef piece_t_hpp
#define piece_t_hpp
#include <iostream>
#include <vector>
#include <array>

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

    piece_t() : type(PieceType::EMPTY), color(Color::NONE) {} // default constructor
    piece_t(PieceType typeval) : type(typeval), color(Color::NONE) {}
    piece_t(Color colorval) : type(PieceType::EMPTY), color(colorval) {}

    virtual vector<array<int, 2>> get_moves(int x, int y) {
        return vector<array<int, 2>>();
    }
};

struct pawn_t : piece_t {
    pawn_t(Color colorval) : piece_t(colorval) {
        type = PieceType::PAWN;
    }

    vector<array<int, 2>> get_moves(int x, int y) override {
        vector<array<int, 2>> moves;
        int direction = (color == Color::WHITE) ? 1 : -1;

        if (y == 1 && color == Color::WHITE) {
            moves.push_back({x, 2});
            moves.push_back({x, 3});
        } else if (y == 6 && color == Color::BLACK) {
            moves.push_back({x, 4});
            moves.push_back({x, 5});
        } else {
            moves.push_back({x, y + direction});
        }
        return moves;
    }
};

struct rook_t : piece_t {
    rook_t(Color colorval) : piece_t(colorval) {
        type = PieceType::ROOK;
    }

    vector<array<int, 2>> get_moves(int x, int y) override {
        vector<array<int, 2>> moves;
        for (int xi = 0; xi < 8; xi++) {
            if (xi != x)
                moves.push_back({xi, y});
        }
        for (int yi = 0; yi < 8; yi++) {
            if (yi != y)
                moves.push_back({x, yi});
        }
        return moves;
    }
};

struct knight_t : piece_t {
    knight_t(Color colorval) : piece_t(colorval) {
        type = PieceType::KNIGHT;
    }

    vector<array<int, 2>> get_moves(int x, int y) override {
        vector<array<int, 2>> moves;
        vector<std::array<int, 2>> offsets = {
        {x + 1, y + 2}, {x - 1, y + 2}, {x + 1, y - 2}, {x - 1, y - 2},
        {x + 2, y + 1}, {x - 2, y + 1}, {x + 2, y - 1}, {x - 2, y - 1}
        };
        for (const auto& [xi, yi] : offsets) {
            if(0 <= xi <= 7 && 0 <= yi <= 7){
                moves.push_back({xi, yi});
            };
        }
        return moves;
    }
};

struct bishop_t : piece_t {
    bishop_t(Color colorval) : piece_t(colorval) {
        type = PieceType::BISHOP;
    }

    vector<array<int, 2>> get_moves(int x, int y) override {
        vector<array<int, 2>> moves;
        return moves;
    }
};

struct queen_t : piece_t {
    queen_t(Color colorval) : piece_t(colorval) {
        type = PieceType::QUEEN;
    }

    vector<array<int, 2>> get_moves(int x, int y) override {
        vector<array<int, 2>> moves;
        return moves;
    }
};

struct king_t : piece_t {
    king_t(Color colorval) : piece_t(colorval) {
        type = PieceType::KING;
    }

    vector<array<int, 2>> get_moves(int x, int y) override {
        vector<array<int, 2>> moves;
        vector<std::array<int, 2>> offsets = {
            {x + 1, y + 1}, {x + 1, y}, {x + 1, y - 1}, {x, y - 1}, 
            {x - 1, y - 1}, {x - 1, y}, {x - 1, y + 1}, {x, y + 1}
        };
        for (const auto& [xi, yi] : offsets) {
            if(0 <= xi <= 7 && 0 <= yi <= 7){
                moves.push_back({xi, yi});
            };
        }
        return moves;
    }
};

#endif