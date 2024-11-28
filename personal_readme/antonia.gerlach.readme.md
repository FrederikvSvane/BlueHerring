# README file for Antonia Gerlach
piece_t structure:
Used to define a piece, and is initalized using only color. 
It contains a virtual method 'get_moves', which will output all possible moves for that specific piece given its current position, ignoring the constraints imposed by other pieces on the board.

individual pieces structure:
Each type of piece is a structure that inherits from piece_t. It includes 