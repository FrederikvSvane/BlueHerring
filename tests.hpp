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

    /* The FULL test suite (Sucessfully passes every single one as of December 14, 2024)
    Taken from https://github.com/AndyGrant/Ethereal/blob/master/src/perft/standard.epd

    vector<perft_test_case> test_cases = {
        {"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", 4, {1, 20, 400, 8902, 197281}, false},
        {"r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1", 4, {1, 48, 2039, 97862, 4085603}, false},
        {"4k3/8/8/8/8/8/8/4K2R w K - 0 1", 4, {1, 15, 66, 1197, 7059}, false},
        {"4k3/8/8/8/8/8/8/R3K3 w Q - 0 1", 4, {1, 16, 71, 1287, 7626}, false},
        {"4k2r/8/8/8/8/8/8/4K3 w k - 0 1", 4, {1, 5, 75, 459, 8290}, false},
        {"r3k3/8/8/8/8/8/8/4K3 w q - 0 1", 4, {1, 5, 80, 493, 8897}, false},
        {"4k3/8/8/8/8/8/8/R3K2R w KQ - 0 1", 4, {1, 26, 112, 3189, 17945}, false},
        {"r3k2r/8/8/8/8/8/8/4K3 w kq - 0 1", 4, {1, 5, 130, 782, 22180}, false},
        {"8/8/8/8/8/8/6k1/4K2R w K - 0 1", 4, {1, 12, 38, 564, 2219}, false},
        {"8/8/8/8/8/8/1k6/R3K3 w Q - 0 1", 4, {1, 15, 65, 1018, 4573}, false},
        {"4k2r/6K1/8/8/8/8/8/8 w k - 0 1", 4, {1, 3, 32, 134, 2073}, false},
        {"r3k3/1K6/8/8/8/8/8/8 w q - 0 1", 4, {1, 4, 49, 243, 3991}, false},
        {"r3k2r/8/8/8/8/8/8/R3K2R w KQkq - 0 1", 4, {1, 26, 568, 13744, 314346}, false},
        {"r3k2r/8/8/8/8/8/8/1R2K2R w Kkq - 0 1", 4, {1, 25, 567, 14095, 328965}, false},
        {"r3k2r/8/8/8/8/8/8/2R1K2R w Kkq - 0 1", 4, {1, 25, 548, 13502, 312835}, false},
        {"r3k2r/8/8/8/8/8/8/R3K1R1 w Qkq - 0 1", 4, {1, 25, 547, 13579, 316214}, false},
        {"1r2k2r/8/8/8/8/8/8/R3K2R w KQk - 0 1", 4, {1, 26, 583, 14252, 334705}, false},
        {"2r1k2r/8/8/8/8/8/8/R3K2R w KQk - 0 1", 4, {1, 25, 560, 13592, 317324}, false},
        {"r3k1r1/8/8/8/8/8/8/R3K2R w KQq - 0 1", 4, {1, 25, 560, 13607, 320792}, false},
        {"4k3/8/8/8/8/8/8/4K2R b K - 0 1", 4, {1, 5, 75, 459, 8290}, false},
        {"4k3/8/8/8/8/8/8/R3K3 b Q - 0 1", 4, {1, 5, 80, 493, 8897}, false},
        {"4k2r/8/8/8/8/8/8/4K3 b k - 0 1", 4, {1, 15, 66, 1197, 7059}, false},
        {"r3k3/8/8/8/8/8/8/4K3 b q - 0 1", 4, {1, 16, 71, 1287, 7626}, false},
        {"4k3/8/8/8/8/8/8/R3K2R b KQ - 0 1", 4, {1, 5, 130, 782, 22180}, false},
        {"r3k2r/8/8/8/8/8/8/4K3 b kq - 0 1", 4, {1, 26, 112, 3189, 17945}, false},
        {"8/8/8/8/8/8/6k1/4K2R b K - 0 1", 4, {1, 3, 32, 134, 2073}, false},
        {"8/8/8/8/8/8/1k6/R3K3 b Q - 0 1", 4, {1, 4, 49, 243, 3991}, false},
        {"4k2r/6K1/8/8/8/8/8/8 b k - 0 1", 4, {1, 12, 38, 564, 2219}, false},
        {"r3k3/1K6/8/8/8/8/8/8 b q - 0 1", 4, {1, 15, 65, 1018, 4573}, false},
        {"8/1n4N1/2k5/8/8/5K2/1N4n1/8 w - - 0 1", 4, {1, 14, 195, 2760, 38675}, false},
        {"8/1k6/8/5N2/8/4n3/8/2K5 w - - 0 1", 4, {1, 11, 156, 1636, 20534}, false},
        {"8/8/4k3/3Nn3/3nN3/4K3/8/8 w - - 0 1", 4, {1, 19, 289, 4442, 73584}, false},
        {"K7/8/2n5/1n6/8/8/8/k6N w - - 0 1", 4, {1, 3, 51, 345, 5301}, false},
        {"k7/8/2N5/1N6/8/8/8/K6n w - - 0 1", 4, {1, 17, 54, 835, 5910}, false},
        {"8/1n4N1/2k5/8/8/5K2/1N4n1/8 b - - 0 1", 4, {1, 15, 193, 2816, 40039}, false},
        {"8/1k6/8/5N2/8/4n3/8/2K5 b - - 0 1", 4, {1, 16, 180, 2290, 24640}, false},
        {"8/8/3K4/3Nn3/3nN3/4k3/8/8 b - - 0 1", 4, {1, 4, 68, 1118, 16199}, false},
        {"K7/8/2n5/1n6/8/8/8/k6N b - - 0 1", 4, {1, 17, 54, 835, 5910}, false},
        {"k7/8/2N5/1N6/8/8/8/K6n b - - 0 1", 4, {1, 3, 51, 345, 5301}, false},
        {"B6b/8/8/8/2K5/4k3/8/b6B w - - 0 1", 4, {1, 17, 278, 4607, 76778}, false},
        {"8/8/1B6/7b/7k/8/2B1b3/7K w - - 0 1", 4, {1, 21, 316, 5744, 93338}, false},
        {"k7/B7/1B6/1B6/8/8/8/K6b w - - 0 1", 4, {1, 21, 144, 3242, 32955}, false},
        {"K7/b7/1b6/1b6/8/8/8/k6B w - - 0 1", 4, {1, 7, 143, 1416, 31787}, false},
        {"B6b/8/8/8/2K5/5k2/8/b6B b - - 0 1", 4, {1, 6, 106, 1829, 31151}, false},
        {"8/8/1B6/7b/7k/8/2B1b3/7K b - - 0 1", 4, {1, 17, 309, 5133, 93603}, false},
        {"k7/B7/1B6/1B6/8/8/8/K6b b - - 0 1", 4, {1, 7, 143, 1416, 31787}, false},
        {"K7/b7/1b6/1b6/8/8/8/k6B b - - 0 1", 4, {1, 21, 144, 3242, 32955}, false},
        {"7k/RR6/8/8/8/8/rr6/7K w - - 0 1", 4, {1, 19, 275, 5300, 104342}, false},
        {"R6r/8/8/2K5/5k2/8/8/r6R w - - 0 1", 4, {1, 36, 1027, 29215, 771461}, false},
        {"7k/RR6/8/8/8/8/rr6/7K b - - 0 1", 4, {1, 19, 275, 5300, 104342}, false},
        {"R6r/8/8/2K5/5k2/8/8/r6R b - - 0 1", 4, {1, 36, 1027, 29227, 771368}, false},
        {"6kq/8/8/8/8/8/8/7K w - - 0 1", 4, {1, 2, 36, 143, 3637}, false},
        {"6KQ/8/8/8/8/8/8/7k b - - 0 1", 4, {1, 2, 36, 143, 3637}, false},
        {"K7/8/8/3Q4/4q3/8/8/7k w - - 0 1", 4, {1, 6, 35, 495, 8349}, false},
        {"6qk/8/8/8/8/8/8/7K b - - 0 1", 4, {1, 22, 43, 1015, 4167}, false},
        {"6KQ/8/8/8/8/8/8/7k b - - 0 1", 4, {1, 2, 36, 143, 3637}, false},
        {"K7/8/8/3Q4/4q3/8/8/7k b - - 0 1", 4, {1, 6, 35, 495, 8349}, false},
        {"8/8/8/8/8/K7/P7/k7 w - - 0 1", 4, {1, 3, 7, 43, 199}, false},
        {"8/8/8/8/8/7K/7P/7k w - - 0 1", 4, {1, 3, 7, 43, 199}, false},
        {"K7/p7/k7/8/8/8/8/8 w - - 0 1", 4, {1, 1, 3, 12, 80}, false},
        {"7K/7p/7k/8/8/8/8/8 w - - 0 1", 4, {1, 1, 3, 12, 80}, false},
        {"8/2k1p3/3pP3/3P2K1/8/8/8/8 w - - 0 1", 4, {1, 7, 35, 210, 1091}, false},
        {"8/8/8/8/8/K7/P7/k7 b - - 0 1", 4, {1, 1, 3, 12, 80}, false},
        {"8/8/8/8/8/7K/7P/7k b - - 0 1", 4, {1, 1, 3, 12, 80}, false},
        {"K7/p7/k7/8/8/8/8/8 b - - 0 1", 4, {1, 3, 7, 43, 199}, false},
        {"7K/7p/7k/8/8/8/8/8 b - - 0 1", 4, {1, 3, 7, 43, 199}, false},
        {"8/2k1p3/3pP3/3P2K1/8/8/8/8 b - - 0 1", 4, {1, 5, 35, 182, 1091}, false},
        {"8/8/8/8/8/4k3/4P3/4K3 w - - 0 1", 4, {1, 2, 8, 44, 282}, false},
        {"4k3/4p3/4K3/8/8/8/8/8 b - - 0 1", 4, {1, 2, 8, 44, 282}, false},
        {"8/8/7k/7p/7P/7K/8/8 w - - 0 1", 4, {1, 3, 9, 57, 360}, false},
        {"8/8/k7/p7/P7/K7/8/8 w - - 0 1", 4, {1, 3, 9, 57, 360}, false},
        {"8/8/3k4/3p4/3P4/3K4/8/8 w - - 0 1", 4, {1, 5, 25, 180, 1294}, false},
        {"8/3k4/3p4/8/3P4/3K4/8/8 w - - 0 1", 4, {1, 8, 61, 483, 3213}, false},
        {"8/8/3k4/3p4/8/3P4/3K4/8 w - - 0 1", 4, {1, 8, 61, 411, 3213}, false},
        {"k7/8/3p4/8/3P4/8/8/7K w - - 0 1", 4, {1, 4, 15, 90, 534}, false},
        {"8/8/7k/7p/7P/7K/8/8 b - - 0 1", 4, {1, 3, 9, 57, 360}, false},
        {"8/8/k7/p7/P7/K7/8/8 b - - 0 1", 4, {1, 3, 9, 57, 360}, false},
        {"8/8/3k4/3p4/3P4/3K4/8/8 b - - 0 1", 4, {1, 5, 25, 180, 1294}, false},
        {"8/3k4/3p4/8/3P4/3K4/8/8 b - - 0 1", 4, {1, 8, 61, 411, 3213}, false},
        {"8/8/3k4/3p4/8/3P4/3K4/8 b - - 0 1", 4, {1, 8, 61, 483, 3213}, false},
        {"k7/8/3p4/8/3P4/8/8/7K b - - 0 1", 4, {1, 4, 15, 89, 537}, false},
        {"7k/3p4/8/8/3P4/8/8/K7 w - - 0 1", 4, {1, 4, 19, 117, 720}, false},
        {"7k/8/8/3p4/8/8/3P4/K7 w - - 0 1", 4, {1, 5, 19, 116, 716}, false},
        {"k7/8/8/7p/6P1/8/8/K7 w - - 0 1", 4, {1, 5, 22, 139, 877}, false},
        {"k7/8/7p/8/8/6P1/8/K7 w - - 0 1", 4, {1, 4, 16, 101, 637}, false},
        {"k7/8/8/6p1/7P/8/8/K7 w - - 0 1", 4, {1, 5, 22, 139, 877}, false},
        {"k7/8/6p1/8/8/7P/8/K7 w - - 0 1", 4, {1, 4, 16, 101, 637}, false},
        {"k7/8/8/3p4/4p3/8/8/7K w - - 0 1", 4, {1, 3, 15, 84, 573}, false},
        {"k7/8/3p4/8/8/4P3/8/7K w - - 0 1", 4, {1, 4, 16, 101, 637}, false},
        {"7k/3p4/8/8/3P4/8/8/K7 b - - 0 1", 4, {1, 5, 19, 117, 720}, false},
        {"7k/8/8/3p4/8/8/3P4/K7 b - - 0 1", 4, {1, 4, 19, 117, 712}, false},
        {"k7/8/8/7p/6P1/8/8/K7 b - - 0 1", 4, {1, 5, 22, 139, 877}, false},
        {"k7/8/7p/8/8/6P1/8/K7 b - - 0 1", 4, {1, 4, 16, 101, 637}, false},
        {"k7/8/8/6p1/7P/8/8/K7 b - - 0 1", 4, {1, 5, 22, 139, 877}, false},
        {"k7/8/6p1/8/8/7P/8/K7 b - - 0 1", 4, {1, 4, 16, 101, 637}, false},
        {"k7/8/8/3p4/4p3/8/8/7K b - - 0 1", 4, {1, 5, 15, 102, 569}, false},
        {"k7/8/3p4/8/8/4P3/8/7K b - - 0 1", 4, {1, 4, 16, 101, 637}, false},
        {"7k/8/8/p7/1P6/8/8/7K w - - 0 1", 4, {1, 5, 22, 139, 877}, false},
        {"7k/8/p7/8/8/1P6/8/7K w - - 0 1", 4, {1, 4, 16, 101, 637}, false},
        {"7k/8/8/1p6/P7/8/8/7K w - - 0 1", 4, {1, 5, 22, 139, 877}, false},
        {"7k/8/1p6/8/8/P7/8/7K w - - 0 1", 4, {1, 4, 16, 101, 637}, false},
        {"k7/7p/8/8/8/8/6P1/K7 w - - 0 1", 4, {1, 5, 25, 161, 1035}, false},
        {"k7/6p1/8/8/8/8/7P/K7 w - - 0 1", 4, {1, 5, 25, 161, 1035}, false},
        {"3k4/3pp3/8/8/8/8/3PP3/3K4 w - - 0 1", 4, {1, 7, 49, 378, 2902}, false},
        {"7k/8/8/p7/1P6/8/8/7K b - - 0 1", 4, {1, 5, 22, 139, 877}, false},
        {"7k/8/p7/8/8/1P6/8/7K b - - 0 1", 4, {1, 4, 16, 101, 637}, false},
        {"7k/8/8/1p6/P7/8/8/7K b - - 0 1", 4, {1, 5, 22, 139, 877}, false},
        {"7k/8/1p6/8/8/P7/8/7K b - - 0 1", 4, {1, 4, 16, 101, 637}, false},
        {"k7/7p/8/8/8/8/6P1/K7 b - - 0 1", 4, {1, 5, 25, 161, 1035}, false},
        {"k7/6p1/8/8/8/8/7P/K7 b - - 0 1", 4, {1, 5, 25, 161, 1035}, false},
        {"3k4/3pp3/8/8/8/8/3PP3/3K4 b - - 0 1", 4, {1, 7, 49, 378, 2902}, false},
        {"8/Pk6/8/8/8/8/6Kp/8 w - - 0 1", 4, {1, 11, 97, 887, 8048}, false},
        {"n1n5/1Pk5/8/8/8/8/5Kp1/5N1N w - - 0 1", 4, {1, 24, 421, 7421, 124608}, false},
        {"8/PPPk4/8/8/8/8/4Kppp/8 w - - 0 1", 4, {1, 18, 270, 4699, 79355}, false},
        {"n1n5/PPPk4/8/8/8/8/4Kppp/5N1N w - - 0 1", 4, {1, 24, 496, 9483, 182838}, false},
        {"8/Pk6/8/8/8/8/6Kp/8 b - - 0 1", 4, {1, 11, 97, 887, 8048}, false},
        {"n1n5/1Pk5/8/8/8/8/5Kp1/5N1N b - - 0 1", 4, {1, 24, 421, 7421, 124608}, false},
        {"8/PPPk4/8/8/8/8/4Kppp/8 b - - 0 1", 4, {1, 18, 270, 4699, 79355}, false},
        {"n1n5/PPPk4/8/8/8/8/4Kppp/5N1N b - - 0 1", 4, {1, 24, 496, 9483, 182838}, false},
    };

    */

    for (const auto& test : test_cases) {
        board_t board;
        board.initialize_board_from_fen(test.fen);

        cout << "\nRunning perft for WHITE with DEPTH=" << test.max_depth << ".\nPosition:\n";
        board.pretty_print_board();
        cout << endl;

        bool test_passed = true;
        for (int depth = 0; depth <= test.max_depth; depth++) {

            auto [result_int, move_breakdown] = perft(board, depth, board.active_color, test.single_color_scenario);

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
    board.initialize_board_from_fen("4r3/3P4/8/8/8/8/8/8");
    board.pretty_print_board();

    board_t initial_board = board;

    // Promote pawn to queen while capturing
    move_t move{3, 6, 4, 7, PieceType::QUEEN};
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

void test_pawn_double_move_sets_en_passant() {
    board_t board;
    board.initialize_board_from_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR");
    board.pretty_print_board();

    board_t initial_board = board;

    // Double move white pawn
    move_t move{4, 1, 4, 3, PieceType::EMPTY};
    piece_t captured = moves::make_move(board, move);
    board.pretty_print_board();

    // Verify en passant square is set correctly
    assert(board.en_passant_x == 4);
    assert(board.en_passant_y == 2);
    assert(board.at(4, 1).piece.type == PieceType::EMPTY);
    assert(board.at(4, 3).piece.type == PieceType::PAWN);

    // Undo the move
    moves::undo_move(board, move, captured);
    board.pretty_print_board();

    // Verify board and en passant state are back to initial
    assert(board.en_passant_x == initial_board.en_passant_x);
    assert(board.en_passant_y == initial_board.en_passant_y);
    assert(compare_boards(board, initial_board));
}

void test_multiple_moves_state_history() {
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
        board.pretty_print_board();
    }

    // Undo moves in reverse order
    for (int i = moves.size() - 1; i >= 0; i--) {
        moves::undo_move(board, moves[i], captured_pieces[i]);
        board.pretty_print_board();

        // Verify state is restored correctly
        assert(board.white_king_side_castle == states[i].white_king_side_castle);
        assert(board.white_queen_side_castle == states[i].white_queen_side_castle);
        assert(board.black_king_side_castle == states[i].black_king_side_castle);
        assert(board.black_queen_side_castle == states[i].black_queen_side_castle);
        assert(board.en_passant_x == states[i].en_passant_x);
        assert(board.en_passant_y == states[i].en_passant_y);
    }
}

void test_rook_movement_updates_castling_rights() {
    board_t board;
    board.initialize_board_from_fen("r3k2r/8/8/8/8/8/8/R3K2R w KQkq - 0 1");
    board.pretty_print_board();

    board_t initial_board = board;

    // Move white kingside rook
    move_t move{7, 0, 7, 4, PieceType::EMPTY};
    piece_t captured = moves::make_move(board, move);
    board.pretty_print_board();

    // Verify castling rights updated
    assert(board.white_king_side_castle == false);
    assert(board.white_queen_side_castle == true);

    // Undo move
    moves::undo_move(board, move, captured);
    board.pretty_print_board();

    // Verify castling rights restored
    assert(compare_boards(board, initial_board));
}

void test_king_diagonal_capture() {
    board_t board;
    board.initialize_board_from_fen("4k3/8/8/8/8/8/3p4/4K3 w - - 0 1");
    board.pretty_print_board();

    board_t initial_board = board;

    // King captures diagonally
    move_t move{4, 0, 3, 1, PieceType::EMPTY};
    piece_t captured = moves::make_move(board, move);
    board.pretty_print_board();

    // Verify capture
    assert(board.at(4, 0).piece.type == PieceType::EMPTY);
    assert(board.at(3, 1).piece.type == PieceType::KING);
    assert(board.at(3, 1).piece.color == Color::WHITE);

    // Undo move
    moves::undo_move(board, move, captured);
    board.pretty_print_board();

    // Verify restoration
    assert(compare_boards(board, initial_board));
}

void test_en_passant_symmetry() {
    // Test en passant from both sides and colors
    board_t board;
    board.initialize_board_from_fen("8/8/8/3pP3/8/8/8/8 w - d6 0 1");
    board.pretty_print_board();

    board_t initial_board = board;

    // White captures black pawn en passant
    move_t white_capture{4, 4, 3, 5, PieceType::EMPTY};
    piece_t white_captured = moves::make_move(board, white_capture);
    board.pretty_print_board();

    // Verify white en passant capture
    assert(board.at(4, 4).piece.type == PieceType::EMPTY);
    assert(board.at(3, 4).piece.type == PieceType::EMPTY);
    assert(board.at(3, 5).piece.type == PieceType::PAWN);
    assert(board.at(3, 5).piece.color == Color::WHITE);
    assert(white_captured.type == PieceType::PAWN);
    assert(white_captured.color == Color::BLACK);

    // Undo white capture
    moves::undo_move(board, white_capture, white_captured);
    board.pretty_print_board();
    assert(compare_boards(board, initial_board));

    // Now test black en passant capture
    board.initialize_board_from_fen("8/8/8/8/4pP3/8/8/8 b - f3 0 1");
    board.pretty_print_board();

    initial_board = board;

    // Black captures white pawn en passant
    move_t black_capture{4, 3, 5, 2, PieceType::EMPTY};
    piece_t black_captured = moves::make_move(board, black_capture);
    board.pretty_print_board();

    // Verify black en passant capture
    assert(board.at(4, 3).piece.type == PieceType::EMPTY);
    assert(board.at(5, 3).piece.type == PieceType::EMPTY);
    assert(board.at(5, 2).piece.type == PieceType::PAWN);
    assert(board.at(5, 2).piece.color == Color::BLACK);
    assert(black_captured.type == PieceType::PAWN);
    assert(black_captured.color == Color::WHITE);

    // Undo black capture
    moves::undo_move(board, black_capture, black_captured);
    board.pretty_print_board();
    assert(compare_boards(board, initial_board));
}

void test_en_passant_window() {
    // Test that en passant is only available immediately after double pawn move
    board_t board;
    board.initialize_board_from_fen("8/8/8/8/pP5/8/8/8 w - a3 0 1");
    board.pretty_print_board();

    // Make an unrelated move
    move_t unrelated_move{1, 3, 1, 4, PieceType::EMPTY};
    piece_t captured = moves::make_move(board, unrelated_move);
    board.pretty_print_board();

    // Verify en passant square is reset
    assert(board.en_passant_x == -1);
    assert(board.en_passant_y == -1);

    moves::undo_move(board, unrelated_move, captured);
    board.pretty_print_board();

    // Verify en passant square is restored
    assert(board.en_passant_x == 0);
    assert(board.en_passant_y == 2);
}

void test_check_detection() {
    // Test various check scenarios
    vector<pair<string, bool>> check_positions = {
        // FEN, is white in check?
        {"k7/8/8/8/8/8/8/K6R b - - 0 1", false},  // Rook not giving check
        {"k7/8/8/8/R7/8/8/K7 b - - 0 1", true},   // Rook giving check
        {"k7/8/8/8/8/8/8/K5B1 b - - 0 1", false}, // Bishop not giving check
        {"k7/8/8/8/8/8/6B1/K7 b - - 0 1", true},  // Bishop giving check
        {"k7/2N5/8/8/8/8/8/K7 b - - 0 1", true},  // Knight giving check
        {"k7/8/8/8/8/8/1P6/K7 b - - 0 1", false}, // Pawn not giving check
        {"k7/1P6/8/8/8/8/8/K7 b - - 0 1", true},  // Pawn giving check
        {"k7/8/8/8/8/8/8/KQ6 b - - 0 1", false},  // Queen not giving check
        {"k7/8/8/8/8/8/Q7/K7 b - - 0 1", true}    // Queen giving check
    };

    for (const auto& [fen, expected_check] : check_positions) {
        board_t board;
        board.initialize_board_from_fen(fen);
        board.pretty_print_board();
        bool actual_check = moves::is_in_check(board, Color::BLACK);
        assert(actual_check == expected_check);
        std::cout << "Result: " << (actual_check ? "Check" : "No check")
                  << "\nExpected: " << (expected_check ? "Check" : "No check") << std::endl;
    }
}

void test_checkmate_position() {
    // Test a few checkmate and near-checkmate positions
    vector<pair<string, pair<bool, bool>>> mate_positions = {
        // FEN, {is_in_check, has_legal_moves}
        {"k7/8/8/8/8/8/R7/1R5K b - - 0 1", {true, false}},   // Basic back rank mate
        {"k7/8/8/8/8/8/R7/R6K b - - 0 1", {true, true}},     // Check but can escape
        {"k7/2K5/8/8/8/8/8/R7 b - - 0 1", {true, false}},    // Mate with king support
        {"6rk/5Npp/8/8/8/8/8/6K1 w - - 0 1", {true, false}}, // Smothered mate
    };

    for (const auto& [fen, expected] : mate_positions) {
        board_t board;
        board.initialize_board_from_fen(fen);
        board.pretty_print_board();

        const auto& [expected_check, expected_has_moves] = expected;
        bool actual_check                                = moves::is_in_check(board, Color::BLACK);
        vector<move_t> legal_moves                       = moves::generate_all_moves_for_color(board, Color::BLACK);
        bool has_legal_moves                             = !legal_moves.empty();
        bool is_in_checkmate                             = (!has_legal_moves && actual_check);

        assert(actual_check == expected_check);
        // cout << "Number of legal moves: " << legal_moves.size() << endl;
        // for(auto& move : legal_moves){
        //     cout << encode_move(move) << endl;
        // }
        assert(has_legal_moves == expected_has_moves);

        cout << "In check: " << (actual_check ? "true" : "false")
             << "\nNumber of legal moves: " << legal_moves.size()
             << "\nCheckmate: " << (is_in_checkmate ? "Yes" : "No") << endl;
    }
}

void test_discovered_check() {
    // Test discovered check scenarios
    board_t board;
    board.initialize_board_from_fen("k7/8/2P5/3B4/8/8/8/K7 w - - 0 1");
    board.pretty_print_board();

    // Moving bishop reveals check from rook
    move_t discover_check{2, 5, 2, 6, PieceType::EMPTY};
    piece_t captured = moves::make_move(board, discover_check);
    board.pretty_print_board();

    assert(moves::is_in_check(board, Color::BLACK));

    moves::undo_move(board, discover_check, captured);
    board.pretty_print_board();

    assert(!moves::is_in_check(board, Color::BLACK));
}

void test_en_passant_reset_after_other_move() {
    board_t board;
    board.initialize_board_from_fen("rnbqkbnr/1p1p1p1p/8/pPp1p3/8/8/P1PPP1PP/RNBQKBNR w KQkq c6 0 1");
    board.pretty_print_board();

    // Save initial state with en passant possibility
    assert(board.en_passant_x == 2);
    assert(board.en_passant_y == 5);

    // Make a move unrelated to en passant
    move_t unrelated_move{4, 1, 4, 2, PieceType::EMPTY}; // e2e3
    piece_t captured = moves::make_move(board, unrelated_move);
    board.pretty_print_board();

    // Verify en passant square is reset
    assert(board.en_passant_x == -1);
    assert(board.en_passant_y == -1);

    moves::undo_move(board, unrelated_move, captured);
    board.pretty_print_board();

    // Verify original en passant state is restored
    assert(board.en_passant_x == 2);
    assert(board.en_passant_y == 5);
}

void test_castling_rights_after_rook_capture() {
    board_t board;
    board.initialize_board_from_fen("r3k2r/8/8/8/8/8/8/R3K2R w KQkq - 0 1");
    board.pretty_print_board();

    // Capture black's kingside rook
    move_t capture_rook{7, 0, 7, 7, PieceType::EMPTY};
    piece_t captured = moves::make_move(board, capture_rook);
    board.pretty_print_board();

    // Verify black loses kingside castling rights but keeps queenside
    assert(!board.black_king_side_castle);
    assert(board.black_queen_side_castle);
    assert(!board.white_king_side_castle);
    assert(board.white_queen_side_castle);

    moves::undo_move(board, capture_rook, captured);
    board.pretty_print_board();

    // Verify all castling rights are restored
    assert(board.black_king_side_castle);
    assert(board.black_queen_side_castle);
    assert(board.white_king_side_castle);
    assert(board.white_queen_side_castle);
}

void test_sliding_piece_check_detection() {
    vector<pair<string, bool>> positions = {
        // Position, expected check status
        {"8/8/8/3k4/3R4/8/8/7K b - - 0 1", true},  // Rook horizontal check
        {"8/8/8/3k4/8/4R3/8/7K b - - 0 1", false}, // Rook not giving check
        {"8/8/8/3k4/8/5B2/8/7K b - - 0 1", true},  // Bishop diagonal check
        {"8/8/8/3k4/8/8/5B2/7K b - - 0 1", false}, // Bishop not giving check
        {"8/8/8/3kQ3/8/8/8/7K b - - 0 1", true},   // Queen horizontal check
        {"8/8/8/3k4/4Q3/8/8/7K b - - 0 1", true},  // Queen diagonal check
        {"8/8/8/3k4/8/6Q1/8/7K b - - 0 1", false}, // Queen not giving check
    };

    for (const auto& [fen, expected] : positions) {
        board_t board;
        board.initialize_board_from_fen(fen);
        board.pretty_print_board();
        bool actual = moves::is_in_check(board, Color::BLACK);
        assert(actual == expected);
    }
}

void test_pawn_check_direction() {
    vector<pair<string, bool>> positions = {
        // Position, expected check status for the king
        {"k7/1P6/8/8/8/8/8/7K b - - 0 1", true}, // White pawn checking black king
        {"k7/8/P7/8/8/8/8/7K b - - 0 1", false}, // White pawn not checking black king
        {"k7/8/8/8/8/8/6p1/7K w - - 0 1", true}, // Black pawn checking white king
        {"k7/8/8/8/8/8/p7/7K w - - 0 1", false}, // Black pawn not checking white king
    };

    for (const auto& [fen, expected] : positions) {
        board_t board;
        board.initialize_board_from_fen(fen);
        board.pretty_print_board();
        Color king_color = (fen.find('w') != string::npos) ? Color::WHITE : Color::BLACK;
        bool actual      = moves::is_in_check(board, king_color);
        assert(actual == expected);
    }
}

void test_castling_through_check() {
    board_t board;
    // Position where white can't castle kingside due to bishop attacking f1
    board.initialize_board_from_fen("r3k2r/8/8/8/8/8/4b3/R3K2R w KQkq - 0 1");
    board.pretty_print_board();

    vector<move_t> legal_moves = moves::generate_all_moves_for_color(board, Color::WHITE);

    // Check that castling moves are not included
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

void test_move_generation_in_check() {
    board_t board;
    // Position where white king is in check and only legal move is to capture the checking piece
    board.initialize_board_from_fen("k7/8/8/8/8/3b4/8/4K3 w - - 0 1");
    board.pretty_print_board();

    vector<move_t> legal_moves = moves::generate_all_moves_for_color(board, Color::WHITE);

    // Should only have moves that get out of check
    for (const auto& move : legal_moves) {
        piece_t captured = moves::make_move(board, move);
        board.pretty_print_board();
        bool still_in_check = moves::is_in_check(board, Color::WHITE);
        moves::undo_move(board, move, captured);
        board.pretty_print_board();
        assert(!still_in_check);
    }
}

void test_check_after_pawn_promotion_capture() {
    board_t board;
    // Initialize empty board first
    board.initialize_board_from_fen("8/8/8/8/8/8/4K1p1/5N2 b - - 0 1");
    board.pretty_print_board();

    // Make the pawn capture and promote to bishop
    move_t promotion_capture{6, 1, 5, 0, PieceType::BISHOP};
    piece_t captured = moves::make_move(board, promotion_capture);
    board.pretty_print_board();

    // Debug prints
    cout << "Promoted piece type: " << static_cast<int>(board.at(5, 0).piece.type) << endl;
    cout << "Promoted piece color: " << static_cast<int>(board.at(5, 0).piece.color) << endl;

    // Test if square f1 to e2 is a valid attacking line
    vector<square_t> attack_line = moves::line(board, move_t{5, 0, 4, 1, PieceType::EMPTY});
    cout << "Squares between promoted piece and king: " << attack_line.size() << endl;

    // Verify the king is in check
    assert(moves::is_in_check(board, Color::WHITE));

    // Get all legal moves for white
    vector<move_t> legal_moves = moves::generate_all_moves_for_color(board, Color::WHITE);

    // Print all legal moves for debugging
    cout << "Legal moves for White:" << endl;
    for (const auto& move : legal_moves) {
        cout << encode_move(move) << endl;
    }

    // Verify there are exactly 7 legal moves
    assert(legal_moves.size() == 7);

    // Undo the move and verify position is restored
    moves::undo_move(board, promotion_capture, captured);
    board.pretty_print_board();
    assert(!moves::is_in_check(board, Color::WHITE));
}

void run_move_test_suite() {
    cout << "\nRunning move/undo move tests...\n"
         << endl;

    run_move_test("Simple pawn move", test_simple_pawn_move);
    run_move_test("Pawn capture", test_pawn_capture);
    run_move_test("En passant", test_en_passant);
    run_move_test("Kingside castling", test_kingside_castling);
    run_move_test("Queenside castling", test_queenside_castling);
    run_move_test("Pawn promotion", test_pawn_promotion);
    run_move_test("Promotion with capture", test_promotion_with_capture);
    run_move_test("Capturing rook updates castling rights", test_capturing_rook_updates_castling_rights);
    run_move_test("Pawn double move sets en passant", test_pawn_double_move_sets_en_passant);
    run_move_test("Multiple moves state history", test_multiple_moves_state_history);
    run_move_test("Rook movement updates castling rights", test_rook_movement_updates_castling_rights);
    run_move_test("King diagonal capture", test_king_diagonal_capture);
    run_move_test("En passant symmetry", test_en_passant_symmetry);
    run_move_test("En passant window", test_en_passant_window);
    run_move_test("Check detection", test_check_detection);
    run_move_test("Checkmate position", test_checkmate_position);
    run_move_test("Discovered check", test_discovered_check);
    run_move_test("En passant reset after other move", test_en_passant_reset_after_other_move);
    run_move_test("Castling rights after rook capture", test_castling_rights_after_rook_capture);
    run_move_test("Sliding piece check detection", test_sliding_piece_check_detection);
    run_move_test("Pawn check direction", test_pawn_check_direction);
    run_move_test("Castling through check", test_castling_through_check);
    run_move_test("Move generation in check", test_move_generation_in_check);
    run_move_test("Pawn promotion move into check", test_check_after_pawn_promotion_capture);
}

void run_speed_test_suite() {
    vector<pair<string, int>> test_positions = {
        // Starting position
        {"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", 5},

        // Middle game position
        {"r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1", 4},

        // End game position
        {"8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1", 5},
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