// move_logic.hpp
#ifndef move_logic_hpp
#define move_logic_hpp

#include "board_t.hpp"
#include "move_t.hpp"
#include <array>
#include <cmath>    //for absolute value
#include <stdint.h> //had to include this, otherwise didn't compile on my pc
#include <vector>

namespace bit_moves {
    piece_t make_move(bitboard_t& board, const move_t& move) { // In the future, a move_t could simply be two bits, or a bitboard with the from/to bits set to 1
        // Determine which piece is being moved
        piece_t moving_piece = board.at(move.from_x, move.from_y).piece;
        
        U64 to_bitmask = board.single_bitmask(move.to_y * 8 + move.to_x);

        if (moving_piece.type == PieceType::EMPTY || moving_piece.color == Color::NONE) {
            throw std::invalid_argument("No piece to move at the source square.");
        }

        // Determine the relevant bitboard for the piece being moved
        U64* piece_board = nullptr;
        piece_board = board.get_board_for_piece(moving_piece.type, moving_piece.color);

        if (!piece_board) {
            throw std::runtime_error("Failed to determine the bitboard for the moving piece.");
        }


        // TODO: Save current state before making the move

        // TODO: Update castling rights if king moves

        // TODO: Update castling rights based on rook activity (check if we move or capture a rook)

        // TODO: Check to_square

        // Save captured piece

        // TODO: Handle special cases

        // TODO: Handle en passant capture

        // TODO: Handle castling rook movement

        // TODO: Reset en passant target square

        // TODO: Set en passant target square if pawn double move

        // Make the actual move
        board.move_bit(piece_board, move.from_y * 8 + move.from_x, move.to_y * 8 + move.to_x);

        // Handle captures by clearing the destination square in all opponent bitboards
        piece_t captured_piece = {PieceType::EMPTY, Color::NONE}; // Track the captured piece
        U64* opponent_boards[] = {
            &board.board_b_P, &board.board_b_N, &board.board_b_B,
            &board.board_b_R, &board.board_b_Q, &board.board_b_K
        };

        if (moving_piece.color == Color::BLACK) {
            opponent_boards[0] = &board.board_w_P;
            opponent_boards[1] = &board.board_w_N;
            opponent_boards[2] = &board.board_w_B;
            opponent_boards[3] = &board.board_w_R;
            opponent_boards[4] = &board.board_w_Q;
            opponent_boards[5] = &board.board_w_K;
        }

        for (int i = 0; i < 6; ++i) {
            if (*opponent_boards[i] & to_bitmask) {
                *opponent_boards[i] &= ~to_bitmask; // Clear the captured piece
                captured_piece.type = static_cast<PieceType>(i + 1);
                captured_piece.color = !moving_piece.color;
                break;
            }
        }

        // Handle promotion if applicable
        if (moving_piece.type == PieceType::PAWN && (move.to_y == 0 || move.to_y == 7)) {
            if (move.promotion_type == PieceType::EMPTY) {
                throw std::invalid_argument("Promotion type not specified for pawn promotion.");
            }

            // Remove the pawn from its bitboard
            *piece_board &= ~to_bitmask;

            // Add the promoted piece to the appropriate bitboard
            if (moving_piece.color == Color::WHITE) {
                if (move.promotion_type == PieceType::QUEEN) board.board_w_Q |= to_bitmask;
                else if (move.promotion_type == PieceType::ROOK) board.board_w_R |= to_bitmask;
                else if (move.promotion_type == PieceType::BISHOP) board.board_w_B |= to_bitmask;
                else if (move.promotion_type == PieceType::KNIGHT) board.board_w_N |= to_bitmask;
            } else {
                if (move.promotion_type == PieceType::QUEEN) board.board_b_Q |= to_bitmask;
                else if (move.promotion_type == PieceType::ROOK) board.board_b_R |= to_bitmask;
                else if (move.promotion_type == PieceType::BISHOP) board.board_b_B |= to_bitmask;
                else if (move.promotion_type == PieceType::KNIGHT) board.board_b_N |= to_bitmask;
            }
        }

        board.history.push_back(move);
        return captured_piece;
    }

