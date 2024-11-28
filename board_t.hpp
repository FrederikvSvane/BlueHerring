#ifndef board_t_hpp
#define board_t_hpp
#include "piece_t.hpp"
#include <array>
#include <iostream>

using namespace std;

struct piece_t {
    bool color;
    piece_t(bool color) : color(color) {}
};

struct pawn_t : piece_t {
    pawn_t(bool color) : piece_t(color) {}
};

struct rook_t : piece_t {
    rook_t(bool color) : piece_t(color) {}
};

struct knight_t : piece_t {
    knight_t(bool color) : piece_t(color) {}
};

struct bishop_t : piece_t {
    bishop_t(bool color) : piece_t(color) {}
};

struct queen_t : piece_t {
    queen_t(bool color) : piece_t(color) {}
};

struct king_t : piece_t {
    king_t(bool color) : piece_t(color) {}
};

struct square_t {
    int x;
    int y;
    piece_t* piece;

    // we need this default constructor for the declaration of the board_t type.
    // it is overwritten with the constructor of board_t. Dont worry.
    square_t() : x(0), y(0) {}

    square_t(int xval, int yval) : x(xval), y(yval) {}

    // I think this should be done in a function instead of here.
    // Like a function called initialize_board()

    // if (y == 0 || y == 1) {
    //     // white piece
    //     if (y == 1) {
    //         // white pawn
    //         this->piece = new pawn_t(1);
    //         return;
    //     } else if (x == 0 || x == 7) {
    //         // white rook
    //         this->piece = new rook_t(1);
    //         return;
    //     } else if (x == 1 || x == 6) {
    //         // white knight
    //         this->piece = new knight_t(1);
    //         return;
    //     } else if (x == 2 || x == 5) {
    //         // white bishop
    //         this->piece = new bishop_t(1);
    //         return;
    //     } else if (x == 3) {
    //         // white queen
    //         this->piece = new queen_t(1);
    //         return;
    //     } else {
    //         // white king
    //         this->piece = new king_t(1);
    //         return;
    //     }
    // } else if (y == 6 || y == 7) {
    //     // black piece
    //     if (y == 6) {
    //         // black pawn
    //         this->piece = new pawn_t(0);
    //         return;
    //     } else if (x == 0 || x == 7) {
    //         // black rook
    //         this->piece = new rook_t(0);
    //         return;
    //     } else if (x == 1 || x == 6) {
    //         // black knight
    //         this->piece = new knight_t(0);
    //         return;
    //     } else if (x == 2 || x == 5) {
    //         // black bishop
    //         this->piece = new bishop_t(0);
    //         return;
    //     } else if (x == 3) {
    //         // black queen
    //         this->piece = new queen_t(0);
    //         return;
    //     } else {
    //         // black king
    //         this->piece = new king_t(0);
    //         return;
    //     }
    // } else {
    //     this->piece = nullptr;
    // }
};

struct board_t {
    array<array<square_t, 8>, 8> board;

    board_t() {
        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 8; j++) {
                board[i][j] = square_t(i, j);
            }
        }
    }
};

#endif