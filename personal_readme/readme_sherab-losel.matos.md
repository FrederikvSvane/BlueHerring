### Sherab Losel Matos

# Week 1
- Argument Parsing w/ Frederik

# Week 2
- Worked on line() function to determine legal moves for non-knight pieces w/ Simon

# Week 3
- Created engine.hpp
- Get best move from eval with negamax and alpha beta pruning

# Week 4
- Debugging negamax (Rook going missing). Found the problem, now working till depth 4 in reasonable time.

# Christmas Holidays
- Initial bitboard implementation
- Bitboards for each of the pieces
- making moves, capturing, promotion with bitboards
- determining legal moves for:
  * knights
  * rooks
  * bishops
  * queen
  * pawns

# Week 5
- Checking whether a square is under attack using the bitboard mplementation
- Getting legal moves for the king
- debugging legal moves for pawns - issue with bitshifting negative numbers for ULL

# Week 6
- Bugfix wrapping pawn attacks on is_square_under_attack()
- Bugfix generate_all_moves_for_color(). Bitboard now gets the correct number of moves!