    // TODO: Test this
    void undo_move(bitboard_t& board, const move_t& move, const piece_t& captured_piece) {
        piece_t piece = board.at(move.to_x, move.to_y).piece;

        // Remove the move from move history (NOT the same as state history!)
        board.history.pop_back();

        if (board.state_history.empty()) {
            throw std::invalid_argument("Trying to pop board state history, when history is empty.");
        }

            // Restore previous state
        board_state previous_state = board.state_history.back();
        board.state_history.pop_back();

        board.white_king_side_castle  = previous_state.white_king_side_castle;
        board.white_queen_side_castle = previous_state.white_queen_side_castle;
        board.black_king_side_castle  = previous_state.black_king_side_castle;
        board.black_queen_side_castle = previous_state.black_queen_side_castle;
        board.en_passant_x            = previous_state.en_passant_x;
        board.en_passant_y            = previous_state.en_passant_y;

        bool is_en_passant = (board.at(move.to_x, move.to_y).piece.type == PieceType::PAWN &&
                                move.from_x != move.to_x &&
                                move.to_x == board.en_passant_x &&
                                move.to_y == board.en_passant_y);
        
        bool is_castling = (board.at(move.to_x, move.to_y).piece.type == PieceType::KING &&
                        abs(move.from_x - move.to_x) == 2);
        
        // Determine the relevant bitboard for the piece being moved
        U64* piece_board = nullptr;
        piece_board = board.get_board_for_piece(piece.type, piece.color);

        if (!piece_board) {
            throw std::runtime_error("Failed to determine the bitboard for the moving piece.");
        }

        // Move the piece from destination to source
        board.move_bit(piece_board, move.to_y * 8 + move.to_x, move.from_y * 8 + move.from_x);


        // TODO: Test this
        if (is_castling) {
            if (move.to_x == 6) { // kingside
                board.move_bit(board.get_board_for_piece(PieceType::ROOK, piece.color), move.from_y*8+5, move.from_y*8+7);
            } else if (move.to_x == 2) { // queenside
                board.move_bit(board.get_board_for_piece(PieceType::ROOK, piece.color), move.from_y*8+3, move.from_y*8);
            }
        }

        // Restore captured piece
        if (is_en_passant) {
            U64* opponent_pawns = board.get_board_for_piece(PieceType::PAWN, !piece.color);
            *opponent_pawns |= board.single_bitmask(8*move.to_x+move.from_y);
        } else {
            U64* opponent_bitboard = board.get_board_for_piece(captured_piece.type, !piece.color);
            *opponent_bitboard |= board.single_bitmask(8*move.to_x+move.to_y);
        }
    }

    // TODO
    bool is_move_legal(bitboard_t& board, const move_t& move) {
        return false;
    }

    // Returns a bitmask with ones on the standard knight squares for the given position
    U64 get_knight_moves(bitboard_t& board, int x, int y) {
        int pos = y*8 + x;

        // Precomputed knight moves table (indexed by square position)
        static const U64 knight_attack_table[64] = {
            0x0000000000020400ULL, 0x0000000000050800ULL, 0x00000000000A1100ULL, 0x0000000000142200ULL,
            0x0000000000284400ULL, 0x0000000000508800ULL, 0x0000000000A01000ULL, 0x0000000000402000ULL,
            0x0000000002040004ULL, 0x0000000005080008ULL, 0x000000000A110011ULL, 0x0000000014220022ULL,
            0x0000000028440044ULL, 0x0000000050880088ULL, 0x00000000A0100010ULL, 0x0000000040200020ULL,
            0x0000000204000402ULL, 0x0000000508000805ULL, 0x0000000A1100110AULL, 0x0000001422002214ULL,
            0x0000002844004428ULL, 0x0000005088008850ULL, 0x000000A0100010A0ULL, 0x0000004020002040ULL,
            0x0000020400040200ULL, 0x0000050800080500ULL, 0x00000A1100110A00ULL, 0x0000142200221400ULL,
            0x0000284400442800ULL, 0x0000508800885000ULL, 0x0000A0100010A000ULL, 0x0000402000204000ULL,
            0x0002040004020000ULL, 0x0005080008050000ULL, 0x000A1100110A0000ULL, 0x0014220022140000ULL,
            0x0028440044280000ULL, 0x0050880088500000ULL, 0x00A0100010A00000ULL, 0x0040200020400000ULL,
            0x0204000402000000ULL, 0x0508000805000000ULL, 0x0A1100110A000000ULL, 0x1422002214000000ULL,
            0x2844004428000000ULL, 0x5088008850000000ULL, 0xA0100010A0000000ULL, 0x4020002040000000ULL,
            0x0400040200000000ULL, 0x0800080500000000ULL, 0x1100110A00000000ULL, 0x2200221400000000ULL,
            0x4400442800000000ULL, 0x8800885000000000ULL, 0x100010A000000000ULL, 0x2000204000000000ULL,
            0x0004020000000000ULL, 0x0008050000000000ULL, 0x00110A0000000000ULL, 0x0022140000000000ULL,
            0x0044280000000000ULL, 0x0088500000000000ULL, 0x0010A00000000000ULL, 0x0020400000000000ULL
        };

        // Get the move table for the given square
        U64 knight_moves = knight_attack_table[pos];

        Color knight_color = (board.board_w_N & (1ULL << pos)) != 0 ? Color::WHITE : Color::BLACK;

        // Remove all moves targeting friendly pieces
        knight_moves &= ~board.get_all_friendly_pieces(knight_color);

        return knight_moves;
    }

