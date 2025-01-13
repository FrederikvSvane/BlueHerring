#ifndef tests_hpp
#define tests_hpp

#include "moves.hpp"
#include <cassert>
#include <chrono>
#include <functional>
#include <iomanip>
#include <map>
#include <string>

namespace bitboard_tests {

struct perft_test_case {
    string fen;
    int max_depth;
    vector<uint64_t> expected_nodes;
    bool single_color_scenario;
};

pair<uint64_t, map<string, uint64_t>> perft(bitboard_t& board, int depth, Color color, bool single_color_only, int indent = 0) {
    if (depth == 0)
        return {1, {}};

    uint64_t nodes = 0;
    map<string, uint64_t> move_counts;
    vector<bitboard_move_t> moves = bitboard_moves::generate_all_moves_for_color(board, color);

    for (const bitboard_move_t& move : moves) {
        piece_t captured_piece  = bitboard_moves::make_move(board, move);
        string move_str         = encode_move(bitboard_to_coordinate_move(move));
        auto [subtree_count, _] = perft(board, depth - 1,
                                        single_color_only ? color : (color == Color::WHITE ? Color::BLACK : Color::WHITE),
                                        single_color_only,
                                        indent + 2); // Increase indentation for next level

        nodes += subtree_count;
        move_counts[move_str] = subtree_count;
        bitboard_moves::undo_move(board, move, captured_piece);
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
        bitboard_t board;
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
        all_tests_passed &= test_passed;
    }

    if (all_tests_passed) {
        cout << "\n-----------> Success! Entire perft suite passed." << endl;
    } else {
        cout << "\n-----------> Fail!" << endl;
    }
}

bool compare_bitboard_states(const bitboard_state& a, const bitboard_state& b) {
    return a.white_king_side_castle == b.white_king_side_castle &&
           a.white_queen_side_castle == b.white_queen_side_castle &&
           a.black_king_side_castle == b.black_king_side_castle &&
           a.black_queen_side_castle == b.black_queen_side_castle &&
           a.en_passant_square == b.en_passant_square;
}

bool compare_boards(const bitboard_t& a, const bitboard_t& b) {
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

void verify_piece_move(bitboard_t& board, const bitboard_move_t& bitboard_move,
                       PieceType expected_piece_type,
                       Color expected_piece_color,
                       PieceType expected_capture = PieceType::EMPTY,
                       Color capture_color        = Color::WHITE) {

    bitboard_t initial_board = board;
    piece_t captured         = bitboard_moves::make_move(board, bitboard_move);
    move_t coordinate_move   = bitboard_to_coordinate_move(bitboard_move);

    // Verify piece moved correctly
    assert(board.at(coordinate_move.from_x, coordinate_move.from_y).piece.type == PieceType::EMPTY);
    assert(board.at(coordinate_move.to_x, coordinate_move.to_y).piece.type == expected_piece_type);
    assert(board.at(coordinate_move.to_x, coordinate_move.to_y).piece.color == expected_piece_color);

    // Verify captured piece if any
    assert(captured.type == expected_capture);
    if (expected_capture != PieceType::EMPTY) {
        assert(captured.color == capture_color);
    }

    bitboard_moves::undo_move(board, bitboard_move, captured);
    assert(compare_boards(board, initial_board));
}

void test_piece_movement_and_capture() {
    // Test 1: Pawn moves and captures
    bitboard_t board;
    board.initialize_board_from_fen("8/8/8/3p4/4P3/8/8/8");
    vector<bitboard_move_t> pawn_moves = bitboard_moves::get_piece_moves(board, 4, 3);
    assert(pawn_moves.size() == 2); // either straight up or capture
    bool found_advance = false, found_capture = false;

    for (const auto& move : pawn_moves) {
        move_t coordinate_move = bitboard_to_coordinate_move(move);
        if (coordinate_move.to_x == 4 && coordinate_move.to_y == 4) {
            verify_piece_move(board, move, PieceType::PAWN, Color::WHITE);
            found_advance = true;
        }
        if (coordinate_move.to_x == 3 && coordinate_move.to_y == 4) {
            verify_piece_move(board, move, PieceType::PAWN, Color::WHITE,
                              PieceType::PAWN, Color::BLACK);
            found_capture = true;
        }
    }
    assert(found_advance && found_capture);

    // Test 2: Knight moves and captures
    board.initialize_board_from_fen("8/8/8/3p4/5N2/8/8/8");
    vector<bitboard_move_t> knight_moves = bitboard_moves::get_piece_moves(board, 5, 3);
    assert(knight_moves.size() == 8);
    bool found_knight_move = false, found_knight_capture = false;

    for (const auto& move : knight_moves) {
        move_t coordinate_move = bitboard_to_coordinate_move(move);

        if (coordinate_move.to_x == 6 && coordinate_move.to_y == 1) {
            verify_piece_move(board, move, PieceType::KNIGHT, Color::WHITE);
            found_knight_move = true;
        }
        if (coordinate_move.to_x == 3 && coordinate_move.to_y == 4) {
            verify_piece_move(board, move, PieceType::KNIGHT, Color::WHITE,
                              PieceType::PAWN, Color::BLACK);
            found_knight_capture = true;
        }
    }
    assert(found_knight_move && found_knight_capture);

    // Test 3: Bishop moves and captures
    board.initialize_board_from_fen("8/8/8/3p4/4B3/8/8/8");
    vector<bitboard_move_t> bishop_moves = bitboard_moves::get_piece_moves(board, 4, 3);
    assert(bishop_moves.size() == 10);
    bool found_bishop_move = false, found_bishop_capture = false;

    for (const auto& move : bishop_moves) {
        move_t coordinate_move = bitboard_to_coordinate_move(move);

        if (coordinate_move.to_x == 6 && coordinate_move.to_y == 5) {
            verify_piece_move(board, move, PieceType::BISHOP, Color::WHITE);
            found_bishop_move = true;
        }
        if (coordinate_move.to_x == 3 && coordinate_move.to_y == 4) {
            verify_piece_move(board, move, PieceType::BISHOP, Color::WHITE,
                              PieceType::PAWN, Color::BLACK);
            found_bishop_capture = true;
        }
    }
    assert(found_bishop_move && found_bishop_capture);

    // Test 4: Rook moves and captures
    board.initialize_board_from_fen("8/8/8/3pR3/8/8/8/8");
    vector<bitboard_move_t> rook_moves = bitboard_moves::get_piece_moves(board, 4, 4);
    assert(rook_moves.size() == 11);
    bool found_rook_move = false, found_rook_capture = false;

    for (const auto& move : rook_moves) {
        move_t coordinate_move = bitboard_to_coordinate_move(move);

        if (coordinate_move.to_x == 4 && coordinate_move.to_y == 7) {
            verify_piece_move(board, move, PieceType::ROOK, Color::WHITE);
            found_rook_move = true;
        }
        if (coordinate_move.to_x == 3 && coordinate_move.to_y == 4) {
            verify_piece_move(board, move, PieceType::ROOK, Color::WHITE,
                              PieceType::PAWN, Color::BLACK);
            found_rook_capture = true;
        }
    }
    assert(found_rook_move && found_rook_capture);

    // Test 5: Queen moves and captures
    board.initialize_board_from_fen("8/8/8/3p4/4Q3/8/8/8");
    vector<bitboard_move_t> queen_moves = bitboard_moves::get_piece_moves(board, 4, 3);
    assert(queen_moves.size() == 24);
    bool found_queen_straight = false, found_queen_diagonal = false, found_queen_capture = false;

    for (const auto& move : queen_moves) {
        move_t coordinate_move = bitboard_to_coordinate_move(move);

        if (coordinate_move.to_x == 4 && coordinate_move.to_y == 7) {
            verify_piece_move(board, move, PieceType::QUEEN, Color::WHITE);
            found_queen_straight = true;
        }
        if (coordinate_move.to_x == 6 && coordinate_move.to_y == 5) {
            verify_piece_move(board, move, PieceType::QUEEN, Color::WHITE);
            found_queen_diagonal = true;
        }
        if (coordinate_move.to_x == 3 && coordinate_move.to_y == 4) {
            verify_piece_move(board, move, PieceType::QUEEN, Color::WHITE,
                              PieceType::PAWN, Color::BLACK);
            found_queen_capture = true;
        }
    }
    assert(found_queen_straight && found_queen_diagonal && found_queen_capture);

    // Test 6: King moves and captures
    board.initialize_board_from_fen("8/8/8/3p4/4K3/8/8/8");
    vector<bitboard_move_t> king_moves = bitboard_moves::get_piece_moves(board, 4, 3);
    assert(king_moves.size() == 8);
    bool found_king_move = false, found_king_capture = false;

    for (const auto& move : king_moves) {
        move_t coordinate_move = bitboard_to_coordinate_move(move);

        if (coordinate_move.to_x == 4 && coordinate_move.to_y == 4) {
            verify_piece_move(board, move, PieceType::KING, Color::WHITE);
            found_king_move = true;
        }
        if (coordinate_move.to_x == 3 && coordinate_move.to_y == 4) {
            verify_piece_move(board, move, PieceType::KING, Color::WHITE,
                              PieceType::PAWN, Color::BLACK);
            found_king_capture = true;
        }
    }
    assert(found_king_move && found_king_capture);
}

void custom_scenario_test_1() {
    bitboard_t board;

    board.initialize_board_from_fen("7p/7p/7p/K6p/7p/7p/7p/7p w - - 0 1");
    board.pretty_print_board();
    // board.print_bitboard(board.board_b_P);
    if (bitboard_moves::is_in_check(board, Color::WHITE)) {
        cout << "white is currently in check" << endl;
    }
    vector<bitboard_move_t> all_pawn_moves;
    for (int i = 0; i < 8; i++) {
        vector<bitboard_move_t> current_pawn_moves = bitboard_moves::get_pawn_moves(board, 7, i);
        // for(auto& move : current_pawn_moves){
        //     cout << encode_move(bitboard_to_coordinate_move(move)) << endl;
        // }
        all_pawn_moves.insert(all_pawn_moves.end(), current_pawn_moves.begin(), current_pawn_moves.end());
    }
    for (auto& move : all_pawn_moves) {
        move_t cmove = bitboard_to_coordinate_move(move);
        cout << "pawn at square (" << encode_move(cmove) << endl;
        // if(cmove.to_x == 3 && cmove.to_y == 0){
        //     cout << "motherfucker piece is at " << cmove.from_x << ", " << cmove.from_y << endl;
        // }
    }
    assert(!bitboard_moves::is_in_check(board, Color::WHITE));

    board.initialize_board_from_fen("7P/7P/7P/k6P/7P/7P/7P/7P w - - 0 1");
    board.pretty_print_board();
}

void test_en_passant() {
    // Test 1: Basic white en passant capture
    bitboard_t board;
    board.initialize_board_from_fen("8/8/8/3pP3/8/8/8/8 w - d6 0 1");
    // board.pretty_print_board();

    bitboard_t initial_board = board;
    bitboard_move_t white_capture{};
    vector<bitboard_move_t> possible_moves = bitboard_moves::get_pawn_moves(board, 4, 4);
    for (auto& move : possible_moves) {
        if (encode_move(bitboard_to_coordinate_move(move)) == "e5d6") {
            white_capture = move;
        };
    }
    assert(encode_move(bitboard_to_coordinate_move(white_capture)) == "e5d6");

    piece_t white_captured = bitboard_moves::make_move(board, white_capture);
    // board.pretty_print_board();
    assert(board.at(4, 4).piece.type == PieceType::EMPTY);
    assert(board.at(3, 4).piece.type == PieceType::EMPTY);
    assert(board.at(3, 5).piece.type == PieceType::PAWN);
    assert(board.at(3, 5).piece.color == Color::WHITE);

    bitboard_moves::undo_move(board, white_capture, white_captured);
    // board.pretty_print_board();
    assert(compare_boards(board, initial_board));

    // Test 2: Basic black en passant capture
    board.initialize_board_from_fen("8/8/8/8/4pP3/8/8/8 b - f3 0 1");
    // board.pretty_print_board();

    initial_board = board;

    move_t move{4, 3, 5, 2, PieceType::EMPTY};
    bitboard_move_t black_capture = coordinate_to_bitboard_move(move);
    piece_t black_captured        = bitboard_moves::make_move(board, black_capture);
    // board.pretty_print_board();

    assert(board.at(4, 3).piece.type == PieceType::EMPTY);
    assert(board.at(5, 3).piece.type == PieceType::EMPTY);
    assert(board.at(5, 2).piece.type == PieceType::PAWN);
    assert(board.at(5, 2).piece.color == Color::BLACK);

    bitboard_moves::undo_move(board, black_capture, black_captured);
    // board.pretty_print_board();
    assert(compare_boards(board, initial_board));

    // Test 3: En passant window (expires after other move)
    board.initialize_board_from_fen("8/8/8/8/pP5/8/8/8 w - a3 0 1");
    // board.pretty_print_board();

    bitboard_move_t unrelated_move{1, 3, 1, 4, PieceType::EMPTY};
    piece_t captured = bitboard_moves::make_move(board, unrelated_move);
    // board.pretty_print_board();

    assert(board.en_passant_square == 0ULL);

    bitboard_moves::undo_move(board, unrelated_move, captured);
    // board.pretty_print_board();

    assert(board.en_passant_square == (1ULL << 16) /* 16'th bit = a3 (8*2 bits above a1)*/);
}

void test_castling() {
    // Test 1: Basic kingside castling
    bitboard_t board;
    board.initialize_board_from_fen("rnbqk2r/ppppbppp/5n2/4p3/4P3/5N2/PPPPBPPP/RNBQK2R");
    bitboard_t initial_board = board;
    bitboard_move_t kingside_castle{4, 0, 6, 0, PieceType::EMPTY};
    piece_t captured = bitboard_moves::make_move(board, kingside_castle);
    assert(board.at(4, 0).piece.type == PieceType::EMPTY);
    assert(board.at(6, 0).piece.type == PieceType::KING);
    assert(board.at(7, 0).piece.type == PieceType::EMPTY);
    assert(board.at(5, 0).piece.type == PieceType::ROOK);
    assert(!board.white_king_side_castle);
    assert(!board.white_queen_side_castle);
    bitboard_moves::undo_move(board, kingside_castle, captured);
    assert(compare_boards(board, initial_board));

    // Test 2: Basic queenside castling
    board.initialize_board_from_fen("r3kbnr/pppqpppp/2n5/3p4/3P4/2N5/PPPQPPPP/R3KBNR");
    initial_board = board;
    bitboard_move_t queenside_castle{4, 0, 2, 0, PieceType::EMPTY};
    captured = bitboard_moves::make_move(board, queenside_castle);
    assert(board.at(4, 0).piece.type == PieceType::EMPTY);
    assert(board.at(2, 0).piece.type == PieceType::KING);
    assert(board.at(0, 0).piece.type == PieceType::EMPTY);
    assert(board.at(3, 0).piece.type == PieceType::ROOK);
    assert(!board.white_king_side_castle);
    assert(!board.white_queen_side_castle);
    bitboard_moves::undo_move(board, queenside_castle, captured);
    assert(compare_boards(board, initial_board));

    // Test 3: Capturing opponent's rook affects castling rights
    board.initialize_board_from_fen("r3k2r/8/8/8/8/8/8/R3K2R w KQkq - 0 1");
    initial_board = board;
    bitboard_move_t capture_rook{7, 0, 7, 7, PieceType::EMPTY};
    captured = bitboard_moves::make_move(board, capture_rook);
    assert(!board.black_king_side_castle);
    assert(board.black_queen_side_castle);
    assert(!board.white_king_side_castle);
    assert(board.white_queen_side_castle);
    bitboard_moves::undo_move(board, capture_rook, captured);
    assert(compare_boards(board, initial_board));

    // Test 4: Moving own rook affects castling rights
    bitboard_move_t move_rook{7, 0, 7, 4, PieceType::EMPTY};
    captured = bitboard_moves::make_move(board, move_rook);
    assert(!board.white_king_side_castle);
    assert(board.white_queen_side_castle);
    bitboard_moves::undo_move(board, move_rook, captured);
    assert(compare_boards(board, initial_board));

    // Test 5: Cannot castle through check
    board.initialize_board_from_fen("r3k2r/8/8/8/8/8/4b3/R3K2R w KQkq - 0 1");
    vector<bitboard_move_t> legal_moves = bitboard_moves::generate_all_moves_for_color(board, Color::WHITE);
    bool found_kingside_castle          = false;
    bool found_queenside_castle         = false;

    for (const auto& move : legal_moves) {
        move_t coordinate_move = bitboard_to_coordinate_move(move);
        if (coordinate_move.from_x == 4 && coordinate_move.from_y == 0) { // King's starting position
            if (coordinate_move.to_x == 6 && coordinate_move.to_y == 0)
                found_kingside_castle = true; // Kingside castle
            if (coordinate_move.to_x == 2 && coordinate_move.to_y == 0)
                found_queenside_castle = true; // Queenside castle
        }
    }

    assert(!found_kingside_castle); // Shouldn't be able to castle through check
    assert(!found_queenside_castle);
}

void test_pawn_promotion() {
    // Test 1: Simple promotion without capture
    bitboard_t board;
    board.initialize_board_from_fen("8/4P3/8/8/8/8/8/8");
    bitboard_t initial_board = board;
    bitboard_move_t move{4, 6, 4, 7, PieceType::QUEEN};
    piece_t captured = bitboard_moves::make_move(board, move);
    assert(board.at(4, 6).piece.type == PieceType::EMPTY);
    assert(board.at(4, 7).piece.type == PieceType::QUEEN);
    assert(board.at(4, 7).piece.color == Color::WHITE);
    bitboard_moves::undo_move(board, move, captured);
    assert(compare_boards(board, initial_board));

    // Test 2: Promotion with capture
    board.initialize_board_from_fen("4r3/3P4/8/8/8/8/8/8");
    initial_board = board;
    bitboard_move_t capture_move{3, 6, 4, 7, PieceType::QUEEN};
    piece_t capture_piece = bitboard_moves::make_move(board, capture_move);
    assert(board.at(3, 6).piece.type == PieceType::EMPTY);
    assert(board.at(4, 7).piece.type == PieceType::QUEEN);
    assert(board.at(4, 7).piece.color == Color::WHITE);
    assert(capture_piece.type == PieceType::ROOK);
    assert(capture_piece.color == Color::BLACK);
    bitboard_moves::undo_move(board, capture_move, capture_piece);
    assert(compare_boards(board, initial_board));
}

void test_board_state_history() {
    bitboard_t board;
    board.initialize_board_from_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR");
    vector<bitboard_move_t> moves = {
        bitboard_move_t{4, 1, 4, 3, PieceType::EMPTY}, // e2e4
        bitboard_move_t{4, 6, 4, 4, PieceType::EMPTY}, // e7e5
        bitboard_move_t{5, 0, 2, 3, PieceType::EMPTY}  // f1c4
    };
    vector<piece_t> captured_pieces;
    vector<bitboard_state> states;

    // Make multiple moves and save states
    for (const auto& move : moves) {
        states.push_back({board.white_king_side_castle,
                          board.white_queen_side_castle,
                          board.black_king_side_castle,
                          board.black_queen_side_castle,
                          board.en_passant_square});
        captured_pieces.push_back(bitboard_moves::make_move(board, move));
    }

    // Undo moves in reverse order
    for (int i = moves.size() - 1; i >= 0; i--) {
        bitboard_moves::undo_move(board, moves[i], captured_pieces[i]);
        assert(board.white_king_side_castle == states[i].white_king_side_castle);
        assert(board.white_queen_side_castle == states[i].white_queen_side_castle);
        assert(board.black_king_side_castle == states[i].black_king_side_castle);
        assert(board.black_queen_side_castle == states[i].black_queen_side_castle);
        assert(board.en_passant_square == states[i].en_passant_square);
    }
}

void test_check_and_checkmate() {
    // Test 1: Direct checks from different pieces
    vector<pair<string, bool>> check_positions = {
        {"k7/8/8/8/R7/8/8/K7 b - - 0 1", true},  // Rook check
        {"k7/8/8/8/8/8/6B1/K7 b - - 0 1", true}, // Bishop check
        {"k7/2N5/8/8/8/8/8/K7 b - - 0 1", true}, // Knight check
        // {"k7/1P6/8/8/8/8/8/K7 b - - 0 1", true},  // Pawn check
        {"k7/8/8/8/8/8/Q7/K7 b - - 0 1", true},   // Queen check
        {"4k3/8/8/8/8/8/8/4K3 b - - 0 1", false}, // Kings adjacent, no check
        {"k7/8/8/8/8/8/8/K6R b - - 0 1", false},  // Rook not giving check
        {"k7/8/8/8/8/8/8/K5B1 b - - 0 1", false}, // Bishop not giving check
        {"k7/8/8/8/8/8/1P6/K7 b - - 0 1", false}, // Pawn not giving check
        {"k7/8/8/8/8/8/8/KQ6 b - - 0 1", false},  // Queen not giving check
    };

    for (const auto& [fen, expected] : check_positions) {
        bitboard_t board;
        board.initialize_board_from_fen(fen);
        Color king_color = (fen.find('w') != string::npos) ? Color::WHITE : Color::BLACK;
        bool actual      = bitboard_moves::is_in_check(board, king_color);
        assert(actual == expected);
    }

    // Test 2: King captures to escape check
    bitboard_t board;
    board.initialize_board_from_fen("4k3/8/8/8/8/8/3p4/4K3 w - - 0 1");
    bitboard_t initial_board = board;
    bitboard_move_t escape_move{4, 0, 3, 1, PieceType::EMPTY};
    piece_t captured = bitboard_moves::make_move(board, escape_move);
    assert(board.at(4, 0).piece.type == PieceType::EMPTY);
    assert(board.at(3, 1).piece.type == PieceType::KING);
    assert(board.at(3, 1).piece.color == Color::WHITE);
    assert(!bitboard_moves::is_in_check(board, Color::WHITE));
    bitboard_moves::undo_move(board, escape_move, captured);
    assert(compare_boards(board, initial_board));

    // Test 3: Checkmate positions
    vector<pair<string, pair<bool, bool>>> mate_positions = {
        {"k7/8/8/8/8/8/R7/1R5K b - - 0 1", {true, false}},   // Back rank mate
        {"k7/8/8/8/8/8/R7/R6K b - - 0 1", {true, true}},     // Check but escapable
        {"k7/2K5/8/8/8/8/8/R7 b - - 0 1", {true, false}},    // Mate with king support
        {"6rk/5Npp/8/8/8/8/8/6K1 w - - 0 1", {true, false}}, // Smothered mate
    };

    for (const auto& [fen, expected] : mate_positions) {
        bitboard_t board;
        board.initialize_board_from_fen(fen);
        const auto& [expected_check, expected_has_moves] = expected;
        bool actual_check                                = bitboard_moves::is_in_check(board, Color::BLACK);
        vector<bitboard_move_t> legal_moves              = bitboard_moves::generate_all_moves_for_color(board, Color::BLACK);
        bool has_legal_moves                             = !legal_moves.empty();
        bool is_in_checkmate                             = (!has_legal_moves && actual_check);
        assert(actual_check == expected_check);
        assert(has_legal_moves == expected_has_moves);
    }

    // Test 4: Move generation under check
    board.initialize_board_from_fen("k7/8/8/8/8/3b4/8/4K3 w - - 0 1");
    vector<bitboard_move_t> legal_moves = bitboard_moves::generate_all_moves_for_color(board, Color::WHITE);

    for (const auto& move : legal_moves) {
        piece_t captured    = bitboard_moves::make_move(board, move);
        bool still_in_check = bitboard_moves::is_in_check(board, Color::WHITE);
        bitboard_moves::undo_move(board, move, captured);
        assert(!still_in_check);
    }

    // Test 5: Check after pawn promotion
    board.initialize_board_from_fen("8/8/8/8/8/8/4K1p1/5N2 b - - 0 1");
    bitboard_move_t promotion_capture{6, 1, 5, 0, PieceType::BISHOP};
    captured = bitboard_moves::make_move(board, promotion_capture);
    assert(bitboard_moves::is_in_check(board, Color::WHITE));
    legal_moves = bitboard_moves::generate_all_moves_for_color(board, Color::WHITE);
    assert(legal_moves.size() == 7);
    bitboard_moves::undo_move(board, promotion_capture, captured);
    assert(!bitboard_moves::is_in_check(board, Color::WHITE));
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
    custom_scenario_test_1();
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
        bitboard_t board;
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

void test_evaluation() {
    // Test 1: Basic piece values
    bitboard_t board;

    // Empty board baseline
    board.initialize_board_from_fen("8/8/8/8/8/8/8/8");
    assert(eval::evaluate_position(board) == 0);

    // Single pieces on e4
    board.initialize_board_from_fen("8/8/8/8/4P3/8/8/8");
    assert(eval::evaluate_position(board) == eval::PAWN_VALUE + eval::PAWN_TABLE[35]);

    board.initialize_board_from_fen("8/8/8/8/4N3/8/8/8");
    assert(eval::evaluate_position(board) == eval::KNIGHT_VALUE + eval::KNIGHT_TABLE[35]);

    board.initialize_board_from_fen("8/8/8/8/4B3/8/8/8");
    assert(eval::evaluate_position(board) == eval::BISHOP_VALUE + eval::BISHOP_TABLE[35]);

    board.initialize_board_from_fen("8/8/8/8/4R3/8/8/8");
    assert(eval::evaluate_position(board) == eval::ROOK_VALUE + eval::ROOK_TABLE[35]);

    board.initialize_board_from_fen("8/8/8/8/4Q3/8/8/8");
    assert(eval::evaluate_position(board) == eval::QUEEN_VALUE + eval::QUEEN_TABLE[35]);

    board.initialize_board_from_fen("8/8/8/8/4K3/8/8/8");
    assert(eval::evaluate_position(board) == eval::KING_VALUE + eval::KING_TABLE[35]);

    // Black piece on 2nd rank gives same score as white piece on 7th rank
    board.initialize_board_from_fen("8/P7/8/8/8/8/8/8");
    int wps = eval::evaluate_position(board);
    assert(wps == 150);

    board.initialize_board_from_fen("8/8/8/8/8/8/p7/8");
    int bps = eval::evaluate_position(board);
    assert(bps == -150);

    

    // Test 2: Color symmetry
    board.initialize_board_from_fen("8/8/8/4P3/8/8/8/8");
    int white_pawn_score = eval::evaluate_position(board);

    board.initialize_board_from_fen("8/8/8/8/4p3/8/8/8");
    int black_pawn_score = eval::evaluate_position(board);
    assert(white_pawn_score == -black_pawn_score);

    // Test 3: Piece square tables
    // Test that pawns are worth more in the center and towards promotion
    board.initialize_board_from_fen("8/4P3/8/8/8/8/8/8");
    int pawn_7th_rank = eval::evaluate_position(board);

    board.initialize_board_from_fen("8/8/8/4P3/8/8/8/8");
    int pawn_5th_rank = eval::evaluate_position(board);

    board.initialize_board_from_fen("8/8/8/8/8/8/4P3/8");
    int pawn_2nd_rank = eval::evaluate_position(board);

    assert(pawn_7th_rank > pawn_5th_rank); // Pawns worth more closer to promotion
    assert(pawn_5th_rank > pawn_2nd_rank); // And worth more advanced than starting position

    // Test 4: Full position evaluation
    // Simple position with clear material advantage
    board.initialize_board_from_fen("8/8/8/3q4/4P3/8/8/8");
    int score = eval::evaluate_position(board);
    assert(score < 0); // Black should be winning (has queen vs pawn)

    // Starting position should be equal
    board.initialize_board_from_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    assert(eval::evaluate_position(board) == 0);

    // Position with multiple pieces
    board.initialize_board_from_fen("8/8/8/2bp4/4P3/2N5/8/8");
    int complex_score = eval::evaluate_position(board);
    assert(complex_score < 0); // Black should be slightly better (bishop + pawn vs knight + pawn)

    cout << "✓ Evaluation tests passed\n"
         << endl;
}

void run_eval_test_suite() {
    cout << "\nRunning evaluation tests...\n"
         << endl;
    test_evaluation();
}

} // namespace bitboard_tests

#endif