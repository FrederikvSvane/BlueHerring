#ifndef hash_hpp
#define hash_hpp

#include "board_t.hpp"
#include <random>

class hash_t {
  private:
    struct position_keys_t {
        U64 piece_square[12][64]; // 12 piece types (6 pieces * 2 colors) * 64 squares
        U64 castling_rights[16];  // 2^4 possible castling combinations
        U64 en_passant[64];       // En passant square
        U64 side_to_move;         // Who's turn it is

        position_keys_t() {
            std::random_device rd;
            std::mt19937_64 eng(rd());
            std::uniform_int_distribution<U64> distr;

            // Initialize piece-square keys
            for (int piece = 0; piece < 12; piece++) {
                for (int square = 0; square < 64; square++) {
                    piece_square[piece][square] = distr(eng);
                }
            }

            // Initialize castling keys
            for (int i = 0; i < 16; i++) {
                castling_rights[i] = distr(eng);
            }

            // Initialize en passant keys
            for (int square = 0; square < 64; square++) {
                en_passant[square] = distr(eng);
            }

            side_to_move = distr(eng);
        }
    };

    static position_keys_t keys;

  public:
    static U64 compute_hash(const bitboard_t& board) {
        U64 hash       = 0;
        U64 all_pieces = board.get_all_pieces();

        while (all_pieces) {
            int square      = __builtin_ctzll(all_pieces);
            U64 square_mask = 1ULL << square;
            if (board.board_w_K & square_mask) {
                hash ^= keys.piece_square[0][square];
            } else if (board.board_w_Q & square_mask) {
                hash ^= keys.piece_square[1][square];
            } else if (board.board_w_R & square_mask) {
                hash ^= keys.piece_square[2][square];
            } else if (board.board_w_B & square_mask) {
                hash ^= keys.piece_square[3][square];
            } else if (board.board_w_N & square_mask) {
                hash ^= keys.piece_square[4][square];
            } else if (board.board_w_P & square_mask) {
                hash ^= keys.piece_square[5][square];
            } else if (board.board_b_K & square_mask) {
                hash ^= keys.piece_square[6][square];
            } else if (board.board_b_Q & square_mask) {
                hash ^= keys.piece_square[7][square];
            } else if (board.board_b_R & square_mask) {
                hash ^= keys.piece_square[8][square];
            } else if (board.board_b_B & square_mask) {
                hash ^= keys.piece_square[9][square];
            } else if (board.board_b_N & square_mask) {
                hash ^= keys.piece_square[10][square];
            } else if (board.board_b_P & square_mask) {
                hash ^= keys.piece_square[11][square];
            }
            all_pieces &= (all_pieces - 1); // Clear least significant bit
        }

        // Hash castling rights
        int castling = 0;
        if (board.white_king_side_castle)
            castling |= 1;
        if (board.white_queen_side_castle)
            castling |= 2;
        if (board.black_king_side_castle)
            castling |= 4;
        if (board.black_queen_side_castle)
            castling |= 8;
        hash ^= keys.castling_rights[castling];

        // Hash en passant square
        if (board.en_passant_square) {
            int square = __builtin_ctzll(board.en_passant_square);
            hash ^= keys.en_passant[square];
        }

        // Hash side to move
        if (board.active_color == Color::BLACK) {
            hash ^= keys.side_to_move;
        }

        return hash;
    }

    static bool is_threefold_repetition(const bitboard_t& board) {
        U64 current_hash = compute_hash(board);
        int count        = 0;

        // Check history for same position
        for (const U64& hash : board.position_hash_history) {
            if (hash == current_hash) {
                count++;
                if (count >= 2) { // Current position + 2 previous = 3 occurrences
                    return true;
                }
            }
        }

        return false;
    }
};

// Define the static member for use in other header files
hash_t::position_keys_t hash_t::keys;

#endif