    U64 get_rook_moves(bitboard_t& board, int x, int y) {
        int pos = y * 8 + x;

        U64 occupied = board.get_all_pieces();

        // Friendly pieces to exclude
        Color rook_color = (board.board_w_R & (1ULL << pos)) != 0 ? Color::WHITE : Color::BLACK;
        U64 friendly_pieces = board.get_all_friendly_pieces(rook_color);

        U64 rook_moves = 0;

        // Directions: North, South, East, West
        const int directions[4] = {8, -8, 1, -1}; // File and rank shifts

        for (int dir : directions) {
            int square = pos;
            while (true) {
                square += dir;

                // Move is out of bounds
                if (square < 0 || square >= 64) break;

                // Handle edge cases for east/west wrapping
                if ((dir == 1 && square % 8 == 0) || (dir == -1 && pos % 8 == 0)) break;

                // Add square to moves
                U64 square_bit = 1ULL << square;
                rook_moves |= square_bit;

                // Stop at blockers
                if (occupied & square_bit) {
                    // Allow capture if the blocker is an opponent piece
                    if (friendly_pieces & square_bit) {
                        rook_moves &= ~square_bit; // Remove square if blocked by friendly piece
                    }
                    break; // Stop sliding in this direction
                }
            }
        }

        return rook_moves;
    }

    U64 get_bishop_moves(bitboard_t& board, int x, int y) {
        int pos = y * 8 + x;

        U64 occupied = board.get_all_pieces();

        // Friendly pieces to exclude
        Color bishop_color = (board.board_w_B & (1ULL << pos)) != 0 ? Color::WHITE : Color::BLACK;
        U64 friendly_pieces = board.get_all_friendly_pieces(bishop_color);

        // Compute bishop moves
        U64 bishop_moves = 0;

        // Directions: North-East, South-West, North-West, South-East
        const int directions[4] = {9, -9, 7, -7}; // Diagonal shifts

        for (int dir : directions) {
            int square = pos;
            while (true) {
                square += dir;

                // Move out of bounds
                if (square < 0 || square >= 64) break;

                // Handle edge cases for diagonal wrapping
                if ((dir == 9 && square % 8 == 0)   ||  // North-East wraps around
                    (dir == -9 && pos % 8 == 0)     ||  // South-West wraps around
                    (dir == 7 && square % 8 == 7)   ||  // North-West wraps around
                    (dir == -7 && pos % 8 == 7))        // South-East wraps around
                    break;

                // Add square to moves
                U64 square_bit = 1ULL << square;
                bishop_moves |= square_bit;

                // Stop at blockers
                if (occupied & square_bit) {
                    // Allow capture if the blocker is an opponent piece
                    if (friendly_pieces & square_bit) {
                        bishop_moves &= ~square_bit; // Remove square if blocked by friendly piece
                    }
                    break; // Stop sliding in this direction
                }
            }
        }

        return bishop_moves;
    }

    U64 get_queen_moves(bitboard_t& board, int x, int y) {
        return 0ULL;
    }

    U64 get_king_moves(bitboard_t& board, int x, int y) {
        return 0ULL;
    }

    U64 get_pawn_moves(bitboard_t& board, int x, int y) {
        return 0ULL;
    }

}

namespace moves {

piece_t make_move(board_t& board, const move_t& move) {
    square_t& from_square = board.at(move.from_x, move.from_y);
    square_t& to_square   = board.at(move.to_x, move.to_y);

    // Save current state before making the move
    board_state current_state = {
        board.white_king_side_castle,
        board.white_queen_side_castle,
        board.black_king_side_castle,
        board.black_queen_side_castle,
        board.en_passant_x,
        board.en_passant_y};
    board.state_history.push_back(current_state);

    // Update castling rights if king moves
    if (from_square.piece.type == PieceType::KING) {
        if (from_square.piece.color == Color::WHITE) {
            board.white_king_side_castle  = false;
            board.white_queen_side_castle = false;
        } else {
            board.black_king_side_castle  = false;
            board.black_queen_side_castle = false;
        }
    }

    // Update castling rights based on rook activity (check if we move or capture a rook)
    if (from_square.piece.type == PieceType::ROOK || to_square.piece.type == PieceType::ROOK) {
        // Check from_square
        if (from_square.piece.type == PieceType::ROOK) {
            if (from_square.x == 0) {
                if (from_square.y == 0)
                    board.white_queen_side_castle = false;
                if (from_square.y == 7)
                    board.black_queen_side_castle = false;
            }
            if (from_square.x == 7) {
                if (from_square.y == 0)
                    board.white_king_side_castle = false;
                if (from_square.y == 7)
                    board.black_king_side_castle = false;
            }
        }

        // Check to_square
        if (to_square.piece.type == PieceType::ROOK) {
            if (to_square.x == 0) {
                if (to_square.y == 0)
                    board.white_queen_side_castle = false;
                if (to_square.y == 7)
                    board.black_queen_side_castle = false;
            }
            if (to_square.x == 7) {
                if (to_square.y == 0)
                    board.white_king_side_castle = false;
                if (to_square.y == 7)
                    board.black_king_side_castle = false;
            }
        }
    }

    // Save captured piece
    piece_t captured_piece = to_square.piece;

    // Handle special cases

    bool is_pawn              = (from_square.piece.type == PieceType::PAWN);
    bool is_diagonal_move     = (to_square.x != from_square.x);
    bool is_target_empty      = (to_square.piece.type == PieceType::EMPTY);
    bool matches_en_passant_x = (to_square.x == board.en_passant_x);
    bool matches_en_passant_y = (to_square.y == board.en_passant_y);

    bool is_en_passant = (is_pawn &&
                          is_diagonal_move &&
                          is_target_empty &&
                          matches_en_passant_x &&
                          matches_en_passant_y);

    bool is_castling = (from_square.piece.type == PieceType::KING &&
                        abs(from_square.x - to_square.x) == 2);

    // Handle en passant capture
    if (is_en_passant) {
        captured_piece                         = board.at(move.to_x, move.from_y).piece;
        board.at(move.to_x, move.from_y).piece = piece_t{};
    }

    // Handle castling rook movement
    if (is_castling) {
        if (move.to_x == 6) { // kingside
            board.at(5, move.from_y).piece = board.at(7, move.from_y).piece;
            board.at(7, move.from_y).piece = piece_t{};
        } else if (move.to_x == 2) { // queenside
            board.at(3, move.from_y).piece = board.at(0, move.from_y).piece;
            board.at(0, move.from_y).piece = piece_t{};
        }
    }

    // Reset en passant target square
    board.en_passant_x = -1;
    board.en_passant_y = -1;

    // Set en passant target square if pawn double move
    if (from_square.piece.type == PieceType::PAWN &&
        abs(move.to_y - move.from_y) == 2) { // this works because the pawn can only move 2 squares from the starting position (so we dont need to check where it is)
        board.en_passant_x = move.to_x;
        board.en_passant_y = (move.from_y + move.to_y) / 2;
    }

    // Make the actual move
    if (move.promotion_type != PieceType::EMPTY) {
        to_square.piece = piece_t(move.promotion_type, from_square.piece.color); // Create new piece
    } else {
        to_square.piece = from_square.piece; // Normal move
    }
    from_square.piece = piece_t{};

    board.history.push_back(move);
    return captured_piece;
}

void undo_move(board_t& board, const move_t& move, const piece_t& captured_piece) {
    square_t& from_square = board.at(move.from_x, move.from_y);
    square_t& to_square   = board.at(move.to_x, move.to_y);

    // Remove the move from move history (NOT the same as state history!)
    board.history.pop_back();

    // Just an error detection
    if (board.state_history.empty()) {
        std::cout << "Error: Trying to pop board state history, when history is empty. This shouldnt happen" << endl;
        exit(1);
    }

    // Restore previous state
    board_state previous_state = board.state_history.back();
    board.state_history.pop_back();

    board.white_king_side_castle  = previous_state.white_king_side_castle;
    board.white_queen_side_castle = previous_state.white_queen_side_castle;
    board.black_king_side_castle  = previous_state.black_king_side_castle;
    board.black_queen_side_castle = previous_state.black_queen_side_castle;
    board.en_passant_x            = previous_state.en_passant_x;
    board.en_passant_y            = previous_state.en_passant_y;

    bool is_en_passant = (to_square.piece.type == PieceType::PAWN &&
                          move.from_x != move.to_x &&
                          move.to_x == board.en_passant_x &&
                          move.to_y == board.en_passant_y);

    bool is_castling = (to_square.piece.type == PieceType::KING &&
                        abs(move.from_x - move.to_x) == 2);

    // Move the piece back
    from_square.piece = to_square.piece;
    if (move.promotion_type != PieceType::EMPTY) {
        from_square.piece.type = PieceType::PAWN;
    }
    to_square.piece = piece_t{};

    // Restore castling rook position
    if (is_castling) {
        if (move.to_x == 6) { // kingside
            board.at(7, move.from_y).piece = board.at(5, move.from_y).piece;
            board.at(5, move.from_y).piece = piece_t{};
        } else if (move.to_x == 2) { // queenside
            board.at(0, move.from_y).piece = board.at(3, move.from_y).piece;
            board.at(3, move.from_y).piece = piece_t{};
        }
    }

    // Restore captured piece
    if (is_en_passant) {
        board.at(move.to_x, move.from_y).piece = captured_piece;
    } else {
        to_square.piece = captured_piece;
    }
}

// returns a vector of the squares that a sliding move goes through, excluding the from- and to- squares of the move!
vector<square_t> line(const board_t& board, const move_t& move) {
    vector<square_t> squares;

    bool up    = move.from_y < move.to_y;
    bool right = move.from_x < move.to_x;

    if (move.from_x == move.to_x) {
        // Vertical line
        if (move.from_y < move.to_y) {
            // Vertical line going up
            for (int j = move.from_y + 1; j < move.to_y; j++) {
                squares.push_back(board.at(move.from_x, j));
            }
            return squares;
        } else {
            // Vertical line going down
            for (int j = move.from_y - 1; j > move.to_y; j--) {
                squares.push_back(board.at(move.from_x, j));
            }
            return squares;
        }
    } else if (move.from_y == move.to_y) {
        // Horizontal line
        if (move.from_x < move.to_x) {
            // Horizontal line going right
            for (int i = move.from_x + 1; i < move.to_x; i++) {
                squares.push_back(board.at(i, move.from_y));
            }
            return squares;
        } else {
            // Horizontal line going left
            for (int i = move.from_x - 1; i > move.to_x; i--) {
                squares.push_back(board.at(i, move.from_y));
            }
            return squares;
        }
    }

    // x and y increment based on the direction of the line
    int x_dir = 2 * right - 1;
    int y_dir = 2 * up - 1;
    // Diagonals
    for (int i = 1; i < abs(move.to_x - move.from_x); i++) {
        squares.push_back(board.at(move.from_x + x_dir * i, move.from_y + y_dir * i));
    }
    return squares;
}

// returns true if there's a threatening piece in this line that can give check
bool is_check_from_line(const board_t& board, const vector<square_t>& diagonal, Color color, bool is_diagonal) {
    for (const auto& square : diagonal) {
        const piece_t& piece = board.at(square.x, square.y).piece;

        if (piece.type != PieceType::EMPTY) {
            // check if it's an enemy bishop/queen on diagonal, or rook/queen on straight line
            if (piece.color != color && ((is_diagonal && (piece.type == PieceType::BISHOP || piece.type == PieceType::QUEEN)) ||
                                         (!is_diagonal && (piece.type == PieceType::ROOK || piece.type == PieceType::QUEEN)))) {
                return true;
            }
            // if we hit any other piece, it blocks line of sight
            return false;
        }
    }
    return false;
}

bool is_in_check(const board_t& board, Color color) {
    // First find the king
    int k_x = -1, k_y = -1;
    for (const auto& square : board) {
        if (square.piece.type == PieceType::KING && square.piece.color == color) {
            k_x = square.x;
            k_y = square.y;
            break;
        }
    }
    if (k_x == -1)
        return false; // no king found

    // Check each direction for sliding pieces (rook, bishop, queen)
    const array<pair<int, int>, 8> all_directions = {
        // Diagonal directions
        make_pair(1, 1), make_pair(1, -1), make_pair(-1, 1), make_pair(-1, -1),
        // Straight directions
        make_pair(0, 1), make_pair(0, -1), make_pair(1, 0), make_pair(-1, 0)};

    for (const auto& [dx, dy] : all_directions) {
        int x            = k_x + dx;
        int y            = k_y + dy;
        bool is_diagonal = dx != 0 && dy != 0;

        while (board.in_board(x, y)) {
            const piece_t& piece = board.at(x, y).piece;
            if (piece.type != PieceType::EMPTY) {
                if (piece.color != color) {
                    if (is_diagonal && (piece.type == PieceType::BISHOP || piece.type == PieceType::QUEEN)) {
                        return true;
                    }
                    if (!is_diagonal && (piece.type == PieceType::ROOK || piece.type == PieceType::QUEEN)) {
                        return true;
                    }
                }
                break; // blocked by any piece
            }
            x += dx;
            y += dy;
        }
    }

    // Check for knight threats
    const array<pair<int, int>, 8> knight_moves = {
        make_pair(2, 1), make_pair(2, -1), make_pair(-2, 1), make_pair(-2, -1),
        make_pair(1, 2), make_pair(1, -2), make_pair(-1, 2), make_pair(-1, -2)};

    for (const auto& [dx, dy] : knight_moves) {
        int x = k_x + dx;
        int y = k_y + dy;
        if (board.in_board(x, y)) {
            const piece_t& piece = board.at(x, y).piece;
            if (piece.type == PieceType::KNIGHT && piece.color != color) {
                return true;
            }
        }
    }

    // Check for pawn threats
    const pair<int, int> pawn_attacks[] = {
        make_pair(-1, color == Color::WHITE ? 1 : -1),
        make_pair(1, color == Color::WHITE ? 1 : -1)};

    for (const auto& [dx, dy] : pawn_attacks) {
        int x = k_x + dx;
        int y = k_y + dy;
        if (board.in_board(x, y)) {
            const piece_t& piece = board.at(x, y).piece;
            if (piece.type == PieceType::PAWN && piece.color != color) {
                return true;
            }
        }
    }

    // Check for king threats (needed for some edge cases)
    for (const auto& [dx, dy] : all_directions) {
        int x = k_x + dx;
        int y = k_y + dy;
        if (board.in_board(x, y)) {
            const piece_t& piece = board.at(x, y).piece;
            if (piece.type == PieceType::KING && piece.color != color) {
                return true;
            }
        }
    }

    return false;
}

bool is_move_legal(board_t& board, const move_t& move) {
    // checks knight moves
    vector<square_t> line_of_move = line(board, move);
    if (board.at(move.from_x, move.from_y).piece.type != PieceType::KNIGHT) {
        for (auto& square : line_of_move) {
            if (board.at(square.x, square.y).piece.type != PieceType::EMPTY) {
                return false;
            }
        }
    }

    // checks target is empty or of different color
    Color moving_color = board.at(move.from_x, move.from_y).piece.color;
    if (board.at(move.to_x, move.to_y).piece.color == moving_color) {
        return false;
    }

    // make move and check if it reveals check
    piece_t captured = make_move(board, move);
    bool is_checked  = is_in_check(board, moving_color);
    undo_move(board, move, captured);

    return !is_checked;
}

vector<move_t> get_pawn_moves(const board_t& board, int x, int y) {
    vector<move_t> moves;
    Color own_color = board.at(x, y).piece.color;
    int direction   = (own_color == Color::WHITE) ? 1 : -1;

    // regular move
    if (board.in_board(x, y + direction) && board.at(x, y + direction).piece.type == PieceType::EMPTY) {
        // promotion for straight moves
        if ((own_color == Color::WHITE && y + direction == 7) ||
            (own_color == Color::BLACK && y + direction == 0)) {
            for (PieceType promotion : {PieceType::QUEEN, PieceType::ROOK,
                                        PieceType::BISHOP, PieceType::KNIGHT}) {
                moves.push_back(move_t{x, y, x, y + direction, promotion});
            }
        } else {
            moves.push_back(move_t{x, y, x, y + direction, PieceType::EMPTY});

            // first move - two squares (only if not promoting)
            if ((own_color == Color::WHITE && y == 1) || (own_color == Color::BLACK && y == 6)) {
                if (board.at(x, y + 2 * direction).piece.type == PieceType::EMPTY) {
                    moves.push_back(move_t{x, y, x, y + 2 * direction, PieceType::EMPTY});
                }
            }
        }
    }

    // captures
    for (int dx : {-1, 1}) {
        if (board.in_board(x + dx, y + direction)) {
            const square_t& target = board.at(x + dx, y + direction);
            if (target.piece.type != PieceType::EMPTY && target.piece.color != own_color) {
                // promotion for capture moves
                if ((own_color == Color::WHITE && y + direction == 7) ||
                    (own_color == Color::BLACK && y + direction == 0)) {
                    for (PieceType promotion : {PieceType::QUEEN, PieceType::ROOK,
                                                PieceType::BISHOP, PieceType::KNIGHT}) {
                        moves.push_back(move_t{x, y, x + dx, y + direction, promotion});
                    }
                } else {
                    moves.push_back(move_t{x, y, x + dx, y + direction, PieceType::EMPTY});
                }
            }
        }
    }

    // en-passant
    if (board.en_passant_x == x + 1 && board.en_passant_y == y + direction) {
        moves.push_back(move_t{x, y, x + 1, y + direction, PieceType::EMPTY});
    }
    if (board.en_passant_x == x - 1 && board.en_passant_y == y + direction) {
        moves.push_back(move_t{x, y, x - 1, y + direction, PieceType::EMPTY});
    }

    return moves;
}

vector<move_t> get_rook_moves(const board_t& board, int x, int y) {
    vector<move_t> moves;
    Color own_color = board.at(x, y).piece.color;

    const array<pair<int, int>, 4> directions = {
        make_pair(0, 1),  // up
        make_pair(0, -1), // down
        make_pair(1, 0),  // right
        make_pair(-1, 0)  // left
    };

    for (const auto& [dx, dy] : directions) {
        int current_x = x + dx;
        int current_y = y + dy;

        while (board.in_board(current_x, current_y)) {
            const square_t& target = board.at(current_x, current_y);

            if (target.piece.type == PieceType::EMPTY) {
                moves.push_back(move_t{x, y, current_x, current_y, PieceType::EMPTY});
            } else {
                if (target.piece.color != own_color) {
                    moves.push_back(move_t{x, y, current_x, current_y, PieceType::EMPTY});
                }
                break; // stop after encountering any piece
            }

            current_x += dx;
            current_y += dy;
        }
    }

    return moves;
}

vector<move_t> get_knight_moves(const board_t& board, int x, int y) {
    vector<move_t> moves;
    Color own_color = board.at(x, y).piece.color;

    vector<array<int, 2>> offsets = {
        {x + 1, y + 2}, {x - 1, y + 2}, {x + 1, y - 2}, {x - 1, y - 2}, {x + 2, y + 1}, {x - 2, y + 1}, {x + 2, y - 1}, {x - 2, y - 1}};

    for (const auto& [to_x, to_y] : offsets) {
        if (board.in_board(to_x, to_y) && board.at(to_x, to_y).piece.color != own_color) {
            moves.push_back(move_t{x, y, to_x, to_y, PieceType::EMPTY});
        }
    }

    return moves;
}

vector<move_t> get_bishop_moves(const board_t& board, int x, int y) {
    vector<move_t> moves;
    Color own_color = board.at(x, y).piece.color;

    const array<pair<int, int>, 4> directions = {
        make_pair(1, 1),  // up-right
        make_pair(1, -1), // down-right
        make_pair(-1, 1), // up-left
        make_pair(-1, -1) // down-left
    };

    for (const auto& [dx, dy] : directions) {
        int current_x = x + dx;
        int current_y = y + dy;

        while (board.in_board(current_x, current_y)) {
            const square_t& target = board.at(current_x, current_y);

            if (target.piece.type == PieceType::EMPTY) {
                moves.push_back(move_t{x, y, current_x, current_y, PieceType::EMPTY});
            } else {
                if (target.piece.color != own_color) {
                    moves.push_back(move_t{x, y, current_x, current_y, PieceType::EMPTY});
                }
                break; // stop after encountering any piece
            }

            current_x += dx;
            current_y += dy;
        }
    }

    return moves;
}

vector<move_t> get_queen_moves(const board_t& board, int x, int y) {
    vector<move_t> line_moves     = get_rook_moves(board, x, y);
    vector<move_t> diagonal_moves = get_bishop_moves(board, x, y);
    line_moves.insert(line_moves.end(), diagonal_moves.begin(), diagonal_moves.end());
    return line_moves;
}

static bool has_moved(const board_t& board, int x, int y) {
    // Checks whether any move including x,y has ever been made:
    for (const auto& move_t : board.history) {
        if ((move_t.from_x == x && move_t.from_y == y) || (move_t.to_x == x && move_t.to_y == y)) {
            return true;
        }
    }
    return false;
}

bool is_square_attacked(const board_t& board, int target_x, int target_y, Color attacking_color) {
    // Check pawn attacks
    int pawn_direction = (attacking_color == Color::WHITE) ? 1 : -1;
    // Check both diagonal captures
    for (int dx : {-1, 1}) {
        int px = target_x + dx;
        int py = target_y - pawn_direction;
        if (board.in_board(px, py)) {
            const piece_t& piece = board.at(px, py).piece;
            if (piece.type == PieceType::PAWN && piece.color == attacking_color) {
                return true;
            }
        }
    }

    // Check knight attacks
    vector<array<int, 2>> knight_offsets = {
        {-2, -1}, {-2, 1}, {2, -1}, {2, 1}, {-1, -2}, {1, -2}, {-1, 2}, {1, 2}};
    for (const auto& [dx, dy] : knight_offsets) {
        int nx = target_x + dx;
        int ny = target_y + dy;
        if (board.in_board(nx, ny)) {
            const piece_t& piece = board.at(nx, ny).piece;
            if (piece.type == PieceType::KNIGHT && piece.color == attacking_color) {
                return true;
            }
        }
    }

    // Check diagonal attacks (bishop/queen)
    vector<array<int, 2>> diagonals = {{1, 1}, {1, -1}, {-1, 1}, {-1, -1}};
    for (const auto& [dx, dy] : diagonals) {
        int x = target_x + dx;
        int y = target_y + dy;
        while (board.in_board(x, y)) {
            const piece_t& piece = board.at(x, y).piece;
            if (piece.type != PieceType::EMPTY) {
                if (piece.color == attacking_color &&
                    (piece.type == PieceType::BISHOP || piece.type == PieceType::QUEEN)) {
                    return true;
                }
                break;
            }
            x += dx;
            y += dy;
        }
    }

    // Check horizontal/vertical attacks (rook/queen)
    vector<array<int, 2>> straight = {{0, 1}, {0, -1}, {1, 0}, {-1, 0}};
    for (const auto& [dx, dy] : straight) {
        int x = target_x + dx;
        int y = target_y + dy;
        while (board.in_board(x, y)) {
            const piece_t& piece = board.at(x, y).piece;
            if (piece.type != PieceType::EMPTY) {
                if (piece.color == attacking_color &&
                    (piece.type == PieceType::ROOK || piece.type == PieceType::QUEEN)) {
                    return true;
                }
                break;
            }
            x += dx;
            y += dy;
        }
    }

    // Check king attacks (for adjacent squares)
    vector<array<int, 2>> king_offsets = {
        {-1, -1}, {-1, 0}, {-1, 1}, {0, -1}, {0, 1}, {1, -1}, {1, 0}, {1, 1}};
    for (const auto& [dx, dy] : king_offsets) {
        int kx = target_x + dx;
        int ky = target_y + dy;
        if (board.in_board(kx, ky)) {
            const piece_t& piece = board.at(kx, ky).piece;
            if (piece.type == PieceType::KING && piece.color == attacking_color) {
                return true;
            }
        }
    }

    return false;
}

vector<move_t> get_king_moves(const board_t& board, int x, int y) {
    vector<move_t> moves;
    Color own_color   = board.at(x, y).piece.color;
    Color enemy_color = (own_color == Color::WHITE) ? Color::BLACK : Color::WHITE;

    // Normal king moves
    vector<array<int, 2>> offsets = {
        {x + 1, y + 1}, {x + 1, y}, {x + 1, y - 1}, {x, y - 1}, {x - 1, y - 1}, {x - 1, y}, {x - 1, y + 1}, {x, y + 1}};

    for (const auto& [to_x, to_y] : offsets) {
        if (board.in_board(to_x, to_y) && board.at(to_x, to_y).piece.color != own_color) {
            moves.push_back(move_t{x, y, to_x, to_y, PieceType::EMPTY});
        }
    }

    // Castling moves
    if (own_color == Color::WHITE) {
        // Check if king is in check
        if (!is_in_check(board, Color::WHITE)) {
            // Kingside castle
            if (board.white_king_side_castle) {
                if (board.at(5, 0).piece.type == PieceType::EMPTY &&
                    board.at(6, 0).piece.type == PieceType::EMPTY) {

                    // Check if squares are attacked
                    bool f1_attacked = is_square_attacked(board, 5, 0, enemy_color);
                    bool g1_attacked = is_square_attacked(board, 6, 0, enemy_color);

                    if (!f1_attacked && !g1_attacked) {
                        moves.push_back(move_t{4, 0, 6, 0, PieceType::EMPTY});
                    }
                }
            }

            // Queenside castle
            if (board.white_queen_side_castle) {
                if (board.at(1, 0).piece.type == PieceType::EMPTY &&
                    board.at(2, 0).piece.type == PieceType::EMPTY &&
                    board.at(3, 0).piece.type == PieceType::EMPTY) {

                    // Check if squares are attacked
                    bool d1_attacked = is_square_attacked(board, 3, 0, enemy_color);
                    bool c1_attacked = is_square_attacked(board, 2, 0, enemy_color);

                    if (!d1_attacked && !c1_attacked) {
                        moves.push_back(move_t{4, 0, 2, 0, PieceType::EMPTY});
                    }
                }
            }
        }
    } else {
        // Check if king is in check
        if (!is_in_check(board, Color::BLACK)) {
            // Kingside castle
            if (board.black_king_side_castle) {
                if (board.at(5, 7).piece.type == PieceType::EMPTY &&
                    board.at(6, 7).piece.type == PieceType::EMPTY) {

                    // Check if squares are attacked
                    bool f8_attacked = is_square_attacked(board, 5, 7, enemy_color);
                    bool g8_attacked = is_square_attacked(board, 6, 7, enemy_color);

                    if (!f8_attacked && !g8_attacked) {
                        moves.push_back(move_t{4, 7, 6, 7, PieceType::EMPTY});
                    }
                }
            }

            // Queenside castle
            if (board.black_queen_side_castle) {
                if (board.at(1, 7).piece.type == PieceType::EMPTY &&
                    board.at(2, 7).piece.type == PieceType::EMPTY &&
                    board.at(3, 7).piece.type == PieceType::EMPTY) {

                    // Check if squares are attacked
                    bool d8_attacked = is_square_attacked(board, 3, 7, enemy_color);
                    bool c8_attacked = is_square_attacked(board, 2, 7, enemy_color);

                    if (!d8_attacked && !c8_attacked) {
                        moves.push_back(move_t{4, 7, 2, 7, PieceType::EMPTY});
                    }
                }
            }
        }
    }

    return moves;
}

vector<move_t> get_piece_moves(const board_t& board, int x, int y) {
    vector<move_t> moves;
    const square_t& square = board.at(x, y);

    switch (square.piece.type) {
    case PieceType::PAWN: return get_pawn_moves(board, x, y);
    case PieceType::ROOK: return get_rook_moves(board, x, y);
    case PieceType::KNIGHT: return get_knight_moves(board, x, y);
    case PieceType::BISHOP: return get_bishop_moves(board, x, y);
    case PieceType::QUEEN: return get_queen_moves(board, x, y);
    case PieceType::KING: return get_king_moves(board, x, y);
    default: return moves;
    }
}

vector<move_t> get_all_moves(const board_t& board) {
    vector<move_t> all_moves;

    for (const auto& square : board) {
        // skip empty squares and pieces of the wrong color
        if (square.piece.type == PieceType::EMPTY) {
            continue;
        }

        vector<move_t> piece_moves = get_piece_moves(board, square.x, square.y);

        // check if each move is legal
        for (const auto& move : piece_moves) {
            board_t temp_board = board; // Create a copy of the board
            if (is_move_legal(temp_board, move)) {
                all_moves.push_back(move);
            }
        }
    }

    return all_moves;
}

vector<move_t> generate_all_moves_for_color(board_t& board, Color color) {
    vector<move_t> all_moves;

    for (const auto& square : board) {
        if (square.piece.type == PieceType::EMPTY ||
            square.piece.color != color ||
            !board.in_board(square.x, square.y)) {
            continue;
        }

        vector<move_t> piece_moves = get_piece_moves(board, square.x, square.y);

        for (const auto& move : piece_moves) {
            piece_t captured = make_move(board, move);
            if (!is_in_check(board, color)) {
                all_moves.push_back(move);
            }
            undo_move(board, move, captured);
        }
    }

    return all_moves;
}

} // namespace moves

#endif