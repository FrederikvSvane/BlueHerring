#ifndef square_t_hpp
#define square_t_hpp

#include "piece_t.hpp"

struct square_t {
    int x;
    int y;
    piece_t piece; // color: none, piecetype: empty
};

#endif