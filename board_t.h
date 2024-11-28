#include <iostream>
#include <array>

struct piece_t;

struct square_t {
    int x;
    int y;

    piece_t* piece;

    square_t(int x, int y);
};

struct board_t {
    std::array<std::array<square_t, 8>, 8> board;

    board_t();
};