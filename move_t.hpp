#ifndef move_t_hpp
#define move_t_hpp

#include "piece_t.hpp"
#include <string>

// for prettier code
using U64 = unsigned long long;

// forward declarations
struct coordinate_move_t;
struct bitboard_move_t;
coordinate_move_t bitboard_move_to_coordinate_move(const bitboard_move_t& move);
bitboard_move_t coordinate_move_to_bitboard_move(const coordinate_move_t& move);

// Normal, coordinate based move type
struct coordinate_move_t {
    int from_x;
    int from_y;
    int to_x;
    int to_y;
    PieceType promotion_type;

    coordinate_move_t() : from_x(0), from_y(0), to_x(0), to_y(0), promotion_type(PieceType::EMPTY) {}
    coordinate_move_t(int fx, int fy, int tx, int ty, PieceType prom = PieceType::EMPTY)
        : from_x(fx), from_y(fy), to_x(tx), to_y(ty), promotion_type(prom) {}
};

// Alternate, bitboard based move type
struct bitboard_move_t {
    U64 from_board;
    U64 to_board;
    PieceType promotion_type;

    bitboard_move_t() : from_board(0), to_board(0), promotion_type(PieceType::EMPTY) {}
    bitboard_move_t(U64 from, U64 to, PieceType prom = PieceType::EMPTY)
        : from_board(from), to_board(to), promotion_type(prom) {}
    // Constructor taking x,y coordinates (like coordinate_move_t)
    bitboard_move_t(int fx, int fy, int tx, int ty, PieceType prom = PieceType::EMPTY)
        : from_board(1ULL << (fx + fy * 8)),
          to_board(1ULL << (tx + ty * 8)),
          promotion_type(prom) {}
};

constexpr int MAX_MOVES = 218; // Maximum possible moves in any chess position

struct move_list_t {
    bitboard_move_t moves[MAX_MOVES];
    int count;

    move_list_t() : count(0) {}

    void add(const bitboard_move_t& move) {
        if (count < MAX_MOVES) {
            moves[count++] = move;
        }
    }
};

static int col_to_int(char col) {
    return col - 'a'; // fx 'a' => 0, 'b' => 1.
}
static string int_to_col(int x) {
    return string(1, "abcdefgh"[x]);
}

static int row_to_int(char row) {
    return (row - '1'); // fx '1' => 0, '2' => 1.
}
string int_to_row(int y) {
    return to_string(y + 1);
}

static PieceType char_to_piece(char p) {
    switch (p) {
    case 'q': return PieceType::QUEEN;
    case 'n': return PieceType::KNIGHT;
    case 'r': return PieceType::ROOK;
    case 'b': return PieceType::BISHOP;
    default: return PieceType::EMPTY;
    }
}
static string piece_to_string(PieceType piece) {
    switch (piece) {
    case PieceType::QUEEN: return "q";
    case PieceType::KNIGHT: return "n";
    case PieceType::ROOK: return "r";
    case PieceType::BISHOP: return "b";
    default: return "";
    }
}

inline coordinate_move_t bitboard_move_to_coordinate_move(const bitboard_move_t& move) {
    int from_idx = __builtin_ctzll(move.from_board); // builtin GNU function. ctzll = count trailing zeroes long long (eg. ctz(10000) = 4)
    int to_idx   = __builtin_ctzll(move.to_board);
    return coordinate_move_t{
        from_idx % 8,
        from_idx / 8,
        to_idx % 8,
        to_idx / 8,
        move.promotion_type};
}

inline bitboard_move_t coordinate_move_to_bitboard_move(const coordinate_move_t& move) {
    U64 from = 1ULL << (move.from_y * 8 + move.from_x);
    U64 to   = 1ULL << (move.to_y * 8 + move.to_x);
    return bitboard_move_t(from, to, move.promotion_type);
}

inline coordinate_move_t parse_move_from_string(const string& move_str) { // move_str example: "e2e4"
    if (move_str.length() < 4) {
        throw invalid_argument("Move string too short");
    }
    int from_x = col_to_int(move_str[0]);
    int from_y = row_to_int(move_str[1]);
    int to_x   = col_to_int(move_str[2]);
    int to_y   = row_to_int(move_str[3]);

    if (from_x < 0 || from_x > 7 || from_y < 0 || from_y > 7 || // validate coordinates are within bounds (do we need this tho? can the input file lie?)
        to_x < 0 || to_x > 7 || to_y < 0 || to_y > 7) {
        throw invalid_argument("Invalid coordinates in move string");
    }

    PieceType promotion_type = (move_str.length() > 4) ? char_to_piece(move_str[4]) : PieceType::EMPTY;

    return coordinate_move_t{from_x, from_y, to_x, to_y, promotion_type};
}

inline string encode_move(const coordinate_move_t& move) {
    string move_str = int_to_col(move.from_x) + int_to_row(move.from_y) + int_to_col(move.to_x) + int_to_row(move.to_y);
    return (move.promotion_type == PieceType::EMPTY) ? move_str : move_str + piece_to_string(move.promotion_type);
}

inline vector<coordinate_move_t> translate_to_coordinate_moves(const vector<string>& move_strings) {
    vector<coordinate_move_t> moves;
    moves.reserve(move_strings.size());
    for (const string& move_str : move_strings) {
        moves.push_back(parse_move_from_string(move_str));
    }
    return moves;
}

inline vector<bitboard_move_t> translate_to_bitboard_moves(const vector<string>& move_strings) {
    vector<bitboard_move_t> moves;
    moves.reserve(move_strings.size());
    for (const string& move_str : move_strings) {
        moves.push_back(coordinate_move_to_bitboard_move(parse_move_from_string(move_str)));
    }
    return moves;
}

// This converts a possible moves board (1's on the squares where the piece can move to) into actual bitboard_move_t's
inline move_list_t get_moves_from_possible_moves_bitboard(U64 possible_moves_board, U64 from_square) {
    move_list_t moves;
    while (possible_moves_board) {
        U64 to_square = 1ULL << __builtin_ctzll(possible_moves_board);
        moves.add(bitboard_move_t(from_square, to_square));
        possible_moves_board &= possible_moves_board - 1;
    }
    return moves;
}

// This does the same as the one above, except it also handles promotions (which only pawns can do, duh!)
inline move_list_t get_pawn_moves_from_possible_moves_bitboard(U64 possible_moves_board, U64 from_square, bool is_promotion) {
    move_list_t moves;
    while (possible_moves_board) {
        U64 to_square = 1ULL << __builtin_ctzll(possible_moves_board);
        if (is_promotion) {
            moves.add(bitboard_move_t(from_square, to_square, PieceType::QUEEN));
            moves.add(bitboard_move_t(from_square, to_square, PieceType::ROOK));
            moves.add(bitboard_move_t(from_square, to_square, PieceType::BISHOP));
            moves.add(bitboard_move_t(from_square, to_square, PieceType::KNIGHT));
        } else {
            moves.add(bitboard_move_t(from_square, to_square));
        }
        possible_moves_board &= (possible_moves_board - 1);
    }
    return moves;
}

#endif