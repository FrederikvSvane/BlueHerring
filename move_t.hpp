#ifndef move_t_hpp
#define move_t_hpp

#include "piece_t.hpp"

struct move_t {
    int from_x;
    int from_y;
    int to_x;
    int to_y;
    PieceType promotion_type;
    bool is_special; // for castling or en-passant
};

static int col_to_int(char col) {
    return col - 'a'; // fx 'a' => 0, 'b' => 1.
}

static int row_to_int(char row) {
    return (row - '1'); // fx '1' => 0, '2' => 1.
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

    // TODO: handle promotion properly
    PieceType promotion_type = PieceType::EMPTY;

    return move_t{from_x, from_y, to_x, to_y, promotion_type, false};
}

vector<move_t> translate_moves(const vector<string>& move_strings) {
    vector<move_t> moves;
    moves.reserve(move_strings.size());
    for (const string& move_str : move_strings) {
        moves.push_back(parse_move(move_str));
    }
    return moves;
}

#endif