#include "board_t.hpp"
#include "engine.hpp"
#include "eval.hpp"
#include "file_util.hpp"
#include "move_t.hpp"
#include "moves.hpp"
#include "tests.hpp"
#include "bit_engine.hpp"
#include "chrono"

int main(int argc, char const* argv[]) // ./BlueHerring -H history.csv -m move.csv
{
    if (argc != 5) {
        printf("Wrong input size!, %i", argc);
        return -1;
    }
    std::locale::global(std::locale("en_US.UTF-8")); // To enable printing of unicode characters
    string input_file_name  = argv[2];
    string output_file_name = argv[4];

    coordinate_tests::run_speed_test_suite();
    bitboard_tests::run_speed_test_suite();

    // coordinate_tests::run_rules_test_board();
    // coordinate_tests::run_perft_board();

    // bitboard_tests::run_rules_test_board();
    // bitboard_tests::run_perft_board();
    // bitboard_tests::run_speed_test_board();

    // bitboard.pretty_print_board();

    // vector<string> string_moves   = read_moves_from_input_file(&input_file_name);
    // vector<bitboard_move_t> moves = translate_to_bitboard_moves(string_moves);
    // for (const bitboard_move_t& move : moves) {
    //     bitboard_moves::make_move(bitboard, move);
    //     bitboard.pretty_print_board();
    // }

    // std::cout << "Testing best_move speed for board_t vs bitboard_t" << "\n" << std::endl;

    // // Color color_to_move = (move.size() % 2 == 0) ? Color::WHITE : Color::BLACK;
    // Color color_to_move = Color::WHITE;

    // // Timing bitboard_t
    // auto bitboard_start    = chrono::high_resolution_clock::now();

    // bitboard_move_t best_bit_move     = bit_eval::get_best_move(bitboard, 5, color_to_move);
    // move_t best_bit_move_t = bitboard_to_coordinate_move(best_bit_move);
    // string best_bit_move_str = encode_move(best_bit_move_t);

    // auto bitboard_end      = chrono::high_resolution_clock::now();
    // auto bitboard_duration = chrono::duration_cast<chrono::milliseconds>(bitboard_end - bitboard_start);

    // std::cout << "bitboard_t, depth 3 took: " << bitboard_duration.count() << " to find: " << best_bit_move_str << std::endl;



    // // Timing board_t
    // auto board_start    = chrono::high_resolution_clock::now();

    // move_t best_move     = eval::get_best_move(board, 5, color_to_move);
    // string best_move_str = encode_move(best_move);

    // auto board_end      = chrono::high_resolution_clock::now();
    // auto board_duration = chrono::duration_cast<chrono::milliseconds>(board_end - board_start);

    // std::cout << "board_t, depth 3 took: " << board_duration.count() << " to find: "<< best_move_str << std::endl;

    // write_move_to_output_file(&output_file_name, &best_move_str);

    return 0;
}