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

    vector<square_t> line(move_t move) {
        vector<square_t> squares;

        bool up = move.from_y < move.to_y;
        bool right = move.from_x < move.to_x;

        // x and y increment based on the direction of the line
        int x_dir = 2*right-1;
        int y_dir = 2*up-1;


        if (move.from_x == move.to_x) { // Vertical line
            for (int j = min(move.from_y, move.to_y)+1; j <= max(move.from_y, move.to_y); j++) {
                squares.push_back(at(move.from_x, j));
            }
            return squares;
        } else if (move.from_y == move.to_y) { // Horizontal line
            for (int i = min(move.from_x, move.to_x)+1; i <= max(move.from_x, move.to_x); i++) {
                squares.push_back(at(i, move.from_y));
            }
            return squares;
        }


        // Diagonals

        printf("up,right (%i,%i)\n", x_dir, y_dir);

        for (int i = 1; i <= abs(move.to_x-move.from_x); i++) {
            squares.push_back(at(move.from_x+x_dir*i,move.from_y+y_dir*i));
        }

        return squares;
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

                // Initialize coordinates
                at(x,y).x = x;
                at(x,y).y = y;

                // Initial board setup
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
            // cout << y + 1 << " ";
            cout << y << " ";
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

    int is_move_legal(const move_t& move) {
        // Target does not contain a piece of the same color
        if (at(move.to_x, move.to_y).piece.color == at(move.from_x, move.from_y).piece.color) {
            return -1;
        }

        // Not jumping over piece (if not knight)
        if (at(move.from_x, move.from_y).piece.type != PieceType::KNIGHT) {
            // Get line of squares [from,to]
            //vector<square_t> line_squares = line();
        }

        // Move does not result in discovered check
        return 1;
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