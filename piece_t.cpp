// #include <iostream>
// #include <vector>
// using namespace std;
// #include "piece_t.hpp"

// struct pawn_t:piece_t{
//     vector<int[2]> get_moves(int x, int y){
//         vector<int[2]> moves;
//         int direction = this->color ? 1 : -1; //if the piece is white it moves up, otherwise it moves down
//         if(y == 1 && direction == 1){ //if it's the first move for white:
//             moves.push_back({x, 2});
//             moves.push_back({x, 3});
//             return moves;
//         }

//         else if(y == 6 && direction == -1){ //if it's the first move for black:
//             moves.push_back({x, 6});
//             moves.push_back({x, 5});
//             return moves;
//         }

//         else{
//             moves.push_back({x, y + direction});
//         };
//     };
// };

// struct knight_t:piece_t{
// };

// struct rook_t:piece_t{
//     vector<int[2]> get_moves(int x, int y){
//         vector<int[2]> moves;
//         for (int xi = 0; xi < 8; xi++){  // move horizontally
//             if(xi != x){
//                 moves.push_back({xi, y});
//             }
//         }
//         for (int yi = 0; yi < 8; yi++){  // move vertically
//             if(yi != y){
//                 moves.push_back({x, yi});
//             }
//         }
//         return moves;
//     }
// };

// struct bishop_t:piece_t{
//     vector<int[2]> get_moves(int x, int y){
//         vector<int[2]> moves;
//         //FILL IN
//     }
// };

// struct queen_t:piece_t{
// };

// struct king_t:piece_t{
// };