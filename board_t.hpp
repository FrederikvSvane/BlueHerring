#ifndef board_t_hpp
#define board_t_hpp
#include "piece_t.hpp"
#include <array>
#include <iostream>

using namespace std;

struct square_t {
    int x;
    int y;
    piece_t piece;

    // we need this default constructor for the declaration of the board_t type.
    // it is overwritten with the constructor of board_t. Dont worry.
    square_t() : x(0), y(0) {}

    square_t(int xval, int yval) : x(xval), y(yval) {}
};

ostream& operator<<(ostream& os, square_t square) { // for pretty printing a square. It needs to be placed here. Otherwise compiler complains
    char color;
    switch (square.piece.color) {
    case Color::NONE:
        color = ' ';
        break;
    case Color::WHITE:
        color = 'w';
        break;
    case Color::BLACK:
        color = 'b';
        break;
    }
    char type;
    switch (square.piece.type) {
    case PieceType::PAWN:
        type = 'P';
        break;
    case PieceType::ROOK:
        type = 'R';
        break;
    case PieceType::KNIGHT:
        type = 'N';
        break;
    case PieceType::BISHOP:
        type = 'B';
        break;
    case PieceType::QUEEN:
        type = 'Q';
        break;
    case PieceType::KING:
        type = 'K';
        break;
    default:
        type = ' ';
    }
    return os << color << type;
}

struct board_t {
    array<array<square_t, 8>, 8> board;

    board_t() {
        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 8; j++) {
                board[i][j] = square_t(i, j);
            }
        }
    }

    void initialize_starting_board() {
        for (int x = 0; x < 8; x++) {
            for (int y = 0; y < 8; y++) {
                if (y == 0 || y == 1) {
                    if (y == 1) {
                        board[x][y].piece = pawn_t(Color::WHITE);
                    } else if (x == 0 || x == 7) {
                        board[x][y].piece = rook_t(Color::WHITE);
                    } else if (x == 1 || x == 6) {
                        board[x][y].piece = knight_t(Color::WHITE);
                    } else if (x == 2 || x == 5) {
                        board[x][y].piece = bishop_t(Color::WHITE);
                    } else if (x == 3) {
                        board[x][y].piece = queen_t(Color::WHITE);
                    } else {
                        board[x][y].piece = king_t(Color::WHITE);
                    }
                } else if (y == 6 || y == 7) {
                    if (y == 6) {
                        board[x][y].piece = pawn_t(Color::BLACK);
                    } else if (x == 0 || x == 7) {
                        board[x][y].piece = rook_t(Color::BLACK);
                    } else if (x == 1 || x == 6) {
                        board[x][y].piece = knight_t(Color::BLACK);
                    } else if (x == 2 || x == 5) {
                        board[x][y].piece = bishop_t(Color::BLACK);
                    } else if (x == 3) {
                        board[x][y].piece = queen_t(Color::BLACK);
                    } else {
                        board[x][y].piece = king_t(Color::BLACK);
                    }
                } else {
                    board[x][y].piece = piece_t(PieceType::EMPTY);
                }
            }
        }
    }

    void pretty_print_board() {
        for (int y = 7; y >= 0; y--) {
            cout << "   +----+----+----+----+----+----+----+----+" << endl;
            cout << " " << y + 1 << " ";
            for (int x = 0; x < 8; x++) {
                cout << "| " << board[x][y] << " ";
            }
            cout << "|" << endl;
        }
        cout << "   +----+----+----+----+----+----+----+----+" << endl;
        cout << "     a    b    c    d    e    f    g    h" << endl;
    }

    class board_iterator_t { // class for nice iteration over board
        array<array<square_t, 8>, 8>& board;
        int x = 0;
        int y = 0;

      public:
        board_iterator_t(array<array<square_t, 8>, 8>& b, int startX = 0, int startY = 0)
            : board(b), x(startX), y(startY) {}

        board_iterator_t& operator++() { // loops left to right, from bottom left to top right
            if (++y >= 8) {
                y = 0;
                ++x;
            }
            return *this;
        }

        square_t& operator*() { return board[x][y]; }

        bool operator!=(const board_iterator_t& other) {
            return x != other.x || y != other.y;
        }
    };

  public:
    board_iterator_t begin() { return board_iterator_t(board); }
    board_iterator_t end() { return board_iterator_t(board, 8, 0); }
};

#endif