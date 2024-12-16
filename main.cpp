#include "board_t.hpp"
#include "eval.hpp"
#include "file_util.hpp"
#include "move_t.hpp"
#include "moves.hpp"
#include "tests.hpp"
#include <locale>
#include <random>

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

    // Initalising the board
    board_t board;
    board.initialize_starting_board();

    vector<string> input_moves = read_moves_from_input_file(&input_file_name);
    for (auto& move : input_moves) {
        moves::make_move(board, parse_move(move));
    }
    board.pretty_print_board();

    // Playing the moves from the input file
    Color color_to_move = (input_moves.size() % 2 == 0) ? Color::WHITE : Color::BLACK;
    vector<move_t> all_moves = moves::generate_all_moves_for_color(board, color_to_move);

    // Get a random index in range [0, all_moves.size()-1] (copied from the internet)
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> distrib(0, all_moves.size() - 1);
    int random_int = distrib(gen);

    // Write the random move to the output file
    move_t random_move = all_moves[random_int];
    string random_move_str = encode_move(random_move);

    // Writing the random move to the output file
    write_move_to_output_file(&output_file_name, &random_move_str);

    return 0;
}
