#ifndef engine_hpp
#define engine_hpp

#include <string.h>

#include "move_t.hpp"
#include "moves.hpp"
#include "eval.hpp"
#include "piece_t.hpp"

namespace eval{
int negamax(board_t &board, int depth, int alpha, int beta, Color color) {

  // Check for checkmate
  if (depth == 0) {
    return eval::evaluate_position(board);
  }

  vector<move_t> possible_moves = moves::get_all_moves(board);
  
  if (color == Color::WHITE) {
    int max_score = -(int)INFINITY;

    for (move_t move : possible_moves) {
      if(board.at(move.from_x, move.from_y).piece.type == PieceType::ROOK){
        cout << "Moving from: (" << move.from_x << "," << move.from_y << ") to: (" << move.to_x << "," << move.to_y << ")" << endl;
        board.pretty_print_board();
      }
      // Add move to board
      piece_t cap_piece = moves::make_move(board, move);

      if(board.at(move.to_x, move.to_y).piece.type == PieceType::ROOK){
        board.pretty_print_board();
      } 

      // Calculate score of this branch
      int score = negamax(board, depth-1, alpha, beta, Color::BLACK);
      max_score = max(max_score,score);
      alpha = max(alpha, score);

      // Undo move from board
      moves::undo_move(board, move, cap_piece);

      // Remove branch if it's irrelevant
      if (beta <= alpha) {
        break;
      }
    }

    return max_score;
  } 
  else {
    int min_score = (int)INFINITY;

    for (move_t move : possible_moves) {
      // Add move to board
      piece_t cap_piece = moves::make_move(board, move);

      // 
      int score = negamax(board, depth-1, alpha, beta, Color::WHITE);
      min_score = min(min_score,score);
      alpha = min(alpha, score);

      // Undo move from board
      moves::undo_move(board, move, cap_piece);

      if (beta <= alpha) {
        break;
      }
    }

    return min_score;
  }

}


move_t get_best_move(board_t &board, int depth, Color color) {

  vector<move_t> possible_moves = moves::generate_all_moves_for_color(board, color);
  for(move_t move: possible_moves){
    //cout << "Moving from: (" << move.from_x << "," << move.from_y << ") to: (" << move.to_x << "," << move.to_y << ")" << endl;
  }

  // check that there are possible moves (aka not checkmate/stalemate)

  move_t best_move = possible_moves[0];

  if (color == Color::WHITE) {
    int best_score = -(int)INFINITY;

    for (move_t move : possible_moves) {
      // Add move to board
      piece_t cap_piece = moves::make_move(board, move);

      int score = negamax(board, depth - 1, -(int)INFINITY, (int)INFINITY, !color);

      if (score > best_score) {
        best_score = score;
        best_move = move;
      }

      // Undo move
      moves::undo_move(board, move, cap_piece);
    }
  }
  else {
    int best_score = (int)INFINITY;

    for (move_t move : possible_moves) {
      // Add move to board
      piece_t cap_piece = moves::make_move(board, move);

      int score = negamax(board, depth - 1, -(int)INFINITY, (int)INFINITY, !color);

      if (score > best_score) {
        best_score = score;
        best_move = move;
      }

      // Undo move
      moves::undo_move(board, move, cap_piece);
    }
  }

  return best_move;

}
}

#endif