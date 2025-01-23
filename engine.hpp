#ifndef engine_hpp
#define engine_hpp

#include <string.h>

#include "eval.hpp"
#include "hash.hpp"
#include "move_t.hpp"
#include "moves.hpp"
#include "piece_t.hpp"
#include <chrono>

extern chrono::high_resolution_clock::time_point t0;
extern chrono::high_resolution_clock::time_point t;
extern long duration;
const long time_limit = 9500; // In milliseconds, ie 3000ms = 3s

extern bitboard_move_t unique_best_move;

namespace engine {
constexpr int NEG_INFINITY = -2147483647;
constexpr int POS_INFINITY = 2147483647;

struct SearchResult {
    int score;
    int nodes;
};

SearchResult negamax(bitboard_t& board, int depth, int alpha, int beta, Color color) {
    if (depth == 0) {
        return {eval::evaluate_position(board), 1};
    }

    move_list_t possible_moves = moves::generate_all_moves_for_color(board, color);
    int nodes      = 1;
    int best_score = (color == Color::WHITE) ? NEG_INFINITY : POS_INFINITY;

    // Testing the time
    t        = chrono::high_resolution_clock::now();
    duration = chrono::duration_cast<chrono::milliseconds>(t - t0).count();
    if (duration > time_limit) {
        return {best_score, nodes};
    }

    for (int i = 0; i < possible_moves.count; i++) {
        piece_t cap_piece   = moves::make_move(board, possible_moves.moves[i]);
        SearchResult result = negamax(board, depth - 1, alpha, beta, !color);
        nodes += result.nodes;

        if (color == Color::WHITE) {
            best_score = max(best_score, result.score);
            alpha      = max(alpha, result.score);
        } else {
            best_score = min(best_score, result.score);
            beta       = min(beta, result.score);
        }

        moves::undo_move(board, possible_moves.moves[i], cap_piece);

        if (alpha >= beta) {
            break; // Beta cutoff
        }
    }

    return {best_score, nodes};
}

pair<bitboard_move_t, SearchResult> get_best_move(bitboard_t& board, int depth, Color color) {
    move_list_t possible_moves = moves::generate_all_moves_for_color(board, color);

    if (possible_moves.count == 0) {
        throw runtime_error("No legal moves available");
    }

    bitboard_move_t best_move = possible_moves.moves[0];
    SearchResult best_result  = {(color == Color::WHITE) ? NEG_INFINITY : POS_INFINITY, 0};

    for (int i = 0; i < possible_moves.count; i++) {

        // Testing the time
        t        = chrono::high_resolution_clock::now();
        duration = chrono::duration_cast<chrono::milliseconds>(t - t0).count();
        if (duration > time_limit) {
            return {best_move, best_result};
        }

        piece_t cap_piece   = moves::make_move(board, possible_moves.moves[i]);
        SearchResult result = negamax(board, depth - 1, NEG_INFINITY, POS_INFINITY, !color);

        if ((color == Color::WHITE && result.score > best_result.score) ||
            (color == Color::BLACK && result.score < best_result.score)) {
            best_result = result;
            best_move   = possible_moves.moves[i];
        }

        moves::undo_move(board, possible_moves.moves[i], cap_piece);
        best_result.nodes += result.nodes;
    }

    unique_best_move = best_move;
    return {best_move, best_result};
}

// ---- FOR TESTING ----

SearchResult negamax_without_pruning(bitboard_t& board, int depth, Color color) {
    if (depth == 0) {
        return {color == Color::WHITE ? eval::evaluate_position(board)
                                      : -eval::evaluate_position(board),
                1};
    }

    move_list_t possible_moves = moves::generate_all_moves_for_color(board, color);
    int nodes                  = 1;
    int max_score              = NEG_INFINITY;

    for (int i = 0; i < possible_moves.count; i++) {
        piece_t cap_piece   = moves::make_move(board, possible_moves.moves[i]);
        SearchResult result = negamax_without_pruning(board, depth - 1, !color);
        nodes += result.nodes;
        int score = -result.score;
        max_score = max(max_score, score);
        moves::undo_move(board, possible_moves.moves[i], cap_piece);
    }

    return {max_score, nodes};
}

// ---------------------

} // namespace engine

#endif