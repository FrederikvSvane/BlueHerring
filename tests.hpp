#ifndef tests_hpp
#define tests_hpp

#include "moves.hpp"
#include <chrono>

namespace tests {

struct perft_test_case {
    string fen;
    int max_depth;
    vector<uint64_t> expected_nodes;
    bool single_color_scenario;
};

uint64_t perft(board_t& board, int depth, Color color, bool single_color_only) {
    if (depth == 0)
        return 1;

    uint64_t nodes       = 0;
    vector<move_t> moves = moves::generate_all_moves_for_color(board, color);

    for (const move_t& move : moves) {
        piece_t captured_piece = moves::make_move(board, move);
        // Only switch color if not single_color_only
        nodes += perft(board, depth - 1, single_color_only ? color : (color == Color::WHITE ? Color::BLACK : Color::WHITE), single_color_only);
        moves::undo_move(board, move, captured_piece);
    }

    return nodes;
}

void run_test_suite() {
    auto start_time         = std::chrono::high_resolution_clock::now();
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
            uint64_t result = perft(board, depth, Color::WHITE, test.single_color_scenario);
            total_node_sum += result;
            bool passed = result == test.expected_nodes[depth];
            test_passed &= passed;

            cout << "Depth " << depth << ": Result: " << result
                 << ", Expected: " << test.expected_nodes[depth]
                 << (passed ? " ✓" : " ✗")
                 << endl;
        }
        cout << (test_passed ? "\nSuccess!\n" : "\nFailed!\n") << endl;
    }

    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(end_time - start_time);
    cout << "Test suite completed in " << duration.count() << " seconds" << endl;

    if (duration.count() > 0) {
        double nps = static_cast<double>(total_node_sum) / duration.count();
        cout << "Nodes per second (NPS): " << nps << endl;
    } else {
        cout << "Duration too short to calculate NPS." << endl;
    }
}

} // namespace tests

#endif