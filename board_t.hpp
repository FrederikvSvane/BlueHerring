#ifndef board_t_hpp
#define board_t_hpp
#include "move_t.hpp"
#include "operators_util.hpp"
#include "piece_t.hpp"
#include "square_t.hpp"
#include "string_util.hpp"
#include <array>
#include <iostream>

using namespace std;

struct bitboard_state {
    bool white_king_side_castle;
    bool white_queen_side_castle;
    bool black_king_side_castle;
    bool black_queen_side_castle;
    U64 en_passant_square;
};

struct bitboard_t {
    // LSB represents the square A1, MSB represents H8

    // Define the bitboards for white and black, for each piece type
    U64 board_w_K;
    U64 board_w_Q;
    U64 board_w_P;
    U64 board_w_N;
    U64 board_w_B;
    U64 board_w_R;

    U64 board_b_K;
    U64 board_b_Q;
    U64 board_b_P;
    U64 board_b_N;
    U64 board_b_B;
    U64 board_b_R;

    // The idea here is to have a move table that contains a bitmask for the given square. This bitmask contains the
    // legal moves of the piece in that square. So when a player moves, we only need to update a few tables, rather
    // than recomputing all the legal moves.
    U64 move_table_white[64] = {};
    U64 move_table_black[64] = {};

    vector<bitboard_move_t> move_history;
    vector<bitboard_state> state_history;
    vector<U64> position_hash_history;
    Color active_color;

    // Current state
    bool white_king_side_castle;
    bool white_queen_side_castle;
    bool black_king_side_castle;
    bool black_queen_side_castle;
    U64 en_passant_square;

    bitboard_t() {
        // Initialize all bitboards to 0 (if desired, call initialize_starting_board to set the pieces)
        board_w_K = board_w_Q = board_w_P = board_w_N = board_w_B = board_w_R = 0;
        board_b_K = board_b_Q = board_b_P = board_b_N = board_b_B = board_b_R = 0;

        // Initialize castling rights
        white_king_side_castle  = true;
        white_queen_side_castle = true;
        black_king_side_castle  = true;
        black_queen_side_castle = true;

        // Initialize en passant (no target square initially)
        U64 en_passant_square = 0ULL;
    }

    inline U64 single_bitmask(int square_idx) const { // Pass an index (0-63) and convert to bitmask
        return 1ULL << square_idx;
    }

    void save_current_state() {
        bitboard_state current_state = {
            white_king_side_castle,
            white_queen_side_castle,
            black_king_side_castle,
            black_queen_side_castle,
            en_passant_square};
        state_history.push_back(current_state);
    }

    void restore_previous_state() {
        if (state_history.empty()) {
            throw std::runtime_error("No state to restore");
        }

        bitboard_state previous_state = state_history.back();
        state_history.pop_back();

        white_king_side_castle  = previous_state.white_king_side_castle;
        white_queen_side_castle = previous_state.white_queen_side_castle;
        black_king_side_castle  = previous_state.black_king_side_castle;
        black_queen_side_castle = previous_state.black_queen_side_castle;
        en_passant_square       = previous_state.en_passant_square;
    }

    // Just for debugging
    void print_state() const {
        std::cout << "Castling rights:\n";
        std::cout << "  White: " << (white_king_side_castle ? "K" : "")
                  << (white_queen_side_castle ? "Q" : "") << "\n";
        std::cout << "  Black: " << (black_king_side_castle ? "k" : "")
                  << (black_queen_side_castle ? "q" : "") << "\n";
        std::cout << "En passant target: ";
        if (en_passant_square) {
            int square = __builtin_ctzll(en_passant_square);
            std::cout << char('a' + (square % 8)) << (square / 8 + 1);
        } else {
            std::cout << "-";
        }
        std::cout << "\n";
    }

