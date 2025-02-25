// move_logic.hpp
#ifndef move_logic_hpp
#define move_logic_hpp

#include "board_t.hpp"
#include "move_t.hpp"
#include "hash.hpp"
#include <array>
#include <cmath>    //for absolute value
#include <stdint.h> //had to include this, otherwise didn't compile on my pc
#include <vector>

namespace moves {

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

// At compile time, these tables store all possible pawn attacks for each square
// 1 bits represent squares that can be attacked from the pawn's position
static const U64 PAWN_ATTACKS_WHITE[64] = {
    0x0000000000000200ULL, 0x0000000000000500ULL, 0x0000000000000A00ULL, 0x0000000000001400ULL,
    0x0000000000002800ULL, 0x0000000000005000ULL, 0x000000000000A000ULL, 0x0000000000004000ULL,
    0x0000000000020000ULL, 0x0000000000050000ULL, 0x00000000000A0000ULL, 0x0000000000140000ULL,
    0x0000000000280000ULL, 0x0000000000500000ULL, 0x0000000000A00000ULL, 0x0000000000400000ULL,
    0x0000000002000000ULL, 0x0000000005000000ULL, 0x000000000A000000ULL, 0x0000000014000000ULL,
    0x0000000028000000ULL, 0x0000000050000000ULL, 0x00000000A0000000ULL, 0x0000000040000000ULL,
    0x0000000200000000ULL, 0x0000000500000000ULL, 0x0000000A00000000ULL, 0x0000001400000000ULL,
    0x0000002800000000ULL, 0x0000005000000000ULL, 0x000000A000000000ULL, 0x0000004000000000ULL,
    0x0000020000000000ULL, 0x0000050000000000ULL, 0x00000A0000000000ULL, 0x0000140000000000ULL,
    0x0000280000000000ULL, 0x0000500000000000ULL, 0x0000A00000000000ULL, 0x0000400000000000ULL,
    0x0002000000000000ULL, 0x0005000000000000ULL, 0x000A000000000000ULL, 0x0014000000000000ULL,
    0x0028000000000000ULL, 0x0050000000000000ULL, 0x00A0000000000000ULL, 0x0040000000000000ULL,
    0x0200000000000000ULL, 0x0500000000000000ULL, 0x0A00000000000000ULL, 0x1400000000000000ULL,
    0x2800000000000000ULL, 0x5000000000000000ULL, 0xA000000000000000ULL, 0x4000000000000000ULL,
    0x0000000000000000ULL, 0x0000000000000000ULL, 0x0000000000000000ULL, 0x0000000000000000ULL,
    0x0000000000000000ULL, 0x0000000000000000ULL, 0x0000000000000000ULL, 0x0000000000000000ULL
};

static const U64 PAWN_ATTACKS_BLACK[64] = {
    0x0000000000000000ULL, 0x0000000000000000ULL, 0x0000000000000000ULL, 0x0000000000000000ULL,
    0x0000000000000000ULL, 0x0000000000000000ULL, 0x0000000000000000ULL, 0x0000000000000000ULL,
    0x0000000000000002ULL, 0x0000000000000005ULL, 0x000000000000000AULL, 0x0000000000000014ULL,
    0x0000000000000028ULL, 0x0000000000000050ULL, 0x00000000000000A0ULL, 0x0000000000000040ULL,
    0x0000000000000200ULL, 0x0000000000000500ULL, 0x0000000000000A00ULL, 0x0000000000001400ULL,
    0x0000000000002800ULL, 0x0000000000005000ULL, 0x000000000000A000ULL, 0x0000000000004000ULL,
    0x0000000000020000ULL, 0x0000000000050000ULL, 0x00000000000A0000ULL, 0x0000000000140000ULL,
    0x0000000000280000ULL, 0x0000000000500000ULL, 0x0000000000A00000ULL, 0x0000000000400000ULL,
    0x0000000002000000ULL, 0x0000000005000000ULL, 0x000000000A000000ULL, 0x0000000014000000ULL,
    0x0000000028000000ULL, 0x0000000050000000ULL, 0x00000000A0000000ULL, 0x0000000040000000ULL,
    0x0000000200000000ULL, 0x0000000500000000ULL, 0x0000000A00000000ULL, 0x0000001400000000ULL,
    0x0000002800000000ULL, 0x0000005000000000ULL, 0x000000A000000000ULL, 0x0000004000000000ULL,
    0x0000020000000000ULL, 0x0000050000000000ULL, 0x00000A0000000000ULL, 0x0000140000000000ULL,
    0x0000280000000000ULL, 0x0000500000000000ULL, 0x0000A00000000000ULL, 0x0000400000000000ULL,
    0x0002000000000000ULL, 0x0005000000000000ULL, 0x000A000000000000ULL, 0x0014000000000000ULL,
    0x0028000000000000ULL, 0x0050000000000000ULL, 0x00A0000000000000ULL, 0x0040000000000000ULL
};

static const U64 PAWN_PUSH_WHITE[64] = {
    0x0000000000000100ULL, 0x0000000000000200ULL, 0x0000000000000400ULL, 0x0000000000000800ULL,
    0x0000000000001000ULL, 0x0000000000002000ULL, 0x0000000000004000ULL, 0x0000000000008000ULL,
    0x0000000000010000ULL, 0x0000000000020000ULL, 0x0000000000040000ULL, 0x0000000000080000ULL,
    0x0000000000100000ULL, 0x0000000000200000ULL, 0x0000000000400000ULL, 0x0000000000800000ULL,
    0x0000000001000000ULL, 0x0000000002000000ULL, 0x0000000004000000ULL, 0x0000000008000000ULL,
    0x0000000010000000ULL, 0x0000000020000000ULL, 0x0000000040000000ULL, 0x0000000080000000ULL,
    0x0000000100000000ULL, 0x0000000200000000ULL, 0x0000000400000000ULL, 0x0000000800000000ULL,
    0x0000001000000000ULL, 0x0000002000000000ULL, 0x0000004000000000ULL, 0x0000008000000000ULL,
    0x0000010000000000ULL, 0x0000020000000000ULL, 0x0000040000000000ULL, 0x0000080000000000ULL,
    0x0000100000000000ULL, 0x0000200000000000ULL, 0x0000400000000000ULL, 0x0000800000000000ULL,
    0x0001000000000000ULL, 0x0002000000000000ULL, 0x0004000000000000ULL, 0x0008000000000000ULL,
    0x0010000000000000ULL, 0x0020000000000000ULL, 0x0040000000000000ULL, 0x0080000000000000ULL,
    0x0100000000000000ULL, 0x0200000000000000ULL, 0x0400000000000000ULL, 0x0800000000000000ULL,
    0x1000000000000000ULL, 0x2000000000000000ULL, 0x4000000000000000ULL, 0x8000000000000000ULL,
    0x0000000000000000ULL, 0x0000000000000000ULL, 0x0000000000000000ULL, 0x0000000000000000ULL,
    0x0000000000000000ULL, 0x0000000000000000ULL, 0x0000000000000000ULL, 0x0000000000000000ULL
};

static const U64 PAWN_PUSH_BLACK[64] = {
    0x0000000000000000ULL, 0x0000000000000000ULL, 0x0000000000000000ULL, 0x0000000000000000ULL,
    0x0000000000000000ULL, 0x0000000000000000ULL, 0x0000000000000000ULL, 0x0000000000000000ULL,
    0x0000000000000001ULL, 0x0000000000000002ULL, 0x0000000000000004ULL, 0x0000000000000008ULL,
    0x0000000000000010ULL, 0x0000000000000020ULL, 0x0000000000000040ULL, 0x0000000000000080ULL,
    0x0000000000000100ULL, 0x0000000000000200ULL, 0x0000000000000400ULL, 0x0000000000000800ULL,
    0x0000000000001000ULL, 0x0000000000002000ULL, 0x0000000000004000ULL, 0x0000000000008000ULL,
    0x0000000000010000ULL, 0x0000000000020000ULL, 0x0000000000040000ULL, 0x0000000000080000ULL,
    0x0000000000100000ULL, 0x0000000000200000ULL, 0x0000000000400000ULL, 0x0000000000800000ULL,
    0x0000000001000000ULL, 0x0000000002000000ULL, 0x0000000004000000ULL, 0x0000000008000000ULL,
    0x0000000010000000ULL, 0x0000000020000000ULL, 0x0000000040000000ULL, 0x0000000080000000ULL,
    0x0000000100000000ULL, 0x0000000200000000ULL, 0x0000000400000000ULL, 0x0000000800000000ULL,
    0x0000001000000000ULL, 0x0000002000000000ULL, 0x0000004000000000ULL, 0x0000008000000000ULL,
    0x0000010000000000ULL, 0x0000020000000000ULL, 0x0000040000000000ULL, 0x0000080000000000ULL,
    0x0000100000000000ULL, 0x0000200000000000ULL, 0x0000400000000000ULL, 0x0000800000000000ULL,
    0x0001000000000000ULL, 0x0002000000000000ULL, 0x0004000000000000ULL, 0x0008000000000000ULL,
    0x0010000000000000ULL, 0x0020000000000000ULL, 0x0040000000000000ULL, 0x0080000000000000ULL
};

void update_castling_rights(bitboard_t& board, const bitboard_move_t& move, int from_idx, int to_idx) {
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

void update_en_passant_square(bitboard_t& board, const bitboard_move_t& move, int from_idx, int to_idx) {
    board.en_passant_square = 0; // Always reset

    bool is_pawn               = (board.board_w_P & move.from_board) || (board.board_b_P & move.from_board);
    bool is_two_square_move    = abs(to_idx / 8 - from_idx / 8) == 2;
    bool same_file             = (from_idx % 8) == (to_idx % 8);
    bool is_from_starting_rank = (from_idx / 8 == 1 || from_idx / 8 == 6);

    if (is_pawn && is_two_square_move && same_file && is_from_starting_rank) {
        int ep_idx              = (from_idx + to_idx) / 2;
        board.en_passant_square = 1ULL << ep_idx;
    }
}

piece_t make_move(bitboard_t& board, const bitboard_move_t& move) {
    // Saving current state (pushing to the stack) before making any changes
    board.save_current_state();
    U64 hash = hash_t::compute_hash(board);
    board.position_hash_history.push_back(hash);

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

    // Update castling rights and en passant square
    update_castling_rights(board, move, from_idx, to_idx);
    update_en_passant_square(board, move, from_idx, to_idx);

    // Add move to move_history
    board.move_history.push_back(move);

    // Make the actual move (and handle promotion)
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
    return captured_piece;
}

void undo_move(bitboard_t& board, const bitboard_move_t& move, const piece_t& captured_piece) {
    // Get indices
    int from_idx = __builtin_ctzll(move.from_board);
    int to_idx   = __builtin_ctzll(move.to_board);


    board.move_history.pop_back();
    board.position_hash_history.pop_back();
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
        pawn_attacks = ((square << 7) & ~0x8080808080808080ULL) | // exclude h-file
                       ((square << 9) & ~0x0101010101010101ULL);  // exclude a-file
    } else {
        // Check if white pawns can attack this square
        pawn_attacks = ((square >> 7) & ~0x0101010101010101ULL) | // exclude h-file
                       ((square >> 9) & ~0x8080808080808080ULL);  // exclude a-file
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

move_list_t get_knight_moves(bitboard_t& board, int x, int y) {
    int pos            = y * 8 + x;
    U64 from_square    = board.single_bitmask(pos);
    U64 possible_moves = knight_attack_table[pos];                                      // get the possible moves table for the given square
    Color knight_color = (board.board_w_N & from_square) ? Color::WHITE : Color::BLACK; // "is one of the white knights on the from-square?" if yes then color=white
    possible_moves &= ~board.get_all_friendly_pieces(knight_color);                     // remove moves to squares occupied by friendly pieces
    return get_moves_from_possible_moves_bitboard(possible_moves, from_square);
}

move_list_t get_rook_moves(bitboard_t& board, int x, int y) {
    int pos         = y * 8 + x;
    U64 from_square = board.single_bitmask(pos);

    U64 occupied        = board.get_all_pieces();
    Color rook_color    = (board.board_w_R & from_square) ? Color::WHITE : Color::BLACK;
    U64 friendly_pieces = board.get_all_friendly_pieces(rook_color);

    U64 possible_moves = get_orthogonal_moves(occupied, friendly_pieces, pos);

    return get_moves_from_possible_moves_bitboard(possible_moves, from_square);
}

move_list_t get_bishop_moves(bitboard_t& board, int x, int y) {
    int pos         = y * 8 + x;
    U64 from_square = board.single_bitmask(pos);

    U64 occupied = board.get_all_pieces();

    // Friendly pieces to exclude
    Color bishop_color  = (board.board_w_B & from_square) ? Color::WHITE : Color::BLACK;
    U64 friendly_pieces = board.get_all_friendly_pieces(bishop_color);

    U64 possible_moves = get_diagonal_moves(occupied, friendly_pieces, pos);

    return get_moves_from_possible_moves_bitboard(possible_moves, from_square);
}

move_list_t get_queen_moves(bitboard_t& board, int x, int y) {
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

inline move_list_t get_pawn_moves(bitboard_t& board, int x, int y) {
    const int pos = y * 8 + x;
    const U64 from_square = 1ULL << pos;
    move_list_t moves;
    
    // Determine pawn color and relevant constants
    const bool is_white = board.board_w_P & from_square;
    const U64 occupied = board.get_all_pieces();
    const U64 enemy_pieces = board.get_all_friendly_pieces(is_white ? Color::BLACK : Color::WHITE);
    
    // Get attacks using lookup tables
    const U64 attacks = is_white ? PAWN_ATTACKS_WHITE[pos] : PAWN_ATTACKS_BLACK[pos];
    const U64 valid_captures = attacks & enemy_pieces;
    
    // Handle captures
    if (valid_captures) {
        const bool is_promoting = (is_white && y == 6) || (!is_white && y == 1);
        U64 captures = valid_captures;
        while (captures) {
            const int to_idx = __builtin_ctzll(captures);
            const U64 to_square = 1ULL << to_idx;
            
            if (is_promoting) {
                // Add all promotion captures directly
                moves.add({from_square, to_square, PieceType::QUEEN});
                moves.add({from_square, to_square, PieceType::ROOK});
                moves.add({from_square, to_square, PieceType::BISHOP});
                moves.add({from_square, to_square, PieceType::KNIGHT});
            } else {
                moves.add({from_square, to_square});
            }
            captures &= captures - 1;  // Clear LSB
        }
    }
    
    // Handle pushes
    const U64 push_pattern = is_white ? PAWN_PUSH_WHITE[pos] : PAWN_PUSH_BLACK[pos];
    U64 single_push = push_pattern & ~occupied;
    
    if (single_push) {
        const bool is_promoting = (is_white && y == 6) || (!is_white && y == 1);
        if (is_promoting) {
            // Add all promotion pushes directly
            moves.add({from_square, single_push, PieceType::QUEEN});
            moves.add({from_square, single_push, PieceType::ROOK});
            moves.add({from_square, single_push, PieceType::BISHOP});
            moves.add({from_square, single_push, PieceType::KNIGHT});
        } else {
            moves.add({from_square, single_push});
            
            // Double push logic
            if ((is_white && y == 1) || (!is_white && y == 6)) {
                const U64 double_push = is_white ? 
                    (single_push << 8) & ~occupied :
                    (single_push >> 8) & ~occupied;
                    
                if (double_push) {
                    moves.add({from_square, double_push});
                }
            }
        }
    }
    
    // Handle en passant
    if (board.en_passant_square && (y == (is_white ? 4 : 3))) {
        const U64 ep_attacks = attacks & board.en_passant_square;
        if (ep_attacks) {
            moves.add({from_square, board.en_passant_square});
        }
    }
    
    return moves;
}


move_list_t get_king_moves(bitboard_t& board, int x, int y) {
    int pos         = y * 8 + x;
    U64 from_square = board.single_bitmask(pos);
    move_list_t possible_moves;
    U64 raw_king_moves = king_attack_table[pos];
    Color king_color   = (board.board_w_K & from_square) ? Color::WHITE : Color::BLACK; // "is there a white king on the from_square?" if yes => color=white
    Color enemy_color  = !king_color;

    // Remove moves to squares with friendly pieces
    U64 potential_moves = raw_king_moves & ~board.get_all_friendly_pieces(king_color);

    // Check each possible move, that is not moving into check
    while (potential_moves) {
        int move_pos  = __builtin_ctzll(potential_moves); // Get index of least significant 1-bit
        U64 to_square = 1ULL << move_pos;
        potential_moves &= (potential_moves - 1); // Clear the processed bit

        // If square is not under attack, add it as a valid move
        if (!is_square_under_attack(board, king_color, move_pos % 8, move_pos / 8)) {
            possible_moves.add(bitboard_move_t(from_square, to_square));
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
                    possible_moves.add(bitboard_move_t(from_square, g1));
                }
            }
            if (board.white_queen_side_castle) {
                U64 b1 = 1ULL << 1;
                U64 c1 = 1ULL << 2;
                U64 d1 = 1ULL << 3;
                if (!(board.get_all_pieces() & (b1 | c1 | d1)) &&
                    !is_square_under_attack(board, king_color, 2, 0) &&
                    !is_square_under_attack(board, king_color, 3, 0)) {
                    possible_moves.add(bitboard_move_t(from_square, c1));
                }
            }
        } else {
            if (board.black_king_side_castle) {
                U64 f8 = 1ULL << 61;
                U64 g8 = 1ULL << 62;
                if (!(board.get_all_pieces() & (f8 | g8)) &&
                    !is_square_under_attack(board, king_color, 5, 7) &&
                    !is_square_under_attack(board, king_color, 6, 7)) {
                    possible_moves.add(bitboard_move_t(from_square, g8));
                }
            }
            if (board.black_queen_side_castle) {
                U64 b8 = 1ULL << 57;
                U64 c8 = 1ULL << 58;
                U64 d8 = 1ULL << 59;
                if (!(board.get_all_pieces() & (b8 | c8 | d8)) &&
                    !is_square_under_attack(board, king_color, 2, 7) &&
                    !is_square_under_attack(board, king_color, 3, 7)) {
                    possible_moves.add(bitboard_move_t(from_square, c8));
                }
            }
        }
    }

    return possible_moves;
}

move_list_t get_piece_moves(bitboard_t& board, int x, int y) {
    int pos         = y * 8 + x;
    U64 square_mask = 1ULL << pos;

    // Get piece type and color directly from bitboards
    if ((board.board_w_P | board.board_b_P) & square_mask)
        return get_pawn_moves(board, x, y);
    if ((board.board_w_R | board.board_b_R) & square_mask)
        return get_rook_moves(board, x, y);
    if ((board.board_w_N | board.board_b_N) & square_mask)
        return get_knight_moves(board, x, y);
    if ((board.board_w_B | board.board_b_B) & square_mask)
        return get_bishop_moves(board, x, y);
    if ((board.board_w_Q | board.board_b_Q) & square_mask)
        return get_queen_moves(board, x, y);
    if ((board.board_w_K | board.board_b_K) & square_mask)
        return get_king_moves(board, x, y);

    return move_list_t();
}

move_list_t get_all_moves(bitboard_t& board) {
    move_list_t all_moves;

    // Get all pieces on the board
    U64 pieces = board.get_all_pieces();

    // Iterate through all pieces
    while (pieces) {
        int square_idx = __builtin_ctzll(pieces); // Get index of least significant 1-bit
        int x          = square_idx % 8;
        int y          = square_idx / 8;

        move_list_t piece_moves = get_piece_moves(board, x, y);

        // Check each move's legality
        Color piece_color = (board.get_all_friendly_pieces(Color::WHITE) & (1ULL << square_idx)) ? Color::WHITE : Color::BLACK;

        for (const auto& move : piece_moves.moves) {
            piece_t captured = make_move(board, move);
            if (!is_in_check(board, piece_color)) {
                all_moves.add(move);
            }
            undo_move(board, move, captured);
        }

        pieces &= (pieces - 1); // Clear least significant 1-bit
    }

    return all_moves;
}

move_list_t generate_all_moves_for_color(bitboard_t& board, Color color) {
    move_list_t all_moves;
    
    // Get all pieces of the given color
    U64 pieces = board.get_all_friendly_pieces(color);

    while (pieces) {
        int square_idx = __builtin_ctzll(pieces);
        int x = square_idx % 8;
        int y = square_idx / 8;

        move_list_t piece_moves = get_piece_moves(board, x, y);

        // Check move legality
        for (int i = 0; i < piece_moves.count; i++) {
            piece_t captured = moves::make_move(board, piece_moves.moves[i]);
            if (!is_in_check(board, color)) {
                all_moves.add(piece_moves.moves[i]);
            }
            undo_move(board, piece_moves.moves[i], captured);
        }

        pieces &= (pieces - 1);
    }

    return all_moves;
}

} // namespace moves

#endif