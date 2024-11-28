// #include <iostream>
// #include "board_t.hpp"
// #include "piece_t.hpp"

// struct piece_t {
//     bool color;
// };
// struct pawn_t : piece_t;
// struct rook_t : piece_t;
// struct knight_t : piece_t;

// square_t::square_t(int x, int y) {
//     this->x = x;
//     this->y = y;

//     if (y == 0 || y == 1) {
//         // white piece
//         if (y == 1) {
//             // white pawn
//             this->piece = pawn_t(1);
//             return;
//         }
//         else if (x == 0 || x == 7) {
//             // white rook
//             this->piece = rook_t(1);
//             return;
//         }
//         else if (x == 1 || x == 6) {
//             // white knight
//             this->piece = knight_t(1);
//             return;
//         }
//         else if (x == 2 || x == 5) {
//             // white bishop
//             this->piece = bishop_t(1);
//             return;
//         }
//         else if (x == 3) {
//             // white queen
//             this->piece = queen_t(1);
//             return;
//         }
//         else {
//             // white king
//             this->piece = king_t(1);
//             return;
//         }
//     }
//     else if (y == 6 || y == 7) {
//         // black piece
//         if (y == 6) {
//             // black pawn
//             this->piece = pawn_t(0);
//             return;
//         }
//         else if (x == 0 || x == 7) {
//             // black rook
//             this->piece = rook_t(0);
//             return;
//         }
//         else if (x == 1 || x == 6) {
//             // black knight
//             this->piece = knight_t(0);
//             return;
//         }
//         else if (x == 2 || x == 5) {
//             // black bishop
//             this->piece = bishop_t(0);
//             return;
//         }
//         else if (x == 3) {
//             // black queen
//             this->piece = queen_t(0);
//             return;
//         }
//         else {
//             // black king
//             this->piece = king_t(0);
//             return;
//         }
//     }
// };

// board_t::board_t() {
//     for (int i = 0; i < 8; i++) {
//         for (int j = 0; j < 8; j++) {
//             this->board[i][j] = square_t(i, j);
//         }
//     }
// };