    // same for this
    void print_state_history() const {
        std::cout << "\nState History (oldest to current):\n";
        for (size_t i = 0; i < state_history.size(); ++i) {
            const auto& state = state_history[i];
            std::cout << "State " << i << ":\n";
            std::cout << "  Castling: ";
            std::cout << (state.white_king_side_castle ? "K" : "")
                      << (state.white_queen_side_castle ? "Q" : "")
                      << (state.black_king_side_castle ? "k" : "")
                      << (state.black_queen_side_castle ? "q" : "") << "\n";
            std::cout << "  En passant: ";
            if (state.en_passant_square) {
                int square = __builtin_ctzll(state.en_passant_square);
                std::cout << char('a' + (square % 8)) << (square / 8 + 1);
            } else {
                std::cout << "-";
            }
            std::cout << "\n";
        }
        std::cout << "\n";
    }

    void pretty_print_board() {
        cout << endl;
        cout << "  a b c d e f g h" << endl;
        for (int y = 7; y >= 0; y--) {
            // cout << y + 1 << " ";
            cout << y << " ";
            for (int x = 0; x < 8; x++) {
                cout << at(x, y) << " ";
            }
            cout << endl;
        }
        cout << "\n"
             << endl;
    }

    // Just for debugging
    void print_bitboard(U64 board) {
        for (int rank = 7; rank >= 0; --rank) { // Chess ranks (8 to 1, top to bottom)
            std::cout << "  ";

            for (int file = 0; file < 8; ++file) { // Chess files (a to h, left to right)
                int square = rank * 8 + file;
                U64 mask   = 1ULL << square; // Create a mask for the square
                std::cout << ((board & mask) ? '1' : '0') << " ";
            }
            std::cout << std::endl;
        }
    }

    square_t at(int x, int y) const {
        int bit = y * 8 + x;

        if (bit < 0 || bit >= 64) {
            throw std::out_of_range("Bit index must be in range [0, 63].");
        }

        U64 pos = single_bitmask(bit); // Convert index to bitboard

        // White
        if (board_w_P & pos)
            return {bit % 8, bit / 8, false, {PieceType::PAWN, Color::WHITE}};
        if (board_w_N & pos)
            return {bit % 8, bit / 8, false, {PieceType::KNIGHT, Color::WHITE}};
        if (board_w_B & pos)
            return {bit % 8, bit / 8, false, {PieceType::BISHOP, Color::WHITE}};
        if (board_w_R & pos)
            return {bit % 8, bit / 8, false, {PieceType::ROOK, Color::WHITE}};
        if (board_w_Q & pos)
            return {bit % 8, bit / 8, false, {PieceType::QUEEN, Color::WHITE}};
        if (board_w_K & pos)
            return {bit % 8, bit / 8, false, {PieceType::KING, Color::WHITE}};

        // Black
        if (board_b_P & pos)
            return {bit % 8, bit / 8, false, {PieceType::PAWN, Color::BLACK}};
        if (board_b_N & pos)
            return {bit % 8, bit / 8, false, {PieceType::KNIGHT, Color::BLACK}};
        if (board_b_B & pos)
            return {bit % 8, bit / 8, false, {PieceType::BISHOP, Color::BLACK}};
        if (board_b_R & pos)
            return {bit % 8, bit / 8, false, {PieceType::ROOK, Color::BLACK}};
        if (board_b_Q & pos)
            return {bit % 8, bit / 8, false, {PieceType::QUEEN, Color::BLACK}};
        if (board_b_K & pos)
            return {bit % 8, bit / 8, false, {PieceType::KING, Color::BLACK}};

        // No piece is found, return empty square
        return {bit % 8, bit / 8, false, {PieceType::EMPTY, Color::NONE}};
    }

