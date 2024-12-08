#ifndef square_t_hpp
#define square_t_hpp

#include "piece_t.hpp"
#include "move_t.hpp"

struct square_t {
    int x;
    int y;
    piece_t piece; // color: none, piecetype: empty
    vector<move_t> eval(){
        if(piece.type == PieceType::EMPTY){
            cout << "This square has no piece!" << endl;
            vector<move_t> moves;
            return moves; //we return an empty array of moves
        }
        else{
            return piece.eval(x, y);
        }
    }
};

#endif