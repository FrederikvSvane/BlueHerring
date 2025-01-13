#ifndef eval_hpp
#define eval_hpp

#include "board_t.hpp"

namespace eval {

using Score = int; // this is purely for code clarity

// the values used here are based on an analysis from chessprogramming.org

constexpr Score PAWN_VALUE   = 100;
constexpr Score KNIGHT_VALUE = 320;
constexpr Score BISHOP_VALUE = 330;
constexpr Score ROOK_VALUE  = 500;
constexpr Score QUEEN_VALUE = 900;
constexpr Score KING_VALUE  = 20000;

// from white's perspective
constexpr Score PAWN_TABLE[64] = {
     0,  0,  0,  0,  0,  0,  0,  0,
    50, 50, 50, 50, 50, 50, 50, 50,
    10, 10, 20, 30, 30, 20, 10, 10,
     5,  5, 10, 25, 25, 10,  5,  5,
     0,  0,  0, 20, 20,  0,  0,  0,
     5, -5,-10,  0,  0,-10, -5,  5,
     5, 10, 10,-20,-20, 10, 10,  5,
     0,  0,  0,  0,  0,  0,  0,  0
};

constexpr Score KNIGHT_TABLE[64] = {
    -50,-40,-30,-30,-30,-30,-40,-50,
    -40,-20,  0,  0,  0,  0,-20,-40,
    -30,  0, 10, 15, 15, 10,  0,-30,
    -30,  5, 15, 20, 20, 15,  5,-30,
    -30,  0, 15, 20, 20, 15,  0,-30,
    -30,  5, 10, 15, 15, 10,  5,-30,
    -40,-20,  0,  5,  5,  0,-20,-40,
    -50,-40,-30,-30,-30,-30,-40,-50
};

constexpr Score BISHOP_TABLE[64] = {
    -20,-10,-10,-10,-10,-10,-10,-20,
    -10,  0,  0,  0,  0,  0,  0,-10,
    -10,  0,  5, 10, 10,  5,  0,-10,
    -10,  5,  5, 10, 10,  5,  5,-10,
    -10,  0, 10, 10, 10, 10,  0,-10,
    -10, 10, 10, 10, 10, 10, 10,-10,
    -10,  5,  0,  0,  0,  0,  5,-10,
    -20,-10,-10,-10,-10,-10,-10,-20
};

constexpr Score ROOK_TABLE[64] = {
     0,  0,  0,  0,  0,  0,  0,  0,
     5, 10, 10, 10, 10, 10, 10,  5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    -5,  0,  0,  0,  0,  0,  0, -5,
     0,  0,  0,  5,  5,  0,  0,  0
};

constexpr Score QUEEN_TABLE[64] = {
    -20,-10,-10, -5, -5,-10,-10,-20,
    -10,  0,  0,  0,  0,  0,  0,-10,
    -10,  0,  5,  5,  5,  5,  0,-10,
     -5,  0,  5,  5,  5,  5,  0, -5,
      0,  0,  5,  5,  5,  5,  0, -5,
    -10,  5,  5,  5,  5,  5,  0,-10,
    -10,  0,  5,  0,  0,  0,  0,-10,
    -20,-10,-10, -5, -5,-10,-10,-20
};

constexpr Score KING_TABLE[64] = {
    -30,-40,-40,-50,-50,-40,-40,-30,
    -30,-40,-40,-50,-50,-40,-40,-30,
    -30,-40,-40,-50,-50,-40,-40,-30,
    -30,-40,-40,-50,-50,-40,-40,-30,
    -20,-30,-30,-40,-40,-30,-30,-20,
    -10,-20,-20,-20,-20,-20,-20,-10,
     20, 20,  0,  0,  0,  0, 20, 20,
     20, 30, 10,  0,  0, 10, 30, 20
};

Score get_piece_square_value(PieceType type, int square_idx, Color color) {
    // Flip square index for black pieces
    int adjusted_index = (color == Color::WHITE) ? (63 - square_idx) : square_idx;
    
    switch (type) {
        case PieceType::PAWN: return PAWN_TABLE[adjusted_index];
        case PieceType::KNIGHT: return KNIGHT_TABLE[adjusted_index];
        case PieceType::BISHOP: return BISHOP_TABLE[adjusted_index];
        case PieceType::ROOK: return ROOK_TABLE[adjusted_index];
        case PieceType::QUEEN: return QUEEN_TABLE[adjusted_index];
        case PieceType::KING: return KING_TABLE[adjusted_index];
        default: return 0;
    }
}

Score evaluate_position(const bitboard_t& board) {
    Score score = 0;

    // Helper function to evaluate pieces of a specific type
    auto evaluate_pieces = [&](U64 bitboard, PieceType type, Color color) {
        Score piece_value = 0;
        switch (type) {
            case PieceType::PAWN: piece_value = PAWN_VALUE; break;
            case PieceType::KNIGHT: piece_value = KNIGHT_VALUE; break;
            case PieceType::BISHOP: piece_value = BISHOP_VALUE; break;
            case PieceType::ROOK: piece_value = ROOK_VALUE; break;
            case PieceType::QUEEN: piece_value = QUEEN_VALUE; break;
            case PieceType::KING: piece_value = KING_VALUE; break;
            default: break;
        }

        U64 pieces = bitboard;
        while (pieces) {
            int square_idx = __builtin_ctzll(pieces); // Get index of least significant 1-bit
            score += (color == Color::WHITE) ? 
                    (piece_value + get_piece_square_value(type, square_idx, color)) :
                    -(piece_value + get_piece_square_value(type, square_idx, color));
            pieces &= (pieces - 1); // Clear least significant 1-bit
        }
    };

    // Evaluate white pieces
    evaluate_pieces(board.board_w_P, PieceType::PAWN, Color::WHITE);
    evaluate_pieces(board.board_w_N, PieceType::KNIGHT, Color::WHITE);
    evaluate_pieces(board.board_w_B, PieceType::BISHOP, Color::WHITE);
    evaluate_pieces(board.board_w_R, PieceType::ROOK, Color::WHITE);
    evaluate_pieces(board.board_w_Q, PieceType::QUEEN, Color::WHITE);
    evaluate_pieces(board.board_w_K, PieceType::KING, Color::WHITE);

    // Evaluate black pieces
    evaluate_pieces(board.board_b_P, PieceType::PAWN, Color::BLACK);
    evaluate_pieces(board.board_b_N, PieceType::KNIGHT, Color::BLACK);
    evaluate_pieces(board.board_b_B, PieceType::BISHOP, Color::BLACK);
    evaluate_pieces(board.board_b_R, PieceType::ROOK, Color::BLACK);
    evaluate_pieces(board.board_b_Q, PieceType::QUEEN, Color::BLACK);
    evaluate_pieces(board.board_b_K, PieceType::KING, Color::BLACK);

    return score;
}

} // namespace eval

