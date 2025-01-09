#ifndef engine_hpp
#define engine_hpp

#include <string.h>

#include "move_t.hpp"
#include "moves.hpp"
#include "eval.hpp"
#include "piece_t.hpp"
#include "move_eval.hpp"

namespace eval{
vector<move_t> ordered_moves(board_t board, Color color, int depth){
  vector<move_t> possible_moves =  moves::generate_all_moves_for_color(board, color);
      std::sort(possible_moves.begin(), possible_moves.end(), [&board, depth](const move_t& move, const move_t& other) {
        return better(board, move, other, depth);
    });
}


int negamax(board_t &board, int depth, int alpha, int beta, Color color) {
  // Check for checkmate
  if (depth == 0) {
    return eval::evaluate_position(board);
  }

  vector<move_t> possible_moves = ordered_moves(board, color, depth);
  
  if (color == Color::WHITE) {
    int max_score = -(int)INFINITY;

    for (const move_t& move : possible_moves) {
      // Add move to board
      piece_t cap_piece = moves::make_move(board, move);

      // Calculate score of this branch (maximize for white)
      int score = negamax(board, depth-1, alpha, beta, Color::BLACK);
      max_score = max(max_score,score);
      alpha = max(alpha, score);

      // Undo move from board
      moves::undo_move(board, move, cap_piece);

      // Prune branch
      if (beta <= alpha) {
        if (not move_eval::is_capture(board, move)){  // we found a killer move
          // if the killer move was not in our array for the current depth, we store it
          if(move != move_eval::killer_moves[depth][0] && move != move_eval::killer_moves[depth][1]){  
            move_eval::killer_moves[depth] = {move_eval::killer_moves[depth][1], move};
          }
        }
        break;
      }
    }
    return max_score;
  } 
  
  else {
    int min_score = (int)INFINITY;

    for (const move_t& move : possible_moves) {
      // Add move to board
      piece_t cap_piece = moves::make_move(board, move);

      // Calculate score of this branch (minimize for black)
      int score = negamax(board, depth-1, alpha, beta, Color::WHITE);
      min_score = min(min_score,score);
      alpha = min(alpha, score);

      // Undo move from board
      moves::undo_move(board, move, cap_piece);

      // Prune branch
      if (beta <= alpha) {
        if (not move_eval::is_capture(board, move)){  // we found a killer move
          // if the killer move was not in our array for the current depth, we store it
          if(move != move_eval::killer_moves[depth][0] && move != move_eval::killer_moves[depth][1]){  
            move_eval::killer_moves[depth] = {move_eval::killer_moves[depth][1], move};
          }
        }
        break;
      }
    }

    return min_score;
  }
}


move_t get_best_move(board_t &board, int depth, Color color) {

  vector<move_t> possible_moves = ordered_moves(board, color, depth);
  for(move_t move: possible_moves){
    //cout << "Moving from: (" << move.from_x << "," << move.from_y << ") to: (" << move.to_x << "," << move.to_y << ")" << endl;
  }

  // check that there are possible moves (aka not checkmate/stalemate)

  move_t best_move = possible_moves[0];

  if (color == Color::WHITE) { // Playing as white
    int best_score = -(int)INFINITY;

    for (const move_t& move : possible_moves) {

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
  else { // Playing as black
    int best_score = (int)INFINITY;

    for (const move_t& move : possible_moves) {
      // Add move to board
      piece_t cap_piece = moves::make_move(board, move);

      int score = negamax(board, depth - 1, -(int)INFINITY, (int)INFINITY, !color);

      if (score < best_score) {
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