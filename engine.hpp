#ifndef engine_hpp
#define engine_hpp

#include <string.h>

#include "move_t.hpp"
#include "moves.hpp"
#include "eval.hpp"


int negamax(board_t &board, int depth, int alpha, int beta, bool player) {

  // Check for checkmate
  if (depth == 0) {
    return eval::evaluate_position(board);
  }

  vector<move_t> possible_moves = moves::get_all_moves(board);
  
  if (player) {
    int max_score = -2147483647;

    for (move_t move : possible_moves) {
      // Add move to board

      int score = negamax(board, depth-1, alpha, beta, false);
      max_score = max(max_score,score);
      alpha = max(alpha, score);

      // Undo move from board

      if (beta <= alpha) {
        break;
      }
    }

    return max_score;
  } 
  else {
    int min_score = 2147483647;

    for (move_t move : possible_moves) {
      // Add move to board

      int score = negamax(board, depth-1, alpha, beta, true);
      min_score = min(min_score,score);
      alpha = min(alpha, score);

      // Undo move from board

      if (beta <= alpha) {
        break;
      }
    }

    return min_score;
  }

}


move_t get_best_move(board_t &board, int depth, int alpha, int beta, bool player) {

  vector<move_t> possible_moves = moves::get_all_moves(board);

  // check that there are possible moves (aka not checkmate/stalemate)

  move_t best_move = possible_moves[0];

  if (player) {
    int best_score = -2147483647;

    for (move_t move : possible_moves) {
      // Add move to board

      int score = negamax(board, depth, alpha, beta, !player);

      if (score > best_score) {
        best_score = score;
        best_move = move;
      }

      // Undo move
    }
  }
  else {
    int best_score = 2147483647;

    for (move_t move : possible_moves) {
      // Add move to board

      int score = negamax(board, depth, alpha, beta, !player);

      if (score > best_score) {
        best_score = score;
        best_move = move;
      }

      // Undo move
    }
  }

  return best_move;

}


#endif