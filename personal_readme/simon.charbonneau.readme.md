# Simon Charbonneau

## Week 1
- Created the project directory did a bit of the logistics of Readme files, branches, etc.
- Created the square_t struct and its constructor
- Created the board_t struct and its constructor, using the square_t struct

## Week 2
- Worked on the line() function to compute the line of squares along the line of non-knight moves
- Implemented functions to check whether a king is in check and whether a move given at the piece level is legal at the board level
- Implemented en-passant, both detecting it from the input file and generating it during move generation

## Week 3
- Debugging a looooot the move generation together with Frederik, and got a slight depression from doing that

## Week 4
- Implementing random move choice, and commiting v1 of the engine, an engine playing legally but randomly
- Fixing our file_util, fixing bugs here and there, and cleaning the remote repository
- Commiting v2 of the engine, an engine search for the best move at depth 3

## Week 5
- Implemented the book of opening lines, i.e. the file utility and the string-manipulation

## Week 6
- Got working on implementing quiescence search for our evaluation