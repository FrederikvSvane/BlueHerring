#ifndef engine_hpp
#define engine_hpp

#include <string.h>

#include "move_t.hpp"
#include "moves.hpp"
#include "eval.hpp"
#include "piece_t.hpp"

namespace eval{
int negamax(bitboard_t &board, int depth, int alpha, int beta, Color color) {

  // Check for checkmate
  if (depth == 0) {
    return eval::evaluate_position(board);
  }

  vector<bitboard_move_t> possible_moves = bitboard_moves::generate_all_moves_for_color(board, color);
  
  if (color == Color::WHITE) {
    int max_score = -2147483647;

    for (bitboard_move_t move : possible_moves) {
      // Add move to board
      piece_t cap_piece = bitboard_moves::make_move(board, move);

      // Calculate score of this branch (maximize for white)
      int score = negamax(board, depth-1, alpha, beta, Color::BLACK);
      max_score = max(max_score,score);
      alpha = max(alpha, score);

      // Undo move from board
      bitboard_moves::undo_move(board, move, cap_piece);

      // Remove branch if it's irrelevant
      if (beta <= alpha) {
        break;
      }
    }

    return max_score;
  } 
  else {
    int min_score = 2147483647;

    for (bitboard_move_t move : possible_moves) {
      // Add move to board
      piece_t cap_piece = bitboard_moves::make_move(board, move);

      // Calculate score of this branch (minimize for black)
      int score = negamax(board, depth-1, alpha, beta, Color::WHITE);
      min_score = min(min_score,score);
      alpha = min(alpha, score);

      // Undo move from board
      bitboard_moves::undo_move(board, move, cap_piece);

      // Remove branch if it's irrelevant
      if (beta <= alpha) {
        break;
      }
    }

    return min_score;
  }

}


bitboard_move_t get_best_move(bitboard_t &board, int depth, Color color) {

  vector<bitboard_move_t> possible_moves = bitboard_moves::generate_all_moves_for_color(board, color);

  // check that there are possible coordinate_moves (aka not checkmate/stalemate)

  bitboard_move_t best_move = possible_moves[0];

  if (color == Color::WHITE) { // Playing as white
    int best_score = -2147483647;

    for (bitboard_move_t move : possible_moves) {

      // Add move to board
      piece_t cap_piece = bitboard_moves::make_move(board, move);

      int score = negamax(board, depth - 1, -2147483647, 2147483647, !color);

      if (score > best_score) {
        best_score = score;
        best_move = move;
      }

      // Undo move
      bitboard_moves::undo_move(board, move, cap_piece);
    }
  }
  else { // Playing as black
    int best_score = 2147483647;

    for (bitboard_move_t move : possible_moves) {
      // Add move to board
      piece_t cap_piece = bitboard_moves::make_move(board, move);

      int score = negamax(board, depth - 1, -2147483647, 2147483647, !color);

      if (score < best_score) {
        best_score = score;
        best_move = move;
      }

      // Undo move
      bitboard_moves::undo_move(board, move, cap_piece);
    }
  }

  return best_move;

}
}

#endif