namespace bit_eval {

using Score = int; // this is purely for code clarity

// the values used here are based on an analysis from chessprogramming.org

constexpr Score PAWN_VALUE   = 100;
constexpr Score KNIGHT_VALUE = 320;
constexpr Score BISHOP_VALUE = 330;
constexpr Score ROOK_VALUE  = 500;
constexpr Score QUEEN_VALUE = 900;
constexpr Score KING_VALUE  = 20000;

// from white's perspective
constexpr Score PAWN_TABLE[64] = {
     0,  0,  0,  0,  0,  0,  0,  0,
    50, 50, 50, 50, 50, 50, 50, 50,
    10, 10, 20, 30, 30, 20, 10, 10,
     5,  5, 10, 25, 25, 10,  5,  5,
     0,  0,  0, 20, 20,  0,  0,  0,
     5, -5,-10,  0,  0,-10, -5,  5,
     5, 10, 10,-20,-20, 10, 10,  5,
     0,  0,  0,  0,  0,  0,  0,  0
};

constexpr Score KNIGHT_TABLE[64] = {
    -50,-40,-30,-30,-30,-30,-40,-50,
    -40,-20,  0,  0,  0,  0,-20,-40,
    -30,  0, 10, 15, 15, 10,  0,-30,
    -30,  5, 15, 20, 20, 15,  5,-30,
    -30,  0, 15, 20, 20, 15,  0,-30,
    -30,  5, 10, 15, 15, 10,  5,-30,
    -40,-20,  0,  5,  5,  0,-20,-40,
    -50,-40,-30,-30,-30,-30,-40,-50
};

constexpr Score BISHOP_TABLE[64] = {
    -20,-10,-10,-10,-10,-10,-10,-20,
    -10,  0,  0,  0,  0,  0,  0,-10,
    -10,  0,  5, 10, 10,  5,  0,-10,
    -10,  5,  5, 10, 10,  5,  5,-10,
    -10,  0, 10, 10, 10, 10,  0,-10,
    -10, 10, 10, 10, 10, 10, 10,-10,
    -10,  5,  0,  0,  0,  0,  5,-10,
    -20,-10,-10,-10,-10,-10,-10,-20
};

constexpr Score ROOK_TABLE[64] = {
     0,  0,  0,  0,  0,  0,  0,  0,
     5, 10, 10, 10, 10, 10, 10,  5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    -5,  0,  0,  0,  0,  0,  0, -5,
     0,  0,  0,  5,  5,  0,  0,  0
};

constexpr Score QUEEN_TABLE[64] = {
    -20,-10,-10, -5, -5,-10,-10,-20,
    -10,  0,  0,  0,  0,  0,  0,-10,
    -10,  0,  5,  5,  5,  5,  0,-10,
     -5,  0,  5,  5,  5,  5,  0, -5,
      0,  0,  5,  5,  5,  5,  0, -5,
    -10,  5,  5,  5,  5,  5,  0,-10,
    -10,  0,  5,  0,  0,  0,  0,-10,
    -20,-10,-10, -5, -5,-10,-10,-20
};

constexpr Score KING_TABLE[64] = {
    -30,-40,-40,-50,-50,-40,-40,-30,
    -30,-40,-40,-50,-50,-40,-40,-30,
    -30,-40,-40,-50,-50,-40,-40,-30,
    -30,-40,-40,-50,-50,-40,-40,-30,
    -20,-30,-30,-40,-40,-30,-30,-20,
    -10,-20,-20,-20,-20,-20,-20,-10,
     20, 20,  0,  0,  0,  0, 20, 20,
     20, 30, 10,  0,  0, 10, 30, 20
};

Score get_piece_square_value(PieceType type, int x, int y, Color color) {
    int square_index = board_t::index(x, y);
    // flip square index for black pieces
    int adjusted_index = (color == Color::WHITE) ? square_index : (63 - square_index);
    
    switch (type) {
        case PieceType::PAWN: return PAWN_TABLE[adjusted_index];
        case PieceType::KNIGHT: return KNIGHT_TABLE[adjusted_index];
        case PieceType::BISHOP: return BISHOP_TABLE[adjusted_index];
        case PieceType::ROOK: return ROOK_TABLE[adjusted_index];
        case PieceType::QUEEN: return QUEEN_TABLE[adjusted_index];
        case PieceType::KING: return KING_TABLE[adjusted_index];
        default: return 0;
    }
}

Score evaluate_position(const bitboard_t& board) {
    Score score = 0;

    // for every square in the bitboard
    for (int x = 0; x<8; x++) {
        for (int y = 0; y < 8; y++){
            square_t square = board.at(x, y);
                if (square.piece.type == PieceType::EMPTY)
                    continue;

            Score piece_value = 0;
            switch (square.piece.type) {
                case PieceType::PAWN: piece_value = PAWN_VALUE; break;
                case PieceType::KNIGHT: piece_value = KNIGHT_VALUE; break;
                case PieceType::BISHOP: piece_value = BISHOP_VALUE; break;
                case PieceType::ROOK: piece_value = ROOK_VALUE; break;
                case PieceType::QUEEN: piece_value = QUEEN_VALUE; break;
                case PieceType::KING: piece_value = KING_VALUE; break;
                default: break;
            }

            piece_value += get_piece_square_value(
                square.piece.type,
                square.x,
                square.y,
                square.piece.color
            );

            score += (square.piece.color == Color::WHITE) ? piece_value : -piece_value;
        }
    }

    return score;
}

} // namespace eval

#endif