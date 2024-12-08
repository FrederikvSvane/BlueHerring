#ifndef piece_t_hpp
#define piece_t_hpp
#include <iostream>
#include <vector>
#include <array>
#include "move_t.hpp"

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

    virtual vector<move_t> eval(int x, int y);
};

struct pawn_t : piece_t {
    // STILL NEED TO IMPLEMENT THE PROMOTION MOVE
    pawn_t(Color colorval) : piece_t(colorval) {
        type = PieceType::PAWN;
    }

    vector<move_t> eval(int x, int y) override {
        vector<move_t> moves;
        int direction = (color == Color::WHITE) ? 1 : -1;

        if (y == 1 && color == Color::WHITE) {
            //If it's the first move, we can move one or two squares
            moves.push_back(move_t {x, y, x, 2, PieceType::EMPTY, false});
            moves.push_back(move_t {x, y, x, 3, PieceType::EMPTY, false});
        } else if (y == 6 && color == Color::BLACK) {
            //If it's the first move, we can move one or two squares
            moves.push_back(move_t {x, y, x, 4, PieceType::EMPTY, false});
            moves.push_back(move_t {x, y, x, 5, PieceType::EMPTY, false});
        } else {
            if(0 <= y + direction && y + direction < 8)
            moves.push_back(move_t {x, y, x, y + direction, PieceType::EMPTY, false});
        }
        return moves;
    }
};

struct rook_t : piece_t {
    rook_t(Color colorval) : piece_t(colorval) {
        type = PieceType::ROOK;
    }

    vector<move_t> eval(int x, int y) override {
        vector<move_t> moves;
        // horizontal moves
        for (int to_x = 0; to_x < 8; to_x++) {
            if (to_x != x)
                moves.push_back(move_t {x, y, to_x, y, PieceType::EMPTY, false});
        }
        //vertical moves
        for (int to_y = 0; to_y < 8; to_y++) {
            if (to_y != y)
                moves.push_back(move_t {x, y, x, to_y, PieceType::EMPTY, false});
        }
        return moves;
    }
};

struct knight_t : piece_t {
    knight_t(Color colorval) : piece_t(colorval) {
        type = PieceType::KNIGHT;
    }

    vector<move_t> eval(int x, int y) override {
        vector<move_t> moves;
        vector<std::array<int, 2>> offsets = {
        {x + 1, y + 2}, {x - 1, y + 2}, {x + 1, y - 2}, {x - 1, y - 2},
        {x + 2, y + 1}, {x - 2, y + 1}, {x + 2, y - 1}, {x - 2, y - 1}
        };
        for (const auto& [to_x, to_y] : offsets) {
            if(0 <= to_x && to_x <= 7 && 0 <= to_y && to_y <= 7){
                moves.push_back(move_t {x, y, to_x, to_y, PieceType::EMPTY, false});
            };
        }
        return moves;
    }
};

struct bishop_t : piece_t {
    bishop_t(Color colorval) : piece_t(colorval) {
        type = PieceType::BISHOP;
    }

    vector<move_t> eval(int x, int y) override {
        vector<move_t> moves;
        vector<std::array<int, 2>> diagonals = { {1, 1}, {1, -1}, {-1, 1}, {-1, -1}};
        for(const auto& [dx, dy] : diagonals) {
            for(int i = 1; i <= 7; i++){
                int to_x = x + dx*i;
                int to_y = y + dy*i;
                if(0 <= to_x && to_x <= 7 && 0 <= to_y && to_y <= 7){
                    moves.push_back(move_t {x, y, to_x, to_y, PieceType::EMPTY, false});
                }
                else{
                    break;
                }
            }
        }
        return moves;
    }
};

struct queen_t : piece_t {
    queen_t(Color colorval) : piece_t(colorval) {
        type = PieceType::QUEEN;
    }

    vector<move_t> eval(int x, int y) override {
        vector<move_t> moves;
        //horizontal moves
        for (int to_x = 0; to_x < 8; to_x++) {
            if (to_x != x)
                moves.push_back(move_t {x, y, to_x, y, PieceType::EMPTY, false});
        }
        //vertical moves
        for (int to_y = 0; to_y < 8; to_y++) {
            if (to_y != y)
                moves.push_back(move_t {x, y, x, to_y, PieceType::EMPTY, false});
        }

        //and now diagonals
        vector<std::array<int, 2>> diagonals = { {1, 1}, {1, -1}, {-1, 1}, {-1, -1}};
        for(const auto& [dx, dy] : diagonals) {
            for(int i = 1; i <= 7; i++){
                int to_x = x + dx*i;
                int to_y = y + dy*i;
                if(0 <= to_x && to_x <= 7 && 0 <= to_y && to_y <= 7){
                    moves.push_back(move_t {x, y, to_x, to_y, PieceType::EMPTY, false});
                }
                else{
                    break;
                }
            }
        }
        return moves;
    }
};

struct king_t : piece_t {
    king_t(Color colorval) : piece_t(colorval) {
        type = PieceType::KING;
    }

    vector<move_t> eval(int x, int y) override {
        vector<move_t> moves;
        vector<std::array<int, 2>> offsets = {
            {x + 1, y + 1}, {x + 1, y}, {x + 1, y - 1}, {x, y - 1}, 
            {x - 1, y - 1}, {x - 1, y}, {x - 1, y + 1}, {x, y + 1}
        };
        for (const auto& [to_x, to_y] : offsets) {
            if(0 <= to_x && to_x <= 7 && 0 <= to_y  && to_y <= 7){
                moves.push_back(move_t {x, y, to_x, to_y, PieceType::EMPTY, false});
            };
        }
        return moves;
    }
};

#endif