    U64* get_board_for_piece(PieceType type, Color color) {
        if (color == Color::WHITE) {
            if (type == PieceType::PAWN)
                return &board_w_P;
            else if (type == PieceType::KNIGHT)
                return &board_w_N;
            else if (type == PieceType::BISHOP)
                return &board_w_B;
            else if (type == PieceType::ROOK)
                return &board_w_R;
            else if (type == PieceType::QUEEN)
                return &board_w_Q;
            else if (type == PieceType::KING)
                return &board_w_K;
        } else {
            if (type == PieceType::PAWN)
                return &board_b_P;
            else if (type == PieceType::KNIGHT)
                return &board_b_N;
            else if (type == PieceType::BISHOP)
                return &board_b_B;
            else if (type == PieceType::ROOK)
                return &board_b_R;
            else if (type == PieceType::QUEEN)
                return &board_b_Q;
            else if (type == PieceType::KING)
                return &board_b_K;
        }
        throw invalid_argument("Invalid piece type and color combination.");
    }

    U64* move_bit(U64* board, int from_idx, int to_idx) {

        // Create bitboards for the source and destination squares
        U64 from_bitmask = single_bitmask(from_idx);
        U64 to_bitmask   = single_bitmask(to_idx);

        *board &= ~from_bitmask; // Clears the source bit
        *board |= to_bitmask;    // Sets the destination bit

        return board;
    }

    // A board with ones wherever the given player has a piece
    U64 get_all_friendly_pieces(Color color) const {
        U64 friendly_pieces = (board_w_P | board_w_N | board_w_B |
                               board_w_R | board_w_Q | board_w_K);

        if (color == Color::BLACK) {
            friendly_pieces = (board_b_P | board_b_N | board_b_B |
                               board_b_R | board_b_Q | board_b_K);
        }

        return friendly_pieces;
    }

    // A board with all occupied squares
    U64 get_all_pieces() const {
        return (board_w_P | board_w_N | board_w_B | board_w_R | board_w_Q | board_w_K |
                board_b_P | board_b_N | board_b_B | board_b_R | board_b_Q | board_b_K);
    }

    void initialize_starting_board() {
        // White
        board_w_P = 0x000000000000FF00ULL;
        board_w_R = 0x0000000000000081ULL;
        board_w_N = 0x0000000000000042ULL;
        board_w_B = 0x0000000000000024ULL;
        board_w_Q = 0x0000000000000008ULL;
        board_w_K = 0x0000000000000010ULL;

        // Black
        board_b_P = 0x00FF000000000000ULL;
        board_b_R = 0x8100000000000000ULL;
        board_b_N = 0x4200000000000000ULL;
        board_b_B = 0x2400000000000000ULL;
        board_b_Q = 0x0800000000000000ULL;
        board_b_K = 0x1000000000000000ULL;
    }

