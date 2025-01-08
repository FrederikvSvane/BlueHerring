#ifndef board_t_hpp
#define board_t_hpp
#include "move_t.hpp"
#include "operators_util.hpp"
#include "piece_t.hpp"
#include "square_t.hpp"
#include "string_util.hpp"
#include <array>
#include <iostream>

// This is the datatype of the bitboard
using U64 = unsigned long long;

using namespace std;

struct board_state {
    bool white_king_side_castle;
    bool white_queen_side_castle;
    bool black_king_side_castle;
    bool black_queen_side_castle;
    int en_passant_x;
    int en_passant_y;
};

struct board_t {
    array<square_t, 64> board;
    vector<move_t> history;
    vector<board_state> state_history; // Track historical states
    Color active_color;                // whose turn it is (used for testing)

    // Current state
    bool white_king_side_castle;
    bool white_queen_side_castle;
    bool black_king_side_castle;
    bool black_queen_side_castle;
    int en_passant_x;
    int en_passant_y;

    board_t() {
        for (int x = 0; x < 8; x++) {
            for (int y = 0; y < 8; y++) {
                at(x, y) = square_t();
            }
            active_color = Color::WHITE;
        }
        // Initialize castling rights
        white_king_side_castle  = true;
        white_queen_side_castle = true;
        black_king_side_castle  = true;
        black_queen_side_castle = true;

        // Initialize en passant (no target square initially)
        en_passant_x = -1;
        en_passant_y = -1;
    }

    static constexpr inline int index(int x, int y) {
        return static_cast<int>(x + (y << 3)); // same as x + y*8 (but faster!)
    }

    square_t& at(int x, int y) {
        return board[index(x, y)];
    }

    const square_t& at(int x, int y) const {
        return board[index(x, y)];
    }

    int in_board(int x, int y) const {
        return 0 <= x && x <= 7 && 0 <= y && y <= 7;
    }

    void initialize_starting_board() {
        for (int x = 0; x < 8; x++) {
            for (int y = 0; y < 8; y++) {
                // initialize coordinates of all square_t
                at(x, y).x = x;
                at(x, y).y = y;

                // initialize pieces
                if (y == 0 || y == 1) {
                    if (y == 1) {
                        at(x, y).piece = piece_t(PieceType::PAWN, Color::WHITE);
                    } else if (x == 0 || x == 7) {
                        at(x, y).piece = piece_t(PieceType::ROOK, Color::WHITE);
                    } else if (x == 1 || x == 6) {
                        at(x, y).piece = piece_t(PieceType::KNIGHT, Color::WHITE);
                    } else if (x == 2 || x == 5) {
                        at(x, y).piece = piece_t(PieceType::BISHOP, Color::WHITE);
                    } else if (x == 3) {
                        at(x, y).piece = piece_t(PieceType::QUEEN, Color::WHITE);
                    } else {
                        at(x, y).piece = piece_t(PieceType::KING, Color::WHITE);
                    }
                } else if (y == 6 || y == 7) {
                    if (y == 6) {
                        at(x, y).piece = piece_t(PieceType::PAWN, Color::BLACK);
                    } else if (x == 0 || x == 7) {
                        at(x, y).piece = piece_t(PieceType::ROOK, Color::BLACK);
                    } else if (x == 1 || x == 6) {
                        at(x, y).piece = piece_t(PieceType::KNIGHT, Color::BLACK);
                    } else if (x == 2 || x == 5) {
                        at(x, y).piece = piece_t(PieceType::BISHOP, Color::BLACK);
                    } else if (x == 3) {
                        at(x, y).piece = piece_t(PieceType::QUEEN, Color::BLACK);
                    } else {
                        at(x, y).piece = piece_t(PieceType::KING, Color::BLACK);
                    }
                } else {
                    at(x, y).piece = piece_t(PieceType::EMPTY, Color::NONE);
                }
            }
        }
    }

