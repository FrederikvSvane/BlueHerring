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

    static constexpr inline int index(int x, int y) {
        return static_cast<int>(x + (y << 3));
    }

    square_t& at(int x, int y) {
        return board[index(x, y)];
    }

    const square_t& at(int x, int y) const {
        return board[index(x, y)];
    }

    board_t() {
        for (int x = 0; x < 8; x++) {
            for (int y = 0; y < 8; y++) {
                at(x, y) = square_t();
            }
        }
    }

    void initialize_starting_board() {
        for (int x = 0; x < 8; x++) {
            for (int y = 0; y < 8; y++) {
                if (y == 0 || y == 1) {
                    if (y == 1) {
                        at(x, y).piece = pawn_t(Color::WHITE);
                    } else if (x == 0 || x == 7) {
                        at(x, y).piece = rook_t(Color::WHITE);
                    } else if (x == 1 || x == 6) {
                        at(x, y).piece = knight_t(Color::WHITE);
                    } else if (x == 2 || x == 5) {
                        at(x, y).piece = bishop_t(Color::WHITE);
                    } else if (x == 3) {
                        at(x, y).piece = queen_t(Color::WHITE);
                    } else {
                        at(x, y).piece = king_t(Color::WHITE);
                    }
                } else if (y == 6 || y == 7) {
                    if (y == 6) {
                        at(x, y).piece = pawn_t(Color::BLACK);
                    } else if (x == 0 || x == 7) {
                        at(x, y).piece = rook_t(Color::BLACK);
                    } else if (x == 1 || x == 6) {
                        at(x, y).piece = knight_t(Color::BLACK);
                    } else if (x == 2 || x == 5) {
                        at(x, y).piece = bishop_t(Color::BLACK);
                    } else if (x == 3) {
                        at(x, y).piece = queen_t(Color::BLACK);
                    } else {
                        at(x, y).piece = king_t(Color::BLACK);
                    }
                } else {
                    at(x, y).piece = piece_t(PieceType::EMPTY);
                }
            }
        }
    }

    void pretty_print_board() {
        cout << endl;
        cout << "  a b c d e f g h" << endl;
        for (int y = 7; y >= 0; y--) {
            cout << y + 1 << " ";
            for (int x = 0; x < 8; x++) {
                cout << at(x, y) << " ";
            }
            cout << endl;
        }
    }

    inline void make_move(const move_t& move) { // using inline for optimization
        square_t& from_square = at(move.from_x, move.from_y);
        square_t& to_square = at(move.to_x, move.to_y);

        to_square.piece = from_square.piece;
        from_square.piece = piece_t{};

        if (move.promotion_type != PieceType::EMPTY) {
            to_square.piece.type = move.promotion_type;
        }
    }

    class board_iterator_t {
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

        bool operator!=(const board_iterator_t& other) {
            return x != other.x || y != other.y;
        }
    };

  public:
    board_iterator_t begin() { return board_iterator_t(board); }
    board_iterator_t end() { return board_iterator_t(board, 8, 0); }
};

#endif