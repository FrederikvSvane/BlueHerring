// move_logic.hpp
#ifndef move_logic_hpp
#define move_logic_hpp

#include "board_t.hpp"
#include "move_t.hpp"
#include <array>
#include <cmath>    //for absolute value
#include <stdint.h> //had to include this, otherwise didn't compile on my pc
#include <vector>

namespace bitboard_moves {

// Precomputed king moves table (indexed by square position)
static const U64 king_attack_table[64] = {
    0x0000000000000302ULL, 0x0000000000000705ULL, 0x0000000000000E0AULL, 0x0000000000001C14ULL,
    0x0000000000003828ULL, 0x0000000000007050ULL, 0x000000000000E0A0ULL, 0x000000000000C040ULL,
    0x0000000000030203ULL, 0x0000000000070507ULL, 0x00000000000E0A0EULL, 0x00000000001C141CULL,
    0x0000000000382838ULL, 0x0000000000705070ULL, 0x0000000000E0A0E0ULL, 0x0000000000C040C0ULL,
    0x0000000003020300ULL, 0x0000000007050700ULL, 0x000000000E0A0E00ULL, 0x000000001C141C00ULL,
    0x0000000038283800ULL, 0x0000000070507000ULL, 0x00000000E0A0E000ULL, 0x00000000C040C000ULL,
    0x0000000302030000ULL, 0x0000000705070000ULL, 0x0000000E0A0E0000ULL, 0x0000001C141C0000ULL,
    0x0000003828380000ULL, 0x0000007050700000ULL, 0x000000E0A0E00000ULL, 0x000000C040C00000ULL,
    0x0000030203000000ULL, 0x0000070507000000ULL, 0x00000E0A0E000000ULL, 0x00001C141C000000ULL,
    0x0000382838000000ULL, 0x0000705070000000ULL, 0x0000E0A0E0000000ULL, 0x0000C040C0000000ULL,
    0x0003020300000000ULL, 0x0007050700000000ULL, 0x000E0A0E00000000ULL, 0x001C141C00000000ULL,
    0x0038283800000000ULL, 0x0070507000000000ULL, 0x00E0A0E000000000ULL, 0x00C040C000000000ULL,
    0x0302030000000000ULL, 0x0705070000000000ULL, 0x0E0A0E0000000000ULL, 0x1C141C0000000000ULL,
    0x3828380000000000ULL, 0x7050700000000000ULL, 0xE0A0E00000000000ULL, 0xC040C00000000000ULL,
    0x0203000000000000ULL, 0x0507000000000000ULL, 0x0A0E000000000000ULL, 0x141C000000000000ULL,
    0x2838000000000000ULL, 0x5070000000000000ULL, 0xA0E0000000000000ULL, 0x40C0000000000000ULL};

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
    0x0044280000000000ULL, 0x0088500000000000ULL, 0x0010A00000000000ULL, 0x0020400000000000ULL};

void update_castling_rights(bitboard_t& board, const bitboard_move_t& move) {
    int from_idx = __builtin_ctzll(move.from_board);
    int to_idx   = __builtin_ctzll(move.to_board);

    // Check if king moves
    if (board.board_w_K & move.from_board) {
        board.white_king_side_castle  = false;
        board.white_queen_side_castle = false;
    }
    if (board.board_b_K & move.from_board) {
        board.black_king_side_castle  = false;
        board.black_queen_side_castle = false;
    }

    // Check rook moves or captures
    if (from_idx == 0 || to_idx == 0)
        board.white_queen_side_castle = false; // a1
    if (from_idx == 7 || to_idx == 7)
        board.white_king_side_castle = false; // h1
    if (from_idx == 56 || to_idx == 56)
        board.black_queen_side_castle = false; // a8
    if (from_idx == 63 || to_idx == 63)
        board.black_king_side_castle = false; // h8
}

void update_en_passant_square(bitboard_t& board, const bitboard_move_t& move) {
    board.en_passant_square = 0; // Reset

    // Check for pawn double push
    int from_idx = __builtin_ctzll(move.from_board);
    int to_idx   = __builtin_ctzll(move.to_board);

    if ((board.board_w_P & move.from_board || board.board_b_P & move.from_board) &&
        abs(to_idx / 8 - from_idx / 8) == 2) {
        // Set en passant square to the square between from and to
        int ep_idx              = (from_idx + to_idx) / 2;
        board.en_passant_square = 1ULL << ep_idx;
    }
}