    void initialize_board_from_fen(const string& fen) {
        // First initialize all squares with coordinates and empty pieces
        for (int x = 0; x < 8; x++) {
            for (int y = 0; y < 8; y++) {
                at(x, y).x     = x;
                at(x, y).y     = y;
                at(x, y).piece = piece_t(PieceType::EMPTY, Color::NONE);
            }
        }

        vector<string> fen_parts = split(fen, ' ');

        const string& position = fen_parts[0]; // Only use the first part for piece placement
        int x = 0, y = 7;                      // start from top row
        for (char c : position) {              // Only parse the piece placement section
            if (c == '/') {
                x = 0;
                y--;
                continue;
            }

            if (isdigit(c)) {
                x += c - '0';
                continue;
            }

            Color color = isupper(c) ? Color::WHITE : Color::BLACK;
            char piece  = toupper(c);

            PieceType type;
            switch (piece) {
            case 'P': type = PieceType::PAWN; break;
            case 'N': type = PieceType::KNIGHT; break;
            case 'B': type = PieceType::BISHOP; break;
            case 'R': type = PieceType::ROOK; break;
            case 'Q': type = PieceType::QUEEN; break;
            case 'K': type = PieceType::KING; break;
            default: continue;
            }

            at(x, y).piece = piece_t(type, color);
            at(x, y).x     = x;
            at(x, y).y     = y;
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
            active_color = Color::WHITE; // Default to white if not specified
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
                en_passant_x = fen_parts[3][0] - 'a';
                en_passant_y = fen_parts[3][1] - '1';
            } else {
                en_passant_x = -1;
                en_passant_y = -1;
            }
        }
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

    string format_castling(bool king_side, bool queen_side) const {
        if (!king_side && !queen_side)
            return "None";
        string result;
        if (queen_side)
            result += "Q";
        if (king_side)
            result += "K";
        return result;
    }

    string format_en_passant(int x, int y) const {
        if (x == -1 || y == -1)
            return "None";
        return string(1, "abcdefgh"[x]) + to_string(y + 1);
    }

    void print_state(const board_state& state, int index) const {
        cout << index << " = {wc: "
             << format_castling(state.white_king_side_castle, state.white_queen_side_castle)
             << ", bc: "
             << format_castling(state.black_king_side_castle, state.black_queen_side_castle)
             << ", en passant square: "
             << format_en_passant(state.en_passant_x, state.en_passant_y)
             << "}\n";
    }

    void print_state_history() const {
        cout << "\nState History Stack (oldest to current):\n";
        size_t i = 0;
        for (; i < state_history.size(); ++i) {
            print_state(state_history[i], i);
        }
        cout << i << " = {wc: "
             << format_castling(white_king_side_castle, white_queen_side_castle)
             << ", bc: "
             << format_castling(black_king_side_castle, black_queen_side_castle)
             << ", en passant square: "
             << format_en_passant(en_passant_x, en_passant_y)
             << "}\n\n";
    }

    class board_iterator_t { // to facilitate looping over the board, square by square, starting from the bottom left
        array<square_t, 64>& board;
        int x = 0;
        int y = 0;

      public:
        board_iterator_t(array<square_t, 64>& b, int startX = 0, int startY = 0)
            : board(b), x(startX), y(startY) {}

        board_iterator_t& operator++() {
            if (++y >= 8) {
                y = 0;
                ++x;
            }
            return *this;
        }

        square_t& operator*() {
            return board[index(x, y)];
        }

        bool operator!=(const board_iterator_t& other) const {
            return x != other.x || y != other.y;
        }
    };

    class const_board_iterator_t { // to facilitate looping over the board, without modifying anything
        const array<square_t, 64>& board;
        int x = 0;
        int y = 0;

      public:
        const_board_iterator_t(const array<square_t, 64>& b, int startX = 0, int startY = 0)
            : board(b), x(startX), y(startY) {}

        const_board_iterator_t& operator++() {
            if (++y >= 8) {
                y = 0;
                ++x;
            }
            return *this;
        }

        const square_t& operator*() const {
            return board[index(x, y)];
        }

        bool operator!=(const const_board_iterator_t& other) const {
            return x != other.x || y != other.y;
        }
    };

  public:
    board_iterator_t begin() { return board_iterator_t(board); }
    board_iterator_t end() { return board_iterator_t(board, 8, 0); }
    const_board_iterator_t begin() const { return const_board_iterator_t(board); }
    const_board_iterator_t end() const { return const_board_iterator_t(board, 8, 0); }
};

struct bitboard_state {
    bool white_king_side_castle;
    bool white_queen_side_castle;
    bool black_king_side_castle;
    bool black_queen_side_castle;
    U64 en_passant_target;
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

    vector<bitboard_move_t> history;
    vector<bitboard_state> state_history;
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
        en_passant_square       = previous_state.en_passant_target;
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
            if (state.en_passant_target) {
                int square = __builtin_ctzll(state.en_passant_target);
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