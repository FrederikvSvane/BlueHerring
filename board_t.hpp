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

    vector<move_t> history;
    vector<board_state> state_history; // TODO: Implement this

    inline U64 square_to_bit(int square) {
        return 1ULL << square;
    }

    void pretty_print_board() {
        cout << endl;
        cout << "  a b c d e f g h" << endl;
        for (int x = 7; x >= 0; x--) {
            // cout << y + 1 << " ";
            cout << x << " ";
            for (int y = 0; y < 8; y++) {
                cout << at(x*8 + y) << " ";
            }
            cout << endl;
        }
        cout << "\n" << endl;
    }

    square_t at(int bit) { // Pass an index (0-63) and convert to bitboard
        if (bit < 0 || bit >= 64) {
            throw std::out_of_range("Bit index must be in range [0, 63].");
        }

        U64 pos = square_to_bit(bit); // Convert index to bitboard

        // White
        if (board_w_P & pos) return {bit % 8, bit / 8, false, {PieceType::PAWN, Color::WHITE}};
        if (board_w_N & pos) return {bit % 8, bit / 8, false, {PieceType::KNIGHT, Color::WHITE}};
        if (board_w_B & pos) return {bit % 8, bit / 8, false, {PieceType::BISHOP, Color::WHITE}};
        if (board_w_R & pos) return {bit % 8, bit / 8, false, {PieceType::ROOK, Color::WHITE}};
        if (board_w_Q & pos) return {bit % 8, bit / 8, false, {PieceType::QUEEN, Color::WHITE}};
        if (board_w_K & pos) return {bit % 8, bit / 8, false, {PieceType::KING, Color::WHITE}};

        // Black
        if (board_b_P & pos) return {bit % 8, bit / 8, false, {PieceType::PAWN, Color::BLACK}};
        if (board_b_N & pos) return {bit % 8, bit / 8, false, {PieceType::KNIGHT, Color::BLACK}};
        if (board_b_B & pos) return {bit % 8, bit / 8, false, {PieceType::BISHOP, Color::BLACK}};
        if (board_b_R & pos) return {bit % 8, bit / 8, false, {PieceType::ROOK, Color::BLACK}};
        if (board_b_Q & pos) return {bit % 8, bit / 8, false, {PieceType::QUEEN, Color::BLACK}};
        if (board_b_K & pos) return {bit % 8, bit / 8, false, {PieceType::KING, Color::BLACK}};

        // No piece is found, return empty square
        return {bit % 8, bit / 8, false, {PieceType::EMPTY, Color::NONE}};
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
        cout << "\n" << endl;
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

#endif