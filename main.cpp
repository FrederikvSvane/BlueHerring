#include "board_t.hpp"
#include "eval.hpp"
#include "file_util.hpp"
#include "move_t.hpp"
#include "moves.hpp"
#include "tests.hpp"
#include <locale>

int main(int argc, char const* argv[]) // ./BlueHerring -H input.txt -m output_example.txt
{
    if (argc != 5) {
        printf("Wrong input size!, %i", argc);
        return -1;
    }
    std::locale::global(std::locale("en_US.UTF-8")); // To enable printing of unicode characters
    string input_file_name  = argv[2];
    string output_file_name = argv[4];

    // tests::run_rules_test_suite();
    // tests::run_speed_test_suite();
    // tests::run_perft_suite();

    board_t board;
    board.initialize_starting_board();
    board.pretty_print_board();

    vector<string> input_moves = read_moves_from_input_file(&input_file_name);
    for (auto& move : input_moves) {
        moves::make_move(board, parse_move(move));
    }
    board.pretty_print_board();

    string output_move = "e2e4";
    write_move_to_output_file(&output_file_name, &output_move);

    return 0;
}
