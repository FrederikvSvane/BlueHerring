#ifndef tests_hpp
#define tests_hpp

#include "moves.hpp"
#include <cassert>
#include <chrono>
#include <functional>
#include <iomanip>
#include <map>
#include <string>

namespace tests {

struct perft_test_case {
    string fen;
    int max_depth;
    vector<uint64_t> expected_nodes;
    bool single_color_scenario;
};

pair<uint64_t, map<string, uint64_t>> perft(board_t& board, int depth, Color color, bool single_color_only) {
    if (depth == 0)
        return {1, {}};

    uint64_t nodes = 0;
    map<string, uint64_t> move_counts;
    vector<move_t> moves = moves::generate_all_moves_for_color(board, color);

    for (const move_t& move : moves) {
        piece_t captured_piece = moves::make_move(board, move);
        uint64_t subtree_count = perft(board, depth - 1,
                                       single_color_only ? color : (color == Color::WHITE ? Color::BLACK : Color::WHITE),
                                       single_color_only)
                                     .first;
        nodes += subtree_count;
        string move_str       = encode_move(move);
        move_counts[move_str] = subtree_count;
        moves::undo_move(board, move, captured_piece);
    }
    return {nodes, move_counts};
}

void run_perft_suite() {
    auto start_time         = chrono::high_resolution_clock::now();
    uint64_t total_node_sum = 0;
    bool all_tests_passed   = true;

    vector<perft_test_case> test_cases = {
        // These are taken from https://www.chessprogramming.org/Perft_Results
        {"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", 4, {1, 20, 400, 8902, 197281}, false},
        {"r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq -", 3, {1, 48, 2039, 97862}, false},
        {"8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - -", 4, {1, 14, 191, 2812, 43238}, false},
        {"r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1", 4, {1, 6, 264, 9467, 422333}, false},
        {"rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8", 4, {1, 44, 1486, 62379, 2103487}, false},
        {"r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10", 4, {1, 46, 2079, 89890, 3894594}, false},
        // // These ones from http://www.rocechess.ch/perft.html
        {"r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1", 4, {1, 48, 2039, 97862, 4085603}, false},
        {"n1n5/PPPk4/8/8/8/8/4Kppp/5N1N b - - 0 1", 5, {1, 24, 496, 9483, 182838, 3605103}, false},
        // And these are custom made
        {"8/P/8/8/8/8/8/8", 2, {1, 4, 44}, true},    // promotion
        {"8/8/8/8/3p4/8/4P3/8", 2, {1, 2, 4}, false} // en passant
    };

    for (const auto& test : test_cases) {
        board_t board;
        board.initialize_board_from_fen(test.fen);
        cout << "\nRunning perft for WHITE with DEPTH=" << test.max_depth << ".\nPosition:\n";
        board.pretty_print_board();
        cout << endl;

        bool test_passed = true;
        for (int depth = 0; depth <= test.max_depth; depth++) {
            auto [result_int, move_breakdown] = perft(board, depth, board.active_color, test.single_color_scenario);

            bool passed = result_int == test.expected_nodes[depth];
            test_passed &= passed;

            cout << "Depth " << depth << ": Result: " << result_int
                 << ", Expected: " << test.expected_nodes[depth]
                 << (passed ? " ✓" : " ✗")
                 << endl;
        }
        cout << (test_passed ? "\nSuccess!" : "\nFailed!") << endl;
        all_tests_passed &= test_passed; // overall success status
    }

    if (all_tests_passed) {
        cout << "\n-----------> Success! Entire perft suite passed." << endl;
    } else {
        cout << "\n-----------> Fail!" << endl;
    }
}

bool compare_board_states(const board_state& a, const board_state& b) {
    return a.white_king_side_castle == b.white_king_side_castle &&
           a.white_queen_side_castle == b.white_queen_side_castle &&
           a.black_king_side_castle == b.black_king_side_castle &&
           a.black_queen_side_castle == b.black_queen_side_castle &&
           a.en_passant_x == b.en_passant_x &&
           a.en_passant_y == b.en_passant_y;
}

bool compare_boards(const board_t& a, const board_t& b) {
    for (int x = 0; x < 8; x++) {
        for (int y = 0; y < 8; y++) {
            if (a.at(x, y).piece.type != b.at(x, y).piece.type ||
                a.at(x, y).piece.color != b.at(x, y).piece.color) {
                return false;
            }
        }
    }
    return true;
}

void run_move_test(const string& test_name, function<void()> test_func) {
    cout << "Testing: " << test_name << endl;
    try {
        test_func();
        cout << "✓ " << test_name << " passed\n"
             << "_____________________________________\n"
             << endl;
    } catch (const exception& e) {
        cout << "✗ " << test_name << " failed: " << e.what() << endl;
    }
}

void verify_piece_move(board_t& board, const move_t& move,
                       PieceType expected_piece_type,
                       Color expected_piece_color,
                       PieceType expected_capture = PieceType::EMPTY,
                       Color capture_color        = Color::WHITE) {

    board_t initial_board = board;
    piece_t captured      = moves::make_move(board, move);

    // Verify piece moved correctly
    assert(board.at(move.from_x, move.from_y).piece.type == PieceType::EMPTY);
    assert(board.at(move.to_x, move.to_y).piece.type == expected_piece_type);
    assert(board.at(move.to_x, move.to_y).piece.color == expected_piece_color);

    // Verify captured piece if any
    assert(captured.type == expected_capture);
    if (expected_capture != PieceType::EMPTY) {
        assert(captured.color == capture_color);
    }

    moves::undo_move(board, move, captured);
    assert(compare_boards(board, initial_board));
}

void test_piece_movement_and_capture() {
    // Test 1: Pawn moves and captures
    board_t board;
    board.initialize_board_from_fen("8/8/8/3p4/4P3/8/8/8");
    vector<move_t> pawn_moves = moves::get_piece_moves(board, 4, 3);
    bool found_advance = false, found_capture = false;

    for (const auto& move : pawn_moves) {
        if (move.to_x == 4 && move.to_y == 4) {
            verify_piece_move(board, move, PieceType::PAWN, Color::WHITE);
            found_advance = true;
        }
        if (move.to_x == 3 && move.to_y == 4) {
            verify_piece_move(board, move, PieceType::PAWN, Color::WHITE,
                              PieceType::PAWN, Color::BLACK);
            found_capture = true;
        }
    }
    assert(found_advance && found_capture);

    // Test 2: Knight moves and captures
    board.initialize_board_from_fen("8/8/8/3p4/5N2/8/8/8");
    vector<move_t> knight_moves = moves::get_piece_moves(board, 5, 3);
    bool found_knight_move = false, found_knight_capture = false;

    for (const auto& move : knight_moves) {
        if (move.to_x == 6 && move.to_y == 1) {
            verify_piece_move(board, move, PieceType::KNIGHT, Color::WHITE);
            found_knight_move = true;
        }
        if (move.to_x == 3 && move.to_y == 4) {
            verify_piece_move(board, move, PieceType::KNIGHT, Color::WHITE,
                              PieceType::PAWN, Color::BLACK);
            found_knight_capture = true;
        }
    }
    assert(found_knight_move && found_knight_capture);

    // Test 3: Bishop moves and captures
    board.initialize_board_from_fen("8/8/8/3p4/4B3/8/8/8");
    vector<move_t> bishop_moves = moves::get_piece_moves(board, 4, 3);
    bool found_bishop_move = false, found_bishop_capture = false;

    for (const auto& move : bishop_moves) {
        if (move.to_x == 6 && move.to_y == 5) {
            verify_piece_move(board, move, PieceType::BISHOP, Color::WHITE);
            found_bishop_move = true;
        }
        if (move.to_x == 3 && move.to_y == 4) {
            verify_piece_move(board, move, PieceType::BISHOP, Color::WHITE,
                              PieceType::PAWN, Color::BLACK);
            found_bishop_capture = true;
        }
    }
    assert(found_bishop_move && found_bishop_capture);

    // Test 4: Rook moves and captures
    board.initialize_board_from_fen("8/8/8/3pR3/8/8/8/8");
    vector<move_t> rook_moves = moves::get_piece_moves(board, 4, 4);
    bool found_rook_move = false, found_rook_capture = false;

    for (const auto& move : rook_moves) {
        if (move.to_x == 4 && move.to_y == 7) {
            verify_piece_move(board, move, PieceType::ROOK, Color::WHITE);
            found_rook_move = true;
        }
        if (move.to_x == 3 && move.to_y == 4) {
            verify_piece_move(board, move, PieceType::ROOK, Color::WHITE,
                              PieceType::PAWN, Color::BLACK);
            found_rook_capture = true;
        }
    }
    assert(found_rook_move && found_rook_capture);

    // Test 5: Queen moves and captures
    board.initialize_board_from_fen("8/8/8/3p4/4Q3/8/8/8");
    vector<move_t> queen_moves = moves::get_piece_moves(board, 4, 3);
    bool found_queen_straight = false, found_queen_diagonal = false, found_queen_capture = false;

    for (const auto& move : queen_moves) {
        if (move.to_x == 4 && move.to_y == 7) {
            verify_piece_move(board, move, PieceType::QUEEN, Color::WHITE);
            found_queen_straight = true;
        }
        if (move.to_x == 6 && move.to_y == 5) {
            verify_piece_move(board, move, PieceType::QUEEN, Color::WHITE);
            found_queen_diagonal = true;
        }
        if (move.to_x == 3 && move.to_y == 4) {
            verify_piece_move(board, move, PieceType::QUEEN, Color::WHITE,
                              PieceType::PAWN, Color::BLACK);
            found_queen_capture = true;
        }
    }
    assert(found_queen_straight && found_queen_diagonal && found_queen_capture);

    // Test 6: King moves and captures
    board.initialize_board_from_fen("8/8/8/3p4/4K3/8/8/8");
    vector<move_t> king_moves = moves::get_piece_moves(board, 4, 3);
    bool found_king_move = false, found_king_capture = false;

    for (const auto& move : king_moves) {
        if (move.to_x == 4 && move.to_y == 4) {
            verify_piece_move(board, move, PieceType::KING, Color::WHITE);
            found_king_move = true;
        }
        if (move.to_x == 3 && move.to_y == 4) {
            verify_piece_move(board, move, PieceType::KING, Color::WHITE,
                              PieceType::PAWN, Color::BLACK);
            found_king_capture = true;
        }
    }
    assert(found_king_move && found_king_capture);
}

void test_en_passant() {
    // Test 1: Basic white en passant capture
    board_t board;
    board.initialize_board_from_fen("8/8/8/3pP3/8/8/8/8 w - d6 0 1");
    // board.pretty_print_board();

    board_t initial_board = board;

    move_t white_capture{4, 4, 3, 5, PieceType::EMPTY};
    piece_t white_captured = moves::make_move(board, white_capture);
    // board.pretty_print_board();

    assert(board.at(4, 4).piece.type == PieceType::EMPTY);
    assert(board.at(3, 4).piece.type == PieceType::EMPTY);
    assert(board.at(3, 5).piece.type == PieceType::PAWN);
    assert(board.at(3, 5).piece.color == Color::WHITE);

    moves::undo_move(board, white_capture, white_captured);
    // board.pretty_print_board();
    assert(compare_boards(board, initial_board));

    // Test 2: Basic black en passant capture
    board.initialize_board_from_fen("8/8/8/8/4pP3/8/8/8 b - f3 0 1");
    // board.pretty_print_board();

    initial_board = board;

    move_t black_capture{4, 3, 5, 2, PieceType::EMPTY};
    piece_t black_captured = moves::make_move(board, black_capture);
    // board.pretty_print_board();

    assert(board.at(4, 3).piece.type == PieceType::EMPTY);
    assert(board.at(5, 3).piece.type == PieceType::EMPTY);
    assert(board.at(5, 2).piece.type == PieceType::PAWN);
    assert(board.at(5, 2).piece.color == Color::BLACK);

    moves::undo_move(board, black_capture, black_captured);
    // board.pretty_print_board();
    assert(compare_boards(board, initial_board));

    // Test 3: En passant window (expires after other move)
    board.initialize_board_from_fen("8/8/8/8/pP5/8/8/8 w - a3 0 1");
    // board.pretty_print_board();

    move_t unrelated_move{1, 3, 1, 4, PieceType::EMPTY};
    piece_t captured = moves::make_move(board, unrelated_move);
    // board.pretty_print_board();

    assert(board.en_passant_x == -1);
    assert(board.en_passant_y == -1);

    moves::undo_move(board, unrelated_move, captured);
    // board.pretty_print_board();

    assert(board.en_passant_x == 0);
    assert(board.en_passant_y == 2);
}

void test_castling() {
    // Test 1: Basic kingside castling
    board_t board;
    board.initialize_board_from_fen("rnbqk2r/ppppbppp/5n2/4p3/4P3/5N2/PPPPBPPP/RNBQK2R");
    board_t initial_board = board;
    move_t kingside_castle{4, 0, 6, 0, PieceType::EMPTY};
    piece_t captured = moves::make_move(board, kingside_castle);
    assert(board.at(4, 0).piece.type == PieceType::EMPTY);
    assert(board.at(6, 0).piece.type == PieceType::KING);
    assert(board.at(7, 0).piece.type == PieceType::EMPTY);
    assert(board.at(5, 0).piece.type == PieceType::ROOK);
    assert(!board.white_king_side_castle);
    assert(!board.white_queen_side_castle);
    moves::undo_move(board, kingside_castle, captured);
    assert(compare_boards(board, initial_board));

    // Test 2: Basic queenside castling
    board.initialize_board_from_fen("r3kbnr/pppqpppp/2n5/3p4/3P4/2N5/PPPQPPPP/R3KBNR");
    initial_board = board;
    move_t queenside_castle{4, 0, 2, 0, PieceType::EMPTY};
    captured = moves::make_move(board, queenside_castle);
    assert(board.at(4, 0).piece.type == PieceType::EMPTY);
    assert(board.at(2, 0).piece.type == PieceType::KING);
    assert(board.at(0, 0).piece.type == PieceType::EMPTY);
    assert(board.at(3, 0).piece.type == PieceType::ROOK);
    assert(!board.white_king_side_castle);
    assert(!board.white_queen_side_castle);
    moves::undo_move(board, queenside_castle, captured);
    assert(compare_boards(board, initial_board));

    // Test 3: Capturing opponent's rook affects castling rights
    board.initialize_board_from_fen("r3k2r/8/8/8/8/8/8/R3K2R w KQkq - 0 1");
    initial_board = board;
    move_t capture_rook{7, 0, 7, 7, PieceType::EMPTY};
    captured = moves::make_move(board, capture_rook);
    assert(!board.black_king_side_castle);
    assert(board.black_queen_side_castle);
    assert(!board.white_king_side_castle);
    assert(board.white_queen_side_castle);
    moves::undo_move(board, capture_rook, captured);
    assert(compare_boards(board, initial_board));

    // Test 4: Moving own rook affects castling rights
    move_t move_rook{7, 0, 7, 4, PieceType::EMPTY};
    captured = moves::make_move(board, move_rook);
    assert(!board.white_king_side_castle);
    assert(board.white_queen_side_castle);
    moves::undo_move(board, move_rook, captured);
    assert(compare_boards(board, initial_board));

    // Test 5: Cannot castle through check
    board.initialize_board_from_fen("r3k2r/8/8/8/8/8/4b3/R3K2R w KQkq - 0 1");
    vector<move_t> legal_moves  = moves::generate_all_moves_for_color(board, Color::WHITE);
    bool found_kingside_castle  = false;
    bool found_queenside_castle = false;

    for (const auto& move : legal_moves) {
        if (move.from_x == 4 && move.from_y == 0) { // King's starting position
            if (move.to_x == 6 && move.to_y == 0)
                found_kingside_castle = true; // Kingside castle
            if (move.to_x == 2 && move.to_y == 0)
                found_queenside_castle = true; // Queenside castle
        }
    }

    assert(!found_kingside_castle); // Shouldn't be able to castle through check
    assert(!found_queenside_castle);
}

void test_pawn_promotion() {
    // Test 1: Simple promotion without capture
    board_t board;
    board.initialize_board_from_fen("8/4P3/8/8/8/8/8/8");
    board_t initial_board = board;
    move_t move{4, 6, 4, 7, PieceType::QUEEN};
    piece_t captured = moves::make_move(board, move);
    assert(board.at(4, 6).piece.type == PieceType::EMPTY);
    assert(board.at(4, 7).piece.type == PieceType::QUEEN);
    assert(board.at(4, 7).piece.color == Color::WHITE);
    moves::undo_move(board, move, captured);
    assert(compare_boards(board, initial_board));

    // Test 2: Promotion with capture
    board.initialize_board_from_fen("4r3/3P4/8/8/8/8/8/8");
    initial_board = board;
    move_t capture_move{3, 6, 4, 7, PieceType::QUEEN};
    piece_t capture_piece = moves::make_move(board, capture_move);
    assert(board.at(3, 6).piece.type == PieceType::EMPTY);
    assert(board.at(4, 7).piece.type == PieceType::QUEEN);
    assert(board.at(4, 7).piece.color == Color::WHITE);
    assert(capture_piece.type == PieceType::ROOK);
    assert(capture_piece.color == Color::BLACK);
    moves::undo_move(board, capture_move, capture_piece);
    assert(compare_boards(board, initial_board));
}

void test_board_state_history() {
    board_t board;
    board.initialize_board_from_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR");
    vector<move_t> moves = {
        move_t{4, 1, 4, 3, PieceType::EMPTY}, // e2e4
        move_t{4, 6, 4, 4, PieceType::EMPTY}, // e7e5
        move_t{5, 0, 2, 3, PieceType::EMPTY}  // f1c4
    };
    vector<piece_t> captured_pieces;
    vector<board_state> states;

    // Make multiple moves and save states
    for (const auto& move : moves) {
        states.push_back({board.white_king_side_castle,
                          board.white_queen_side_castle,
                          board.black_king_side_castle,
                          board.black_queen_side_castle,
                          board.en_passant_x,
                          board.en_passant_y});
        captured_pieces.push_back(moves::make_move(board, move));
    }

    // Undo moves in reverse order
    for (int i = moves.size() - 1; i >= 0; i--) {
        moves::undo_move(board, moves[i], captured_pieces[i]);
        assert(board.white_king_side_castle == states[i].white_king_side_castle);
        assert(board.white_queen_side_castle == states[i].white_queen_side_castle);
        assert(board.black_king_side_castle == states[i].black_king_side_castle);
        assert(board.black_queen_side_castle == states[i].black_queen_side_castle);
        assert(board.en_passant_x == states[i].en_passant_x);
        assert(board.en_passant_y == states[i].en_passant_y);
    }
}

void test_check_and_checkmate() {
    // Test 1: Direct checks from different pieces
    vector<pair<string, bool>> check_positions = {
        {"k7/8/8/8/R7/8/8/K7 b - - 0 1", true},   // Rook check
        {"k7/8/8/8/8/8/6B1/K7 b - - 0 1", true},  // Bishop check
        {"k7/2N5/8/8/8/8/8/K7 b - - 0 1", true},  // Knight check
        {"k7/1P6/8/8/8/8/8/K7 b - - 0 1", true},  // Pawn check
        {"k7/8/8/8/8/8/Q7/K7 b - - 0 1", true},   // Queen check
        {"4k3/8/8/8/8/8/8/4K3 b - - 0 1", false}, // Kings adjacent, no check
        {"k7/8/8/8/8/8/8/K6R b - - 0 1", false},  // Rook not giving check
        {"k7/8/8/8/8/8/8/K5B1 b - - 0 1", false}, // Bishop not giving check
        {"k7/8/8/8/8/8/1P6/K7 b - - 0 1", false}, // Pawn not giving check
        {"k7/8/8/8/8/8/8/KQ6 b - - 0 1", false},  // Queen not giving check
    };

    for (const auto& [fen, expected] : check_positions) {
        board_t board;
        board.initialize_board_from_fen(fen);
        Color king_color = (fen.find('w') != string::npos) ? Color::WHITE : Color::BLACK;
        bool actual      = moves::is_in_check(board, king_color);
        assert(actual == expected);
    }

    // Test 2: King captures to escape check
    board_t board;
    board.initialize_board_from_fen("4k3/8/8/8/8/8/3p4/4K3 w - - 0 1");
    board_t initial_board = board;
    move_t escape_move{4, 0, 3, 1, PieceType::EMPTY};
    piece_t captured = moves::make_move(board, escape_move);
    assert(board.at(4, 0).piece.type == PieceType::EMPTY);
    assert(board.at(3, 1).piece.type == PieceType::KING);
    assert(board.at(3, 1).piece.color == Color::WHITE);
    assert(!moves::is_in_check(board, Color::WHITE));
    moves::undo_move(board, escape_move, captured);
    assert(compare_boards(board, initial_board));

    // Test 3: Checkmate positions
    vector<pair<string, pair<bool, bool>>> mate_positions = {
        {"k7/8/8/8/8/8/R7/1R5K b - - 0 1", {true, false}},   // Back rank mate
        {"k7/8/8/8/8/8/R7/R6K b - - 0 1", {true, true}},     // Check but escapable
        {"k7/2K5/8/8/8/8/8/R7 b - - 0 1", {true, false}},    // Mate with king support
        {"6rk/5Npp/8/8/8/8/8/6K1 w - - 0 1", {true, false}}, // Smothered mate
    };

    for (const auto& [fen, expected] : mate_positions) {
        board_t board;
        board.initialize_board_from_fen(fen);
        const auto& [expected_check, expected_has_moves] = expected;
        bool actual_check                                = moves::is_in_check(board, Color::BLACK);
        vector<move_t> legal_moves                       = moves::generate_all_moves_for_color(board, Color::BLACK);
        bool has_legal_moves                             = !legal_moves.empty();
        bool is_in_checkmate                             = (!has_legal_moves && actual_check);
        assert(actual_check == expected_check);
        assert(has_legal_moves == expected_has_moves);
    }

    // Test 4: Move generation under check
    board.initialize_board_from_fen("k7/8/8/8/8/3b4/8/4K3 w - - 0 1");
    vector<move_t> legal_moves = moves::generate_all_moves_for_color(board, Color::WHITE);

    for (const auto& move : legal_moves) {
        piece_t captured    = moves::make_move(board, move);
        bool still_in_check = moves::is_in_check(board, Color::WHITE);
        moves::undo_move(board, move, captured);
        assert(!still_in_check);
    }

    // Test 5: Check after pawn promotion
    board.initialize_board_from_fen("8/8/8/8/8/8/4K1p1/5N2 b - - 0 1");
    move_t promotion_capture{6, 1, 5, 0, PieceType::BISHOP};
    captured = moves::make_move(board, promotion_capture);
    assert(moves::is_in_check(board, Color::WHITE));
    legal_moves = moves::generate_all_moves_for_color(board, Color::WHITE);
    assert(legal_moves.size() == 7);
    moves::undo_move(board, promotion_capture, captured);
    assert(!moves::is_in_check(board, Color::WHITE));
}

void run_rules_test_suite() {
    cout << "\nRunning move/undo move tests...\n"
         << endl;

    run_move_test("Movement and capture for each piece", test_piece_movement_and_capture);
    run_move_test("En passant", test_en_passant);
    run_move_test("Castling", test_castling);
    run_move_test("Pawn promotion", test_pawn_promotion);
    run_move_test("Board state history", test_board_state_history);
    run_move_test("Check and checkmate", test_check_and_checkmate);
}

void run_speed_test_suite() {
    vector<pair<string, int>> test_positions = {
        {"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", 5},             // starting position
        {"r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1", 4}, // mid game position
        {"8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1", 5},                            // end game position
    };

    vector<double> nps_results;
    uint64_t total_nodes = 0;
    auto suite_start     = chrono::high_resolution_clock::now();

    for (const auto& [fen, max_depth] : test_positions) {
        board_t board;
        board.initialize_board_from_fen(fen);
        cout << "\nRunning speed test for position:\n";
        board.pretty_print_board();
        uint64_t position_nodes = 0;
        auto position_start     = chrono::high_resolution_clock::now();

        for (int depth = 1; depth <= max_depth; depth++) {
            auto depth_start    = chrono::high_resolution_clock::now();
            auto [nodes, moves] = perft(board, depth, board.active_color, false);
            auto depth_end      = chrono::high_resolution_clock::now();
            position_nodes += nodes;
            total_nodes += nodes;
            auto depth_duration = chrono::duration_cast<chrono::milliseconds>(depth_end - depth_start);
            double depth_nps    = static_cast<double>(nodes) * 1000.0 / depth_duration.count();
            cout << "Depth " << depth << ": "
                 << nodes << " nodes in "
                 << depth_duration.count() << "ms"
                 << " (" << fixed << setprecision(0) << depth_nps << " NPS)\n";
        }

        auto position_end      = chrono::high_resolution_clock::now();
        auto position_duration = chrono::duration_cast<chrono::milliseconds>(position_end - position_start);
        double position_nps    = static_cast<double>(position_nodes) * 1000.0 / position_duration.count();
        nps_results.push_back(position_nps);
        cout << "\n------------> Average NPS: " << fixed << setprecision(0) << position_nps << "\n";
        cout << "\n________________________________________________\n";
    }

    auto suite_end      = chrono::high_resolution_clock::now();
    auto suite_duration = chrono::duration_cast<chrono::milliseconds>(suite_end - suite_start);
    double overall_nps  = static_cast<double>(total_nodes) * 1000.0 / suite_duration.count();
    cout << "\nFinal average NPS: " << fixed << setprecision(0) << overall_nps << "\n";
}

} // namespace tests

#endif