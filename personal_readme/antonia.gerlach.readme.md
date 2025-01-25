# README file for Antonia Gerlach
# Week 1
- (As a team) Came up with the different structures needed to create a working chess board
- Defined piece_t structure
- Implemented the method get_moves() for some pieces of the piece_t structure

# Week 2
- Implemented eval() (previously called get_moves) of all piece types, now returning an array of move_t rather than coordinates
- Implemented eval() of square_t
- Tested eval() of pieces

# Week 3
- Implemented and tested castling moves
- Began implementation of our "best possible move" algorithm.

# Week 4
- I was sick

# Week 5
- Implemented a evaluate_move function to compute the 'score' of a move, which is a good indicator of how good a move is
- Using this scoring method, we sort the moves in our get_best_move algorithm to maximize pruning
- Tested efficiency of this: apparently, sorting the moves might increase pruning but takes more time than the algorithm itself :(

# Week 6
- Further tested sorting moves for higher efficiency, failed
- Updated engine.hpp and eval.hpp to work with bitboards

# Week 7
- Updated eval.hpp so as to reward checkmates and punish stalemates

# Week 8
- Optimizing implementation of negamax with checkmate and stalemate
- Addressed dynamic deepening techniques (now we are returning best move for highest possible depth)