    void initialize_board_from_fen(const string& fen) {
        // Reset all bitboards to 0
        board_w_P = board_w_N = board_w_B = board_w_R = board_w_Q = board_w_K = 0ULL;
        board_b_P = board_b_N = board_b_B = board_b_R = board_b_Q = board_b_K = 0ULL;

        vector<string> fen_parts = split(fen, ' ');
        const string& position   = fen_parts[0];

        int x = 0, y = 7; // start from top row
        for (char c : position) {
            if (c == '/') {
                x = 0;
                y--;
                continue;
            }

            if (isdigit(c)) {
                x += c - '0';
                continue;
            }

            // Calculate bit position
            int bit_pos  = y * 8 + x;
            U64 bit_mask = 1ULL << bit_pos;

            // Set the appropriate bit in the corresponding bitboard
            if (isupper(c)) { // White pieces
                switch (c) {
                case 'P': board_w_P |= bit_mask; break;
                case 'N': board_w_N |= bit_mask; break;
                case 'B': board_w_B |= bit_mask; break;
                case 'R': board_w_R |= bit_mask; break;
                case 'Q': board_w_Q |= bit_mask; break;
                case 'K': board_w_K |= bit_mask; break;
                }
            } else { // Black pieces
                switch (tolower(c)) {
                case 'p': board_b_P |= bit_mask; break;
                case 'n': board_b_N |= bit_mask; break;
                case 'b': board_b_B |= bit_mask; break;
                case 'r': board_b_R |= bit_mask; break;
                case 'q': board_b_Q |= bit_mask; break;
                case 'k': board_b_K |= bit_mask; break;
                }
            }
            x++;
        }

        // Reset castling rights first
        white_king_side_castle  = false;
        white_queen_side_castle = false;
        black_king_side_castle  = false;
        black_queen_side_castle = false;

        // Parse active color (second field)
        if (fen_parts.size() > 1) {
            active_color = (fen_parts[1] == "w") ? Color::WHITE : Color::BLACK;
        } else {
            active_color = Color::WHITE;
        }

        if (fen_parts.size() > 2) {
            // Set castling rights based on FEN
            string castling = fen_parts[2];
            if (castling != "-") {
                white_king_side_castle  = castling.find('K') != string::npos;
                white_queen_side_castle = castling.find('Q') != string::npos;
                black_king_side_castle  = castling.find('k') != string::npos;
                black_queen_side_castle = castling.find('q') != string::npos;
            }

            // Set en passant square if exists
            if (fen_parts.size() > 3 && fen_parts[3] != "-") {
                int ep_x          = fen_parts[3][0] - 'a';
                int ep_y          = fen_parts[3][1] - '1';
                en_passant_square = 1ULL << (ep_y * 8 + ep_x);
            } else {
                en_passant_square = 0ULL;
            }
        }
    }

    bool is_path_clear(U64 path_mask, U64 occupied_squares) const {
        // If any bit in path_mask is set in occupied_squares, the path is blocked
        return !(path_mask & occupied_squares);
    }

    U64 get_path_mask(int from_idx, int to_idx) const {
        U64 path = 0ULL;

        int from_row = from_idx / 8;
        int from_col = from_idx % 8;
        int to_row   = to_idx / 8;
        int to_col   = to_idx % 8;

        // Check if move is valid (straight or diagonal)
        int row_diff = abs(to_row - from_row);
        int col_diff = abs(to_col - from_col);

        // This check ensures the move is either:
        // - Straight (one diff is 0) OR
        // - Diagonal (both diffs are equal)
        if (row_diff != 0 && col_diff != 0 && row_diff != col_diff) {
            return 0ULL; // invalid path - not straight or diagonal (ie. only knights return 0ULL)
        }

        // Calculate step direction (-1, 0, or 1 for each direction)
        // If moving up: (8-5)/3 = 1
        // If moving down: (2-5)/3 = -1
        // If same row: (5-5)/1 = 0
        int row_step = (from_row == to_row) ? 0 : (to_row - from_row) / row_diff;
        int col_step = (from_col == to_col) ? 0 : (to_col - from_col) / col_diff;

        // Start from square after 'from' position
        int current_row = from_row + row_step;
        int current_col = from_col + col_step;

        // Add each intermediate square to path until we reach destination
        while (current_row != to_row || current_col != to_col) {
            path |= 1ULL << (current_row * 8 + current_col);
            current_row += row_step;
            current_col += col_step;
        }

        return path;
    }

    bool is_move_legal_basic(const bitboard_move_t& move) const {
        int from_idx = __builtin_ctzll(move.from_board); // Gets index of least significant 1-bit
        int to_idx   = __builtin_ctzll(move.to_board);

        // Get the piece at the source square
        square_t from_square = at((from_idx % 8), (from_idx / 8));

        // Knights don't need path checking
        if (from_square.piece.type == PieceType::KNIGHT) {
            return true;
        }

        // For all other pieces:
        U64 path = get_path_mask(from_idx, to_idx);

        // If path is 0 (invalid) and piece isn't a knight, move is illegal
        if (path == 0ULL && from_square.piece.type != PieceType::KNIGHT) {
            return false;
        }

        // Check if path is clear of any pieces
        return is_path_clear(path, get_all_pieces());
    }
};

#endif