piece_t make_move(bitboard_t& board, const bitboard_move_t& move) { // In the future, a move_t could simply be two bits, or a bitboard with the from/to bits set to 1
    // Saving current state (pushing to the stack) before making any changes
    board.save_current_state();

    int from_idx = __builtin_ctzll(move.from_board);
    int to_idx   = __builtin_ctzll(move.to_board);

    // Get the moving piece and its bitboard
    piece_t moving_piece = board.at(from_idx % 8, from_idx / 8).piece;
    if (moving_piece.type == PieceType::EMPTY) {
        throw std::invalid_argument("No piece at source square"); // maybe we can remove this, i just added for safety
    }
    U64* piece_board = board.get_board_for_piece(moving_piece.type, moving_piece.color);
    if (!piece_board) {
        throw std::runtime_error("Invalid piece type or color"); // maybe we can remove this, i just added for safety
    }

    // Get the captured piece (if any)
    piece_t captured_piece = {PieceType::EMPTY, Color::NONE};
    U64 to_square_mask     = 1ULL << to_idx;
    if (moving_piece.color == Color::WHITE) {
        if (board.board_b_P & to_square_mask) {
            captured_piece = {PieceType::PAWN, Color::BLACK};
            board.board_b_P &= ~to_square_mask;
        } else if (board.board_b_N & to_square_mask) {
            captured_piece = {PieceType::KNIGHT, Color::BLACK};
            board.board_b_N &= ~to_square_mask;
        } else if (board.board_b_B & to_square_mask) {
            captured_piece = {PieceType::BISHOP, Color::BLACK};
            board.board_b_B &= ~to_square_mask;
        } else if (board.board_b_R & to_square_mask) {
            captured_piece = {PieceType::ROOK, Color::BLACK};
            board.board_b_R &= ~to_square_mask;
        } else if (board.board_b_Q & to_square_mask) {
            captured_piece = {PieceType::QUEEN, Color::BLACK};
            board.board_b_Q &= ~to_square_mask;
        }
    } else {
        if (board.board_w_P & to_square_mask) {
            captured_piece = {PieceType::PAWN, Color::WHITE};
            board.board_w_P &= ~to_square_mask;
        } else if (board.board_w_N & to_square_mask) {
            captured_piece = {PieceType::KNIGHT, Color::WHITE};
            board.board_w_N &= ~to_square_mask;
        } else if (board.board_w_B & to_square_mask) {
            captured_piece = {PieceType::BISHOP, Color::WHITE};
            board.board_w_B &= ~to_square_mask;
        } else if (board.board_w_R & to_square_mask) {
            captured_piece = {PieceType::ROOK, Color::WHITE};
            board.board_w_R &= ~to_square_mask;
        } else if (board.board_w_Q & to_square_mask) {
            captured_piece = {PieceType::QUEEN, Color::WHITE};
            board.board_w_Q &= ~to_square_mask;
        }
    }

    // En passant capture
    if (moving_piece.type == PieceType::PAWN && (to_square_mask & board.en_passant_square)) {
        int capture_y    = from_idx / 8; // Same rank as attacking pawn
        int capture_x    = to_idx % 8;   // Same file as target square
        U64 capture_mask = 1ULL << (capture_y * 8 + capture_x);

        if (moving_piece.color == Color::WHITE) {
            board.board_b_P &= ~capture_mask;
            captured_piece = {PieceType::PAWN, Color::BLACK};
        } else {
            board.board_w_P &= ~capture_mask;
            captured_piece = {PieceType::PAWN, Color::WHITE};
        }
    }

    // Castling rook movement
    if (moving_piece.type == PieceType::KING && abs((to_idx % 8) - (from_idx % 8)) == 2) {
        int rank = from_idx / 8;
        if (to_idx % 8 == 6) { // Kingside castle
            U64* rook_board = (moving_piece.color == Color::WHITE) ? &board.board_w_R : &board.board_b_R;
            *rook_board &= ~(1ULL << (rank * 8 + 7)); // Remove rook from h-file
            *rook_board |= 1ULL << (rank * 8 + 5);    // Place rook on f-file
        } else if (to_idx % 8 == 2) {                 // Queenside castle
            U64* rook_board = (moving_piece.color == Color::WHITE) ? &board.board_w_R : &board.board_b_R;
            *rook_board &= ~(1ULL << (rank * 8));  // Remove rook from a-file
            *rook_board |= 1ULL << (rank * 8 + 3); // Place rook on d-file
        }
    }

    // Pawn promotion
    if (move.promotion_type != PieceType::EMPTY) {
        // Remove pawn from source
        if (moving_piece.color == Color::WHITE) {
            board.board_w_P &= ~move.from_board;
        } else {
            board.board_b_P &= ~move.from_board;
        }
        // Add promoted piece at destination
        U64* promoted_board = board.get_board_for_piece(move.promotion_type, moving_piece.color);
        *promoted_board |= to_square_mask;
    } else {
        // Regular move
        board.move_bit(piece_board, from_idx, to_idx);
    }

    // Update castling rights and en passant square
    update_castling_rights(board, move);
    update_en_passant_square(board, move);

    // Add move to history
    board.history.push_back(move);

    return captured_piece;
}

void undo_move(bitboard_t& board, const bitboard_move_t& move, const piece_t& captured_piece) {
    // Get indices
    int from_idx = __builtin_ctzll(move.from_board);
    int to_idx   = __builtin_ctzll(move.to_board);

    // Remove the move from move history
    if (board.history.empty()) {
        throw std::runtime_error("No moves to undo");
    }
    board.history.pop_back();

    // Restore the previous state
    board.restore_previous_state();

    // Get moving piece (from destination square since the move was already made)
    piece_t moving_piece = board.at(to_idx % 8, to_idx / 8).piece;

    // Handle promotion undo
    if (move.promotion_type != PieceType::EMPTY) {
        // Remove promoted piece
        U64* promoted_board = board.get_board_for_piece(move.promotion_type, moving_piece.color);
        *promoted_board &= ~(1ULL << to_idx);

        // Restore pawn
        U64* pawn_board = board.get_board_for_piece(PieceType::PAWN, moving_piece.color);
        *pawn_board |= (1ULL << from_idx);
    } else {
        // Regular move undo
        U64* piece_board = board.get_board_for_piece(moving_piece.type, moving_piece.color);
        board.move_bit(piece_board, to_idx, from_idx);
    }

    // Restore captured piece if any
    if (captured_piece.type != PieceType::EMPTY) {
        if (board.en_passant_square & (1ULL << to_idx)) {
            // En passant capture undo
            int capture_y       = from_idx / 8;
            int capture_x       = to_idx % 8;
            U64* captured_board = board.get_board_for_piece(PieceType::PAWN, captured_piece.color);
            *captured_board |= 1ULL << (capture_y * 8 + capture_x);
        } else {
            // Regular capture undo
            U64* captured_board = board.get_board_for_piece(captured_piece.type, captured_piece.color);
            *captured_board |= 1ULL << to_idx;
        }
    }

    // Undo castling rook movement
    if (moving_piece.type == PieceType::KING && abs((to_idx % 8) - (from_idx % 8)) == 2) {
        int rank = from_idx / 8;
        if (to_idx % 8 == 6) { // Kingside castle
            U64* rook_board = (moving_piece.color == Color::WHITE) ? &board.board_w_R : &board.board_b_R;
            *rook_board |= 1ULL << (rank * 8 + 7);    // Return rook to h-file
            *rook_board &= ~(1ULL << (rank * 8 + 5)); // Remove rook from f-file
        } else if (to_idx % 8 == 2) {                 // Queenside castle
            U64* rook_board = (moving_piece.color == Color::WHITE) ? &board.board_w_R : &board.board_b_R;
            *rook_board |= 1ULL << (rank * 8);        // Return rook to a-file
            *rook_board &= ~(1ULL << (rank * 8 + 3)); // Remove rook from d-file
        }
    }
}

// Used for rook/queen
U64 get_orthogonal_moves(U64 occupied, U64 friendly_pieces, int pos) {
    U64 orthogonal_moves    = 0;
    const int directions[4] = {8, -8, 1, -1}; // North, South, East, West

    for (int dir : directions) {
        int square = pos;
        while (true) {
            square += dir;

            // Out of bounds
            if (square < 0 || square >= 64)
                break;

            // Handle edge cases for east/west wrapping
            if ((dir == 1 && square % 8 == 0) || (dir == -1 && square % 8 == 7))
                break;

            U64 square_bit = 1ULL << square;
            orthogonal_moves |= square_bit;

            // Stop at blockers
            if (occupied & square_bit) {
                if (friendly_pieces & square_bit) {
                    orthogonal_moves &= ~square_bit; // Remove square if blocked by friendly piece
                }
                break; // Stop sliding in this direction
            }
        }
    }

    return orthogonal_moves;
}

// Used for bishop/queen
U64 get_diagonal_moves(U64 occupied, U64 friendly_pieces, int pos) {
    U64 diagonal_moves      = 0;
    const int directions[4] = {9, -9, 7, -7}; // North-East, South-West, North-West, South-East

    for (int dir : directions) {
        int square = pos;
        while (true) {
            square += dir;

            // Out of bounds
            if (square < 0 || square >= 64)
                break;

            // if (dir == -9) {
            //     printf("square %i\n", square);
            // }

            // Handle edge cases for diagonal wrapping
            if ((dir == 9 && square % 8 == 0) ||  // North-East wraps around
                (dir == -9 && square % 8 == 7) || // South-West wraps around
                (dir == 7 && square % 8 == 7) ||  // North-West wraps around
                (dir == -7 && square % 8 == 0))   // South-East wraps around
                break;

            U64 square_bit = 1ULL << square;
            diagonal_moves |= square_bit;

            // Stop at blockers
            if (occupied & square_bit) {
                if (friendly_pieces & square_bit) {
                    diagonal_moves &= ~square_bit; // Remove square if blocked by friendly piece
                }
                break; // Stop sliding in this direction
            }
        }
    }

    return diagonal_moves;
}

// Pass the color under attack
bool is_square_under_attack(bitboard_t& board, Color color, int x, int y) {
    int pos    = y * 8 + x;
    U64 square = 1ULL << pos;

    // King attacks
    if ((king_attack_table[pos] & *board.get_board_for_piece(PieceType::KING, !color)) != 0) {
        return true;
    }

    // Knight attacks
    if ((knight_attack_table[pos] & *board.get_board_for_piece(PieceType::KNIGHT, !color)) != 0) {
        return true;
    }

    U64 allied_pieces = board.get_all_friendly_pieces(color);
    U64 occupied      = board.get_all_pieces();

    U64 orthogonal_moves = get_orthogonal_moves(occupied, allied_pieces, pos);
    U64 diagonal_moves   = get_diagonal_moves(occupied, allied_pieces, pos);

    U64 rooks   = *board.get_board_for_piece(PieceType::ROOK, !color);
    U64 bishops = *board.get_board_for_piece(PieceType::BISHOP, !color);
    U64 queens  = *board.get_board_for_piece(PieceType::QUEEN, !color);

    // Orthogonal attacks
    if (orthogonal_moves & (rooks | queens)) {
        return true;
    }

    // Diagonal attacks
    if (diagonal_moves & (bishops | queens)) {
        return true;
    }

    // Pawn attacks
    U64 pawns        = *board.get_board_for_piece(PieceType::PAWN, !color);
    U64 pawn_attacks = 0ULL;

    if (color == Color::WHITE) {
        // Check if black pawns can attack this square
        pawn_attacks = ((square << 7) & ~0x0101010101010101ULL) | // exclude a-file
                       ((square << 9) & ~0x8080808080808080ULL);  // exclude h-file
    } else {
        // Check if white pawns can attack this square
        pawn_attacks = ((square >> 7) & ~0x8080808080808080ULL) | // exclude h-file
                       ((square >> 9) & ~0x0101010101010101ULL);  // exclude a-file
    }

    if (pawn_attacks & pawns) {
        return true;
    }

    return false;
}

bool is_in_check(bitboard_t& board, Color color) {
    // Find king position
    U64 king_board = (color == Color::WHITE) ? board.board_w_K : board.board_b_K;
    int king_pos   = __builtin_ctzll(king_board);
    // Check if he's under attack
    return is_square_under_attack(board, color, king_pos % 8, king_pos / 8);
}

vector<bitboard_move_t> get_knight_moves(bitboard_t& board, int x, int y) {
    int pos            = y * 8 + x;
    U64 from_square    = board.single_bitmask(pos);
    U64 possible_moves = knight_attack_table[pos];                                      // get the possible moves table for the given square
    Color knight_color = (board.board_w_N & from_square) ? Color::WHITE : Color::BLACK; // "is one of the white knights on the from-square?" if yes then color=white
    possible_moves &= ~board.get_all_friendly_pieces(knight_color);                     // remove moves to squares occupied by friendly pieces
    return get_moves_from_possible_moves_bitboard(possible_moves, from_square);
}

vector<bitboard_move_t> get_rook_moves(bitboard_t& board, int x, int y) {
    int pos         = y * 8 + x;
    U64 from_square = board.single_bitmask(pos);

    U64 occupied        = board.get_all_pieces();
    Color rook_color    = (board.board_w_R & from_square) ? Color::WHITE : Color::BLACK;
    U64 friendly_pieces = board.get_all_friendly_pieces(rook_color);

    U64 possible_moves = get_orthogonal_moves(occupied, friendly_pieces, pos);

    return get_moves_from_possible_moves_bitboard(possible_moves, from_square);
}

vector<bitboard_move_t> get_bishop_moves(bitboard_t& board, int x, int y) {
    int pos         = y * 8 + x;
    U64 from_square = board.single_bitmask(pos);

    U64 occupied = board.get_all_pieces();

    // Friendly pieces to exclude
    Color bishop_color  = (board.board_w_B & from_square) ? Color::WHITE : Color::BLACK;
    U64 friendly_pieces = board.get_all_friendly_pieces(bishop_color);

    U64 possible_moves = get_diagonal_moves(occupied, friendly_pieces, pos);

    return get_moves_from_possible_moves_bitboard(possible_moves, from_square);
}

vector<bitboard_move_t> get_queen_moves(bitboard_t& board, int x, int y) {
    int pos         = y * 8 + x;
    U64 from_square = board.single_bitmask(pos);

    U64 occupied        = board.get_all_pieces();
    Color queen_color   = (board.board_w_Q & from_square) ? Color::WHITE : Color::BLACK;
    U64 friendly_pieces = board.get_all_friendly_pieces(queen_color);

    // Get both diagonal and orthogonal moves
    U64 possible_moves = get_diagonal_moves(occupied, friendly_pieces, pos) |
                         get_orthogonal_moves(occupied, friendly_pieces, pos);

    return get_moves_from_possible_moves_bitboard(possible_moves, from_square);
}

vector<bitboard_move_t> get_pawn_moves(bitboard_t& board, int x, int y) {
    int pos         = y * 8 + x;
    U64 from_square = board.single_bitmask(pos);
    vector<bitboard_move_t> possible_moves;

    Color pawn_color = (board.board_w_P & from_square) ? Color::WHITE : Color::BLACK; // "is there a white pawn on the from square?" if yes => color=white
    int direction    = (pawn_color == Color::WHITE) ? 8 : -8;
    U64 occupied     = board.get_all_pieces();

    // Single push
    U64 single_push = (from_square << direction) & ~occupied;
    if (single_push) {
        bool is_promoting = (pawn_color == Color::WHITE && y == 6) ||
                            (pawn_color == Color::BLACK && y == 1);
        vector<bitboard_move_t> push_moves = get_pawn_moves_from_possible_moves_bitboard(single_push, from_square, is_promoting);
        possible_moves.insert(possible_moves.end(), push_moves.begin(), push_moves.end());

        // Double push (only if single push is availible and pawn havent moved yet)
        if ((pawn_color == Color::WHITE && y == 1) ||
            (pawn_color == Color::BLACK && y == 6)) {
            U64 double_push = (single_push << direction) & ~occupied;
            if (double_push) {
                vector<bitboard_move_t> d_push_moves = get_pawn_moves_from_possible_moves_bitboard(double_push, from_square, false);
                possible_moves.insert(possible_moves.end(), d_push_moves.begin(), d_push_moves.end());
            }
        }
    }

    // Captures
    U64 enemy_pieces = board.get_all_friendly_pieces(!pawn_color);

    if (x > 0) { // Left capture
        U64 left_capture  = (pawn_color == Color::WHITE) ? (from_square << 7) : (from_square >> 9);
        U64 valid_capture = left_capture & enemy_pieces;
        if (valid_capture) {
            bool is_promoting = (pawn_color == Color::WHITE && y == 6) ||
                                (pawn_color == Color::BLACK && y == 1);
            vector<bitboard_move_t> capture_moves = get_pawn_moves_from_possible_moves_bitboard(valid_capture, from_square, is_promoting);
            possible_moves.insert(possible_moves.end(), capture_moves.begin(), capture_moves.end());
        }
    }

    if (x < 7) { // Right capture
        U64 right_capture = (pawn_color == Color::WHITE) ? (from_square << 9) : (from_square >> 7);
        U64 valid_capture = right_capture & enemy_pieces;
        if (valid_capture) {
            bool is_promoting = (pawn_color == Color::WHITE && y == 6) ||
                                (pawn_color == Color::BLACK && y == 1);
            vector<bitboard_move_t> capture_moves = get_pawn_moves_from_possible_moves_bitboard(valid_capture, from_square, is_promoting);
            possible_moves.insert(possible_moves.end(), capture_moves.begin(), capture_moves.end());
        }
    }

    // TODO: En passant captures
    // We'll need to:
    // 1. Check if en_passant_x/y are valid (using a bitboard for marking the square)
    // 2. Verify the capturing pawn is on the correct rank
    // 3. Generate the capture move
    // 4. Handle the removal of the captured pawn in make_move
    // 5. Update the board state

    return possible_moves;
}

vector<bitboard_move_t> get_king_moves(bitboard_t& board, int x, int y) {
    int pos         = y * 8 + x;
    U64 from_square = board.single_bitmask(pos);
    vector<bitboard_move_t> possible_moves;
    U64 raw_king_moves = king_attack_table[pos];
    Color king_color   = (board.board_w_K & from_square) ? Color::WHITE : Color::BLACK; // "is there a white king on the from_square?" if yes => color=white
    Color enemy_color  = !king_color;

    // Remove moves to squares with friendly pieces
    U64 potential_moves = raw_king_moves & ~board.get_all_friendly_pieces(king_color);

    // Check each possible move, that it's not moving into check
    while (potential_moves) {
        int move_pos  = __builtin_ctzll(potential_moves); // Get index of least significant 1-bit
        U64 to_square = 1ULL << move_pos;
        potential_moves &= (potential_moves - 1); // Clear the processed bit

        // If square is not under attack, add it as a valid move
        if (!is_square_under_attack(board, king_color, move_pos % 8, move_pos / 8)) {
            possible_moves.emplace_back(bitboard_move_t(from_square, to_square));
        }
    }

    // Castling moves
    if (!is_in_check(board, king_color)) {
        if (king_color == Color::WHITE) {
            if (board.white_king_side_castle) {
                U64 f1 = 1ULL << 5;
                U64 g1 = 1ULL << 6;
                if (!(board.get_all_pieces() & (f1 | g1)) &&
                    !is_square_under_attack(board, king_color, 5, 0) &&
                    !is_square_under_attack(board, king_color, 6, 0)) {
                    possible_moves.emplace_back(bitboard_move_t(from_square, g1));
                }
            }
            if (board.white_queen_side_castle) {
                U64 b1 = 1ULL << 1;
                U64 c1 = 1ULL << 2;
                U64 d1 = 1ULL << 3;
                if (!(board.get_all_pieces() & (b1 | c1 | d1)) &&
                    !is_square_under_attack(board, king_color, 2, 0) &&
                    !is_square_under_attack(board, king_color, 3, 0)) {
                    possible_moves.emplace_back(bitboard_move_t(from_square, c1));
                }
            }
        } else {
            if (board.black_king_side_castle) {
                U64 f8 = 1ULL << 61;
                U64 g8 = 1ULL << 62;
                if (!(board.get_all_pieces() & (f8 | g8)) &&
                    !is_square_under_attack(board, king_color, 5, 7) &&
                    !is_square_under_attack(board, king_color, 6, 7)) {
                    possible_moves.emplace_back(bitboard_move_t(from_square, g8));
                }
            }
            if (board.black_queen_side_castle) {
                U64 b8 = 1ULL << 57;
                U64 c8 = 1ULL << 58;
                U64 d8 = 1ULL << 59;
                if (!(board.get_all_pieces() & (b8 | c8 | d8)) &&
                    !is_square_under_attack(board, king_color, 2, 7) &&
                    !is_square_under_attack(board, king_color, 3, 7)) {
                    possible_moves.emplace_back(bitboard_move_t(from_square, c8));
                }
            }
        }
    }

    return possible_moves;
}

vector<bitboard_move_t> get_piece_moves(bitboard_t& board, int x, int y) {
    int pos         = y * 8 + x;
    U64 square_mask = 1ULL << pos;

    // Get piece type and color directly from bitboards
    if (board.board_w_P & square_mask)
        return get_pawn_moves(board, x, y);
    if (board.board_w_R & square_mask)
        return get_rook_moves(board, x, y);
    if (board.board_w_N & square_mask)
        return get_knight_moves(board, x, y);
    if (board.board_w_B & square_mask)
        return get_bishop_moves(board, x, y);
    if (board.board_w_Q & square_mask)
        return get_queen_moves(board, x, y);
    if (board.board_w_K & square_mask)
        return get_king_moves(board, x, y);
    if (board.board_b_P & square_mask)
        return get_pawn_moves(board, x, y);
    if (board.board_b_R & square_mask)
        return get_rook_moves(board, x, y);
    if (board.board_b_N & square_mask)
        return get_knight_moves(board, x, y);
    if (board.board_b_B & square_mask)
        return get_bishop_moves(board, x, y);
    if (board.board_b_Q & square_mask)
        return get_queen_moves(board, x, y);
    if (board.board_b_K & square_mask)
        return get_king_moves(board, x, y);

    return vector<bitboard_move_t>();
}

vector<bitboard_move_t> get_all_moves(bitboard_t& board) {
    vector<bitboard_move_t> all_moves;
    all_moves.reserve(218); // Pre-allocate space for efficiency
    // 218 is the best known upper bound for max moves in any position.
    // See https://lichess.org/analysis/fromPosition/R6R/3Q4/1Q4Q1/4Q3/2Q4Q/Q4Q2/pp1Q4/kBNN1KB1_w_-_-

    // Get all pieces on the board
    U64 pieces = board.get_all_pieces();

    // Iterate through all pieces
    while (pieces) {
        int square_idx = __builtin_ctzll(pieces); // Get index of least significant 1-bit
        int x          = square_idx % 8;
        int y          = square_idx / 8;

        vector<bitboard_move_t> piece_moves = get_piece_moves(board, x, y);

        // Check each move's legality
        Color piece_color = (board.get_all_friendly_pieces(Color::WHITE) & (1ULL << square_idx)) ? Color::WHITE : Color::BLACK;

        for (const auto& move : piece_moves) {
            piece_t captured = make_move(board, move);
            if (!is_in_check(board, piece_color)) {
                all_moves.push_back(move);
            }
            undo_move(board, move, captured);
        }

        pieces &= (pieces - 1); // Clear least significant 1-bit
    }

    return all_moves;
}

vector<bitboard_move_t> generate_all_moves_for_color(bitboard_t& board, Color color) {
    vector<bitboard_move_t> all_moves;
    all_moves.reserve(218); // Pre-allocate space for efficiency
    // 218 is the best known upper bound for max moves in any position.
    // See https://lichess.org/analysis/fromPosition/R6R/3Q4/1Q4Q1/4Q3/2Q4Q/Q4Q2/pp1Q4/kBNN1KB1_w_-_-

    // Get all pieces of the given color
    U64 pieces = color == Color::WHITE ? (board.board_w_P | board.board_w_N | board.board_w_B |
                                          board.board_w_R | board.board_w_Q | board.board_w_K)
                                       : (board.board_b_P | board.board_b_N | board.board_b_B |
                                          board.board_b_R | board.board_b_Q | board.board_b_K);

    // Iterate through pieces using bit scanning
    while (pieces) {
        int square_idx = __builtin_ctzll(pieces); // Get index of least significant 1-bit
        int x          = square_idx % 8;
        int y          = square_idx / 8;

        vector<bitboard_move_t> piece_moves = get_piece_moves(board, x, y);

        // Check move legality
        for (const auto& move : piece_moves) {
            piece_t captured = make_move(board, move);
            if (!is_in_check(board, color)) {
                all_moves.push_back(move);
            }
            undo_move(board, move, captured);
        }

        pieces &= (pieces - 1); // Clear least significant 1-bit
    }

    return all_moves;
}

} // namespace bitboard_moves

namespace coordinate_moves {

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

} // namespace coordinate_moves

#endif