#include "board_t.hpp"
#include "engine.hpp"
#include "eval.hpp"
#include "file_util.hpp"
#include "move_t.hpp"
#include "moves.hpp"
#include "tests.hpp"

int main(int argc, char const* argv[]) // ./BlueHerring -H history.csv -m move.csv
{
    if (argc != 5) {
        printf("Wrong input size!, %i", argc);
        return -1;
    }
    std::locale::global(std::locale("en_US.UTF-8")); // To enable printing of unicode characters
    string input_file_name  = argv[2];
    string output_file_name = argv[4];

    // coordinate_tests::run_rules_test_suite();
    // coordinate_tests::run_speed_test_suite();
    // coordinate_tests::run_perft_suite();

    // bitboard_tests::run_perft_suite();
    // bitboard_tests::run_rules_test_suite();
    bitboard_tests::run_speed_test_suite();

    // bitboard_t bitboard;
    // bitboard.initialize_starting_board();
    // bitboard.pretty_print_board();
    // vector<string> string_moves   = read_moves_from_input_file(&input_file_name);
    // vector<bitboard_move_t> moves = translate_to_bitboard_moves(string_moves);
    // for (const bitboard_move_t& move : moves) {
    //     bitboard_moves::make_move(bitboard, move);
    //     bitboard.pretty_print_board();
    // }
    // Color color_to_move = (moves.size() % 2 == 0) ? Color::WHITE : Color::BLACK;

    //move_t best_move     = eval::get_best_move(board, 3, color_to_move);
    //string best_move_str = encode_move(best_move);
    //write_move_to_output_file(&output_file_name, &best_move_str);

    return 0;
}