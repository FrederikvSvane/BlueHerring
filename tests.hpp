#ifndef tests_hpp
#define tests_hpp

#include "moves.hpp"
#include <cassert>
#include <chrono>
#include <functional>
#include <string>

namespace tests {

struct perft_test_case {
    string fen;
    int max_depth;
    vector<uint64_t> expected_nodes;
    bool single_color_scenario;
};

pair<uint64_t, vector<move_t>> perft(board_t& board, int depth, Color color, bool single_color_only) {
    if (depth == 0)
        return {1, {}};

    uint64_t nodes       = 0;
    vector<move_t> moves = moves::generate_all_moves_for_color(board, color);

    for (const move_t& move : moves) {
        piece_t captured_piece = moves::make_move(board, move);
        // Only switch color if not single_color_only
        nodes += perft(board, depth - 1, single_color_only ? color : (color == Color::WHITE ? Color::BLACK : Color::WHITE), single_color_only).first;
        moves::undo_move(board, move, captured_piece);
    }

    return {nodes, moves};
}

void run_perft_suite() {
    auto start_time         = chrono::high_resolution_clock::now();
    uint64_t total_node_sum = 0;

    vector<perft_test_case> test_cases = {
        // These are taken from https://www.chessprogramming.org/Perft_Results
        {"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR", 4, {1, 20, 400, 8902, 197281}, false},
        {"r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R", 3, {1, 48, 2039, 97862}, false},
        {"8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8", 4, {1, 14, 191, 2812, 43238}, false},
        {"r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1", 4, {1, 6, 264, 9467, 422333}, false},
        {"rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R", 4, {1, 44, 1486, 62379, 2103487}, false},
        {"r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1", 4, {1, 46, 2079, 89890, 3894594}, false},
        // These ones from http://www.rocechess.ch/perft.html
        {"r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R", 4, {1, 48, 2039, 97862, 4085603}, false},
        {"n1n5/PPPk4/8/8/8/8/4Kppp/5N1N", 5, {1, 24, 496, 9483, 182838, 3605103}, false},
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

            auto [result_int, result_vector] = perft(board, depth, Color::WHITE, test.single_color_scenario);

            total_node_sum += result_int;
            bool passed = result_int == test.expected_nodes[depth];
            test_passed &= passed;

            cout << "Depth " << depth << ": Result: " << result_int
                 << ", Expected: " << test.expected_nodes[depth]
                 << (passed ? " ✓" : " ✗")
                 << endl;
        }
        cout << (test_passed ? "\nSuccess!\n" : "\nFailed!\n") << endl;
    }

    auto end_time = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::seconds>(end_time - start_time);
    cout << "Test suite completed in " << duration.count() << " seconds" << endl;

    if (duration.count() > 0) {
        double nps = static_cast<double>(total_node_sum) / duration.count();
        cout << "Nodes per second (NPS): " << nps << endl;
    } else {
        cout << "Duration too short to calculate NPS." << endl;
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

void test_simple_pawn_move() {
    board_t board;
    board.initialize_starting_board();
    board.pretty_print_board();

    // Save initial state
    board_t initial_board = board;

    // Make a simple pawn move: e2 to e4
    move_t move{4, 1, 4, 3, PieceType::EMPTY};
    piece_t captured = moves::make_move(board, move);

    board.pretty_print_board();

    // Verify the move
    assert(board.at(4, 1).piece.type == PieceType::EMPTY);
    assert(board.at(4, 3).piece.type == PieceType::PAWN);
    assert(board.at(4, 3).piece.color == Color::WHITE);
    assert(captured.type == PieceType::EMPTY);
    assert(board.en_passant_x == 4);
    assert(board.en_passant_y == 2);

    // Undo the move
    moves::undo_move(board, move, captured);

    board.pretty_print_board();

    // Verify board is back to initial state
    assert(compare_boards(board, initial_board));
}

void test_pawn_capture() {
    board_t board;
    board.initialize_board_from_fen("rnbqkbnr/ppp1pppp/8/3p4/4P3/8/PPPP1PPP/RNBQKBNR");
    board.pretty_print_board();

    board_t initial_board = board;

    // Capture black pawn on d5 with white pawn on e4
    move_t move{4, 3, 3, 4, PieceType::EMPTY};
    piece_t captured = moves::make_move(board, move);

    board.pretty_print_board();

    // Verify capture
    assert(board.at(4, 3).piece.type == PieceType::EMPTY);
    assert(board.at(3, 4).piece.type == PieceType::PAWN);
    assert(board.at(3, 4).piece.color == Color::WHITE);
    assert(captured.type == PieceType::PAWN);
    assert(captured.color == Color::BLACK);

    // Undo the capture
    moves::undo_move(board, move, captured);

    board.pretty_print_board();

    // Verify board is back to initial state
    assert(compare_boards(board, initial_board));
}

void test_en_passant() {
    board_t board;
    board.initialize_board_from_fen("rnbqkbnr/ppp1p1pp/8/3pPp2/8/8/PPPP1PPP/RNBQKBNR w KQkq f6 0 1");
    board.pretty_print_board();

    board.en_passant_x = 5;
    board.en_passant_y = 5;

    board_t initial_board = board;

    // White pawn captures black pawn en passant
    move_t move{4, 4, 5, 5, PieceType::EMPTY};
    piece_t captured = moves::make_move(board, move);
    board.pretty_print_board();

    // Verify en passant capture
    assert(board.at(4, 4).piece.type == PieceType::EMPTY);
    assert(board.at(5, 5).piece.type == PieceType::PAWN);
    assert(board.at(5, 5).piece.color == Color::WHITE);
    assert(board.at(5, 4).piece.type == PieceType::EMPTY);
    assert(captured.type == PieceType::PAWN);
    assert(captured.color == Color::BLACK);

    // Undo the en passant
    moves::undo_move(board, move, captured);
    board.pretty_print_board();

    // Verify board is back to initial state
    assert(compare_boards(board, initial_board));
}

void test_kingside_castling() {
    board_t board;
    board.initialize_board_from_fen("rnbqk2r/ppppbppp/5n2/4p3/4P3/5N2/PPPPBPPP/RNBQK2R");
    board.pretty_print_board();

    board_t initial_board = board;

    // White kingside castle
    move_t move{4, 0, 6, 0, PieceType::EMPTY};
    piece_t captured = moves::make_move(board, move);
    board.pretty_print_board();

    // Verify castling
    assert(board.at(4, 0).piece.type == PieceType::EMPTY);
    assert(board.at(6, 0).piece.type == PieceType::KING);
    assert(board.at(7, 0).piece.type == PieceType::EMPTY);
    assert(board.at(5, 0).piece.type == PieceType::ROOK);
    assert(!board.white_king_side_castle);
    assert(!board.white_queen_side_castle);

    // Undo the castling
    moves::undo_move(board, move, captured);
    board.pretty_print_board();

    // Verify board is back to initial state
    assert(compare_boards(board, initial_board));
}

void test_queenside_castling() {
    board_t board;
    board.initialize_board_from_fen("r3kbnr/pppqpppp/2n5/3p4/3P4/2N5/PPPQPPPP/R3KBNR");
    board.pretty_print_board();

    board_t initial_board = board;

    // White queenside castle
    move_t move{4, 0, 2, 0, PieceType::EMPTY};
    piece_t captured = moves::make_move(board, move);
    board.pretty_print_board();

    // Verify castling
    assert(board.at(4, 0).piece.type == PieceType::EMPTY);
    assert(board.at(2, 0).piece.type == PieceType::KING);
    assert(board.at(0, 0).piece.type == PieceType::EMPTY);
    assert(board.at(3, 0).piece.type == PieceType::ROOK);
    assert(!board.white_king_side_castle);
    assert(!board.white_queen_side_castle);

    // Undo the castling
    moves::undo_move(board, move, captured);
    board.pretty_print_board();

    // Verify board is back to initial state
    assert(compare_boards(board, initial_board));
}

void test_pawn_promotion() {
    board_t board;
    board.initialize_board_from_fen("8/4P3/8/8/8/8/8/8");
    board.pretty_print_board();

    board_t initial_board = board;

    // Promote pawn to queen
    move_t move{4, 6, 4, 7, PieceType::QUEEN};
    piece_t captured = moves::make_move(board, move);
    board.pretty_print_board();

    // Verify promotion
    assert(board.at(4, 6).piece.type == PieceType::EMPTY);
    assert(board.at(4, 7).piece.type == PieceType::QUEEN);
    assert(board.at(4, 7).piece.color == Color::WHITE);

    // Undo the promotion
    moves::undo_move(board, move, captured);
    board.pretty_print_board();

    // Verify board is back to initial state
    assert(compare_boards(board, initial_board));
}

void test_promotion_with_capture() {
    board_t board;
    board.initialize_board_from_fen("4r3/4P3/8/8/8/8/8/8");
    board.pretty_print_board();

    board_t initial_board = board;

    // Promote pawn to queen while capturing
    move_t move{4, 6, 4, 7, PieceType::QUEEN};
    piece_t captured = moves::make_move(board, move);
    board.pretty_print_board();

    // Verify promotion and capture
    assert(board.at(4, 6).piece.type == PieceType::EMPTY);
    assert(board.at(4, 7).piece.type == PieceType::QUEEN);
    assert(board.at(4, 7).piece.color == Color::WHITE);
    assert(captured.type == PieceType::ROOK);
    assert(captured.color == Color::BLACK);

    // Undo the promotion capture
    moves::undo_move(board, move, captured);
    board.pretty_print_board();

    // Verify board is back to initial state
    assert(compare_boards(board, initial_board));
}

void test_capturing_rook_updates_castling_rights() {
    board_t board;
    board.initialize_board_from_fen("r3k2r/8/8/8/8/8/8/R3K2R");
    board.pretty_print_board();

    board_t initial_board = board;

    // White rook captures black kingside rook
    move_t move{7, 0, 7, 7, PieceType::EMPTY};
    piece_t captured = moves::make_move(board, move);
    board.pretty_print_board();

    // Verify castling rights
    assert(board.black_king_side_castle == false);
    assert(board.white_king_side_castle == false);

    // Undo the capture
    moves::undo_move(board, move, captured);
    board.pretty_print_board();

    // Verify board and castling rights are back to initial state
    assert(compare_boards(board, initial_board));
}

void run_move_test_suite() {
    cout << "\nRunning move/unmove tests...\n"
         << endl;

    run_move_test("Simple pawn move", test_simple_pawn_move);
    run_move_test("Pawn capture", test_pawn_capture);
    run_move_test("En passant", test_en_passant);
    run_move_test("Kingside castling", test_kingside_castling);
    run_move_test("Queenside castling", test_queenside_castling);
    run_move_test("Pawn promotion", test_pawn_promotion);
    run_move_test("Promotion with capture", test_promotion_with_capture);
    run_move_test("Capturing rook updates castling rights", test_capturing_rook_updates_castling_rights);
}

} // namespace tests

#endif