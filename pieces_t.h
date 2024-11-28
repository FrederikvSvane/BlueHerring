#include <iostream>
#include <vector>
using namespace std;

struct square_t;
struct board_t;


struct piece_t{
    //FIELDS
    bool color;                                     //0 stands for black, 1 stands for white
    //INITIALIZER
    piece_t(bool color): color{color}{};
    //METHODS
    virtual vector<int[2]> get_moves(int x, int y); //returns a list of possible moves from their current square. See antonia readme for specifics
};

struct pawn_t:piece_t{};

struct knight_t:piece_t{};

struct rook_t:piece_t{};

struct bishop_t:piece_t{};

struct queen_t:piece_t{};

struct king_t:piece_t{};

