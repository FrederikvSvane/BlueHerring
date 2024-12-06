#ifndef eval_hpp
#define eval_hpp

#include "board_t.hpp"

// creating a namespace, so we can type eval::evaluate_position() 
namespace eval {

using Score = int; // this is just for code readability. So we can see any time we work with a score, explicitly

// setting the values of the pieces (according to community consensus)
constexpr Score PAWN_VALUE   = 100;
constexpr Score KNIGHT_VALUE = 300;
constexpr Score BISHOP_VALUE = 300;
constexpr Score ROOK_VALUE   = 500;
constexpr Score QUEEN_VALUE  = 900;

Score evaluate_material(const board_t& board) {
    Score score = 0;

    for (const auto& square : board) {
        if (square.piece.type == PieceType::EMPTY)
            continue;

        Score piece_value = 0;
        switch (square.piece.type) {
        case PieceType::PAWN: piece_value = PAWN_VALUE; break;
        case PieceType::KNIGHT: piece_value = KNIGHT_VALUE; break;
        case PieceType::BISHOP: piece_value = BISHOP_VALUE; break;
        case PieceType::ROOK: piece_value = ROOK_VALUE; break;
        case PieceType::QUEEN: piece_value = QUEEN_VALUE; break;
        default: break;
        }

        // white is maximizing, black is minimizing
        score += (square.piece.color == Color::WHITE) ? piece_value : -piece_value;
    }

    return score;
}

Score evaluate_position(const board_t& board) {
    Score score = evaluate_material(board);

    // TODO: The board should be evaluated in a much more intelligent way. Look at https://www.chessprogramming.org/Evaluation

    return score;
}

} // namespace eval

#endif