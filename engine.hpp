#ifndef engine_hpp
#define engine_hpp

#include <string.h>

#include "eval.hpp"
#include "move_t.hpp"
#include "moves.hpp"
#include "piece_t.hpp"

namespace engine {
constexpr int NEG_INFINITY = -2147483647;
constexpr int POS_INFINITY = 2147483647;

int negamax(bitboard_t& board, int depth, int alpha, int beta, Color color) {
    // Base case
    if (depth == 0) {
        return eval::evaluate_position(board);
    }

    move_list_t possible_moves = moves::generate_all_moves_for_color(board, color);

    if (color == Color::WHITE) {
        int max_score = NEG_INFINITY;

        for (int i = 0; i < possible_moves.count; i++) {
            // Add move to board
            piece_t cap_piece = moves::make_move(board, possible_moves.moves[i]);

            // Calculate score of this branch (maximize for white)
            int score = negamax(board, depth - 1, alpha, beta, Color::BLACK);
            max_score = max(max_score, score);
            alpha     = max(alpha, score);

            // Undo move from board
            moves::undo_move(board, possible_moves.moves[i], cap_piece);

            // Remove branch if it's irrelevant
            if (beta <= alpha) {
                break;
            }
        }

        return max_score;
    } else {
        int min_score = POS_INFINITY;

        for (int i = 0; i < possible_moves.count; i++) {
            // Add move to board
            piece_t cap_piece = moves::make_move(board, possible_moves.moves[i]);

            // Calculate score of this branch (minimize for black)
            int score = negamax(board, depth - 1, alpha, beta, Color::WHITE);
            min_score = min(min_score, score);
            alpha     = min(alpha, score);

            // Undo move from board
            moves::undo_move(board, possible_moves.moves[i], cap_piece);

            // Remove branch if it's irrelevant
            if (beta <= alpha) {
                break;
            }
        }

        return min_score;
    }
}

bitboard_move_t get_best_move(bitboard_t& board, int depth, Color color) {
    move_list_t possible_moves = moves::generate_all_moves_for_color(board, color);

    // Check for no legal moves
    if (possible_moves.count == 0) {
        throw std::runtime_error("No legal moves available");
    }

    bitboard_move_t best_move = possible_moves.moves[0];

    if (color == Color::WHITE) { // Playing as white
        int best_score = NEG_INFINITY;

        for (int i = 0; i < possible_moves.count; i++) {
            // Add move to board
            piece_t cap_piece = moves::make_move(board, possible_moves.moves[i]);

            int score = negamax(board, depth - 1, NEG_INFINITY, POS_INFINITY, !color);

            if (score > best_score) {
                best_score = score;
                best_move  = possible_moves.moves[i];
            }

            // Undo move
            moves::undo_move(board, possible_moves.moves[i], cap_piece);
        }
    } else { // Playing as black
        int best_score = POS_INFINITY;

        for (int i = 0; i < possible_moves.count; i++) {
            // Add move to board
            piece_t cap_piece = moves::make_move(board, possible_moves.moves[i]);

            int score = negamax(board, depth - 1, NEG_INFINITY, POS_INFINITY, !color);

            if (score < best_score) {
                best_score = score;
                best_move  = possible_moves.moves[i];
            }

            // Undo move
            moves::undo_move(board, possible_moves.moves[i], cap_piece);
        }
    }

    return best_move;
}

} // namespace engine

#endif