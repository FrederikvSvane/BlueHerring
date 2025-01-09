#ifndef move_t_hpp
#define move_t_hpp

#include <iostream>
#include "piece_t.hpp"
#include "board_t.hpp"

struct move_t {
    int from_x;
    int from_y;
    int to_x;
    int to_y;
    PieceType promotion_type;
};

static int col_to_int(char col) {
    return col - 'a'; // fx 'a' => 0, 'b' => 1.
}
string int_to_col(int x) {
    return string(1, "abcdefgh"[x]);
}

static int row_to_int(char row) {
    return (row - '1'); // fx '1' => 0, '2' => 1.
}
string int_to_row(int y) {
    return to_string(y + 1);
}

PieceType char_to_piece(char p) {
    switch (p) {
    case 'q': return PieceType::QUEEN;
    case 'n': return PieceType::KNIGHT;
    case 'r': return PieceType::ROOK;
    case 'b': return PieceType::BISHOP;    
    default: return PieceType::EMPTY;
    }
}
string piece_to_string(PieceType piece) {
    switch (piece) {
    case PieceType::QUEEN: return string(1, 'q');
    case PieceType::KNIGHT: return string(1, 'n');
    case PieceType::ROOK: return string(1, 'r');
    case PieceType::BISHOP: return string(1, 'b');
    default: return "";
    }
}

move_t parse_move(const string& move_str) { // move_str example: "e2e4"
    if (move_str.length() < 4) {
        throw invalid_argument("Move string too short");
    }
    int from_x = col_to_int(move_str[0]);
    int from_y = row_to_int(move_str[1]);
    int to_x = col_to_int(move_str[2]);
    int to_y = row_to_int(move_str[3]);

    if (from_x < 0 || from_x > 7 || from_y < 0 || from_y > 7 || // validate coordinates are within bounds (do we need this tho? can the input file lie?)
        to_x < 0 || to_x > 7 || to_y < 0 || to_y > 7) {
        throw invalid_argument("Invalid coordinates in move string");
    }

    PieceType promotion_type = (move_str.length() > 4) ? char_to_piece(move_str[4]) : PieceType::EMPTY;

    return move_t{from_x, from_y, to_x, to_y, promotion_type};
}

string encode_move(const move_t& move) {
    string move_str = int_to_col(move.from_x) + int_to_row(move.from_y) + int_to_col(move.to_x) + int_to_row(move.to_y);
    return (move.promotion_type == PieceType::EMPTY) ? move_str : move_str + piece_to_string(move.promotion_type);
}

vector<move_t> translate_moves(const vector<string>& move_strings) {
    vector<move_t> moves;
    moves.reserve(move_strings.size());
    for (const string& move_str : move_strings) {
        moves.push_back(parse_move(move_str));
    }
    return moves;
}

bool operator==(const move_t move, const move_t other){
    return ((move.from_x == other.from_x) && (move.from_y == other.from_y) && (move.to_x == other.to_x) && (move.to_y == other.to_y));
}

bool operator!=(const move_t move, const move_t other){
    return not (move == other);
}

bool better(board_t board, const move_t move, const move_t other, int depth){
    // the comparison function to order moves from best to least
    return move_eval::move_eval(board, move, depth) > move_eval::move_eval(board, other, depth);
}

#endif