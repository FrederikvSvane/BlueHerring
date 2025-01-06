#include "board_t.hpp"
#include "eval.hpp"
#include "file_util.hpp"
#include "move_t.hpp"
#include "moves.hpp"
#include "engine.hpp"
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

    // create a new file called that and write the one move

    // tests::run_rules_test_suite();
    // tests::run_speed_test_suite();
    // tests::run_perft_suite();

    // Initalising the board
    board_t board;
    board.initialize_starting_board();

    // std::cout << "Starting position evaluation: " << eval::evaluate_position(board) << std::endl;

    vector<string> string_moves = read_moves_from_input_file(&input_file_name);
    vector<move_t> moves        = translate_moves(string_moves);

    for (const move_t& move : moves) {
        moves::make_move(board, move);
    }

    Color color_to_move = (moves.size() % 2 == 0) ? Color::WHITE : Color::BLACK;

    move_t best_move = eval::get_best_move(board, 3, color_to_move);
    string best_move_str = encode_move(best_move);

    write_move_to_output_file(&output_file_name, &best_move_str);

    return 0;
}