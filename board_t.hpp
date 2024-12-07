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

    int in_board(int x, int y) {
        return 0 <= x && x <= 7 && 0 <= y && y <= 7;
    }

    vector<square_t> line(const move_t& move) {
        vector<square_t> squares;

        bool up = move.from_y < move.to_y;
        bool right = move.from_x < move.to_x;

        if (move.from_x == move.to_x) {
            // Vertical line
            if (move.from_y < move.to_y) {
                // Vertical line going up
                for (int j = move.from_y+1; j < move.to_y; j++) {
                    squares.push_back(at(move.from_x, j));
                }
                return squares;
            }
            else {
                // Vertical line going down
                for (int j = move.from_y-1; j > move.to_y; j--) {
                    squares.push_back(at(move.from_x, j));
                }
                return squares;
            }
        } else if (move.from_y == move.to_y) {
            // Horizontal line
            if (move.from_x < move.to_x) {
                // Horizontal line going right
                for (int i = move.from_x+1; i < move.to_x; i++) {
                    squares.push_back(at(i, move.from_y));
                }
                return squares;
            }
            else {
                // Horizontal line going left
                for (int i = move.from_x-1; i > move.to_x; i--) {
                    squares.push_back(at(i, move.from_y));
                }
                return squares;
            }
        }

        // x and y increment based on the direction of the line
        int x_dir = 2*right-1;
        int y_dir = 2*up-1;
        // Diagonals
        for (int i = 1; i < abs(move.to_x-move.from_x); i++) {
            squares.push_back(at(move.from_x+x_dir*i,move.from_y+y_dir*i));
        }
        return squares;
    }

    array<int, 2> is_in_check_line(vector<square_t> diagonal, Color color, bool is_diagonal) { 
        for (const auto& square : diagonal) {
            if (at(square.x, square.y).piece.type != PieceType::EMPTY) {
                // this is the non-empty square on the diagonal closest to the king
                if (at(square.x, square.y).piece.color != color &&
                    (is_diagonal && (at(square.x, square.y).piece.type == PieceType::BISHOP || at(square.x, square.y).piece.type == PieceType::QUEEN) ||
                    (!is_diagonal && at(square.x, square.y).piece.type == PieceType::ROOK))) {
                    // if (opposing piece) and ([diagonal and (queen or bishop)] or [line and rook])
                    return {square.x, square.y};
                }
                else { // it's either an opposing piece that does not attack along this line or an opposing pawm or a piece of the color of the king
                    return {8, 8};
                }
            }
        }
        return {8, 8};
    }

    int is_in_check(Color color) {
        int is_checked = false;
        for (auto & square : board) {
            if (square.piece.type == PieceType::KING && square.piece.color == color) { 
                vector<array<int, 2>> check;
                int k_x = square.x;
                int k_y = square.y;

                // checking for knights
                vector<array<int, 2>> attacked_by_knight = {
                    {k_x + 1, k_y + 2}, {k_x - 1, k_y + 2}, {k_x + 1, k_y - 2}, {k_x - 1, k_y - 2},
                    {k_x + 2, k_y + 1}, {k_x - 2, k_y + 1}, {k_x + 2, k_y - 1}, {k_x - 2, k_y - 1}};
                for (const auto& [x, y] : attacked_by_knight) {
                    if (in_board(x, y) && at(x, y).piece.type == PieceType::KNIGHT && at(x, y).piece.color != color) {
                        // attacked by a knight at (x, y)
                        check.push_back({x, y});
                        is_checked = true;
                    }
                }

                // checking each diagonal for bishops and queen
                vector<square_t> diagonal_ur = line(move_t{k_x, k_y, k_x+min(7-k_x, 7-k_y), k_y+min(7-k_x, 7-k_y), PieceType::EMPTY, 0});
                vector<square_t> diagonal_ul = line(move_t{k_x, k_y, k_x-min(k_x, 7-k_y), k_y+min(k_x, 7-k_y), PieceType::EMPTY, 0});
                vector<square_t> diagonal_dr = line(move_t{k_x, k_y, k_x+min(7-k_x, k_y), k_y-min(7-k_x, k_y), PieceType::EMPTY, 0});
                vector<square_t> diagonal_dl = line(move_t{k_x, k_y, k_x-min(k_x, k_y), k_y-min(k_x, k_y), PieceType::EMPTY, 0});

                array<int, 2> tmp;
                for (const auto& diagonal : array<vector<square_t>, 4> {diagonal_ur, diagonal_ul, diagonal_dr, diagonal_dl}) {
                    tmp = is_in_check_line(diagonal, color, true);
                    if (tmp != array<int, 2> {8, 8}) {
                        check.push_back(tmp);
                        is_checked = true;
                    }
                }

                // checking each line for rooks
                vector<square_t> line_u = line(move_t{k_x, k_y, k_x, 7, PieceType::EMPTY, 0});
                vector<square_t> line_r = line(move_t{k_x, k_y, 7, k_y, PieceType::EMPTY, 0});
                vector<square_t> line_d = line(move_t{k_x, k_y, k_x, 0, PieceType::EMPTY, 0});
                vector<square_t> line_l = line(move_t{k_x, k_y, 0, k_y, PieceType::EMPTY, 0});

                for (const auto& diagonal : array<vector<square_t>, 4>{diagonal_ur, diagonal_ul, diagonal_dr, diagonal_dl}) {
                    tmp = is_in_check_line(diagonal, color, false);
                    if (tmp != array<int, 2>{8, 8}) {
                        check.push_back(tmp);
                        is_checked = true;
                    }
                }

                // checking for pawns
                array<array<int, 2>, 2> attacked_by_pawn;
                switch (color) {
                case Color::WHITE:
                    attacked_by_pawn = {{{k_x+1, k_y+1}, {k_x-1, k_y+1}}};                    
                case Color::BLACK:
                    attacked_by_pawn = {{{k_x+1, k_y-1}, {k_x-1, k_y-1}}};
                default:
                    return 2;
                }

                for (auto& [x, y] : attacked_by_pawn)
                if (in_board(x, y) && at(x, y).piece.type == PieceType::PAWN && at(x, y).piece.color != color) {
                    check.push_back({x, y});
                    is_checked = true;
                }
            }
        }
        return is_checked;
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
        vector<square_t> line_of_move = line(move);

        if (at(move.from_x, move.from_y).piece.type != PieceType::KNIGHT) { // Not jumping over a piece
            for (auto & square : line_of_move) {
                if (at(square.x, square.y).piece.type != PieceType::EMPTY) {
                    return 0;
                }
            }
        }
        if (at(move.to_x, move.to_y).piece.color == at(move.from_x, move.from_y).piece.color) { // Not going to an occupoed square
            return 0;
        }
        // Not revealing a check
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