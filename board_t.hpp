#ifndef board_t_hpp
#define board_t_hpp
#include "move_t.hpp"
#include "operators_util.hpp"
#include "piece_t.hpp"
#include "square_t.hpp"
#include <array>
#include <iostream>

using namespace std;

struct board_t {
    array<square_t, 64> board;

    board_t() {
        for (int x = 0; x < 8; x++) {
            for (int y = 0; y < 8; y++) {
                at(x, y) = square_t();
            }
        }
    }

    static constexpr inline int index(int x, int y) {
        return static_cast<int>(x + (y << 3)); // same as x + y*8 (but faster!)
    }

    square_t& at(int x, int y) {
        return board[index(x, y)];
    }

    const square_t& at(int x, int y) const {
        return board[index(x, y)];
    }

    int in_board(int x, int y) const {
        return 0 <= x && x <= 7 && 0 <= y && y <= 7;
    }

    void initialize_starting_board() {
        for (int x = 0; x < 8; x++) {
            for (int y = 0; y < 8; y++) {
                // initialize coordinates of all square_t
                at(x, y).x = x;
                at(x, y).y = y;

                // initialize pieces
                if (y == 0 || y == 1) {
                    if (y == 1) {
                        at(x, y).piece = piece_t(PieceType::PAWN, Color::WHITE);
                    } else if (x == 0 || x == 7) {
                        at(x, y).piece = piece_t(PieceType::ROOK, Color::WHITE);
                    } else if (x == 1 || x == 6) {
                        at(x, y).piece = piece_t(PieceType::KNIGHT, Color::WHITE);
                    } else if (x == 2 || x == 5) {
                        at(x, y).piece = piece_t(PieceType::BISHOP, Color::WHITE);
                    } else if (x == 3) {
                        at(x, y).piece = piece_t(PieceType::QUEEN, Color::WHITE);
                    } else {
                        at(x, y).piece = piece_t(PieceType::KING, Color::WHITE);
                    }
                } else if (y == 6 || y == 7) {
                    if (y == 6) {
                        at(x, y).piece = piece_t(PieceType::PAWN, Color::BLACK);
                    } else if (x == 0 || x == 7) {
                        at(x, y).piece = piece_t(PieceType::ROOK, Color::BLACK);
                    } else if (x == 1 || x == 6) {
                        at(x, y).piece = piece_t(PieceType::KNIGHT, Color::BLACK);
                    } else if (x == 2 || x == 5) {
                        at(x, y).piece = piece_t(PieceType::BISHOP, Color::BLACK);
                    } else if (x == 3) {
                        at(x, y).piece = piece_t(PieceType::QUEEN, Color::BLACK);
                    } else {
                        at(x, y).piece = piece_t(PieceType::KING, Color::BLACK);
                    }
                } else {
                    at(x, y).piece = piece_t(PieceType::EMPTY, Color::NONE);
                }
            }
        }
    }

    void pretty_print_board() {
        cout << endl;
        cout << "  a b c d e f g h" << endl;
        for (int y = 7; y >= 0; y--) {
            // cout << y + 1 << " ";
            cout << y << " ";
            for (int x = 0; x < 8; x++) {
                cout << at(x, y) << " ";
            }
            cout << endl;
        }
    }

    class board_iterator_t { // to facilitate looping over the board, square by square, starting from the bottom left
        array<square_t, 64>& board;
        int x = 0;
        int y = 0;

      public:
        board_iterator_t(array<square_t, 64>& b, int startX = 0, int startY = 0)
            : board(b), x(startX), y(startY) {}

        board_iterator_t& operator++() {
            if (++y >= 8) {
                y = 0;
                ++x;
            }
            return *this;
        }

        square_t& operator*() {
            return board[index(x, y)];
        }

        bool operator!=(const board_iterator_t& other) const {
            return x != other.x || y != other.y;
        }
    };

    class const_board_iterator_t { // to facilitate looping over the board, without modifying anything
        const array<square_t, 64>& board;
        int x = 0;
        int y = 0;

      public:
        const_board_iterator_t(const array<square_t, 64>& b, int startX = 0, int startY = 0)
            : board(b), x(startX), y(startY) {}

        const_board_iterator_t& operator++() {
            if (++y >= 8) {
                y = 0;
                ++x;
            }
            return *this;
        }

        const square_t& operator*() const {
            return board[index(x, y)];
        }

        bool operator!=(const const_board_iterator_t& other) const {
            return x != other.x || y != other.y;
        }
    };

  public:
    board_iterator_t begin() { return board_iterator_t(board); }
    board_iterator_t end() { return board_iterator_t(board, 8, 0); }
    const_board_iterator_t begin() const { return const_board_iterator_t(board); }
    const_board_iterator_t end() const { return const_board_iterator_t(board, 8, 0); }
};

#endif