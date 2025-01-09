#ifndef move_eval_hpp
#define move_eval_hpp

#include <string.h>

#include "move_t.hpp"
#include "moves.hpp"
#include "eval.hpp"
#include "piece_t.hpp"

namespace move_eval{
    // NOTE: For now, I put 7 as maximum depth of search, but we could change it later
    std::array<std::array<move_t, 2>, 7> killer_moves; // Array to store moves two 'killer moves' per depth. Reading suggests keeping only 2 is best.
    
    int piece_value(piece_t piece){
        int piece_value = 0;
        switch (piece.type) {
            case PieceType::PAWN: piece_value = 1; break;
            case PieceType::KNIGHT: piece_value = 3; break;
            case PieceType::BISHOP: piece_value = 3; break;
            case PieceType::ROOK: piece_value = 5; break;
            case PieceType::QUEEN: piece_value = 9; break;
            case PieceType::KING: piece_value = 1000; break;
            default: break;
        }
        return piece_value;
    }

    bool is_promotion(move_t move){
        return move.promotion_type != PieceType::EMPTY;
    }

    bool is_capture(board_t board, move_t move){
        piece_t cap_piece = moves::make_move(board, move);
        moves::undo_move(board, move, cap_piece);
        return cap_piece.type != PieceType::EMPTY;
    }

    bool is_check(board_t board, move_t move){
        // FILL IN
    }

    bool principal_variation(move_t move){
        // FILL IN
    }

    bool is_killer_move(move_t move, int depth){
        return (move == killer_moves[depth][0]) || (move == killer_moves[depth][1]);
    }

    bool in_history_heuristic(move_t move){
        //FILL IN
    }

    int history_score(move_t move){
        // FILL IN
    }

    int move_eval(board_t board, move_t move, int depth){
        // The move that was best in the previous search
        if (principal_variation(move))
            return 1000;
        // Moves that are known to prune a lot, based on our pruning history
        if (is_killer_move(move, depth))
            return 900; 
        if (is_promotion(move))
            return 800 + piece_value(move.promotion_type); 
        // Captures are better moves, with added bonus depending on tradeoff of the piece (MVV-LVA)
        if (is_capture(board, move))
            return 700 + piece_value(board.at(move.to_x, move.to_y).piece) - piece_value(board.at(move.from_x, move.from_y).piece); 
        if (is_check(board, move))
            return 600;
        if (in_history_heuristic(move)) //should we implement it? feels a bit extra
            return 500 + history_score(move);
        return 0;
    }
//returns all possible moves for the given state ordered form likeliest best move to least likely best move

// capturing, checks, promotion moves are prioritized
// captures with better value are prioritized, i.e. maximize value_of_victim - value_of_attacker
// maybe in the future keep track of moves that pruned a lot of branches. but for this we would need to make a
// table that stores the score of each move

// so we associate a score to each move? by default we set it to the inherent value of the move (depending on 
// whether it's a capture, check, promotion, or none) and then we can increment this score based on our pruning
// history, and also keep track of the best move at depth 2 from our previous search, and place this one as the first
// one in our sorted moves

}

#endif