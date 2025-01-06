#ifndef square_t_hpp
#define square_t_hpp

#include "move_t.hpp"
#include "piece_t.hpp"

struct square_t {
    int x;
    int y;
    bool has_moved; // Currently not handled by the bitboard
    piece_t piece;
};

#endif