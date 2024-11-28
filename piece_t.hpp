#ifndef piece_t_hpp
#define piece_t_hpp
#include <iostream>
#include <vector>

using namespace std;

struct piece_t {
    bool color; // 0 stands for black, 1 stands for white

    piece_t(bool colorval) : color(colorval) {}

    virtual vector<array<int, 2>> get_moves(int x, int y) {
        return vector<array<int, 2>>();
    }
};

struct pawn_t : piece_t {
    using piece_t::piece_t; // inherit constructor

    vector<array<int, 2>> get_moves(int x, int y) override {
        vector<array<int, 2>> moves;
        int direction = this->color ? 1 : -1; // if the piece is white it moves up, otherwise down

        if (y == 1 && direction == 1) { // first move for white
            moves.push_back({x, 2});
            moves.push_back({x, 3});
            return moves;
        } else if (y == 6 && direction == -1) { // first move for black
            moves.push_back({x, 6});
            moves.push_back({x, 5});
            return moves;
        } else {
            moves.push_back({x, y + direction});
            return moves;
        }
    }
};

struct rook_t : piece_t {
    using piece_t::piece_t;

    vector<array<int, 2>> get_moves(int x, int y) override {
        vector<array<int, 2>> moves;

        for (int xi = 0; xi < 8; xi++) { // move horizontally
            if (xi != x) {
                moves.push_back({xi, y});
            }
        }
        for (int yi = 0; yi < 8; yi++) { // move vertically
            if (yi != y) {
                moves.push_back({x, yi});
            }
        }
        return moves;
    }
};

struct bishop_t : piece_t {
    using piece_t::piece_t;

    vector<array<int, 2>> get_moves(int x, int y) override {
        vector<array<int, 2>> moves;
        // FILL IN
        return moves;
    }
};

struct knight_t : piece_t {
    using piece_t::piece_t;
};

struct queen_t : piece_t {
    using piece_t::piece_t;
};

struct king_t : piece_t {
    using piece_t::piece_t;
};

#endif