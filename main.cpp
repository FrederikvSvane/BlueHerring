#include "board_t.hpp"
#include "eval.hpp"
#include "file_util.hpp"
#include "move_t.hpp"
#include <locale>
#include "moves.hpp"

int main(int argc, char const* argv[]) // ./BlueHerring -H input.txt -m output_example.txt
{
    if (argc != 5) {
        printf("Wrong input size!, %i", argc);
        return -1;
    }
    std::locale::global(std::locale("en_US.UTF-8")); // To enable printing of unicode characters
    string input_file_name  = argv[2];
    string output_file_name = argv[4];

    board_t board{};
    board.initialize_starting_board();
    board.pretty_print_board();

    // test_perft();



    // std::cout << "Starting position evaluation: " << eval::evaluate_position(board) << std::endl;

    vector<string> string_moves = read_moves_from_input_file(&input_file_name);
    vector<move_t> moves        = translate_moves(string_moves);

    // For printing the input file line by line (testing)
    cout << endl;
    cout << "Making the following moves:" << endl;
    for (const string& move : string_moves) {
        cout << move << endl;
    }

    for (const move_t& move : moves) {
        make_move(board, move);
        board.history.push_back(move);
    }

    board.pretty_print_board();

    // cout << board.is_in_check(Color::NONE) << ", " << board.is_move_legal(move_t{3, 1, 3, 2, PieceType::EMPTY}) << std::endl;
    // std::cout << "Final position evaluation: " << eval::evaluate_position(board) << std::endl;

    // move_t m = {7, 0, 0, 0, PieceType::BISHOP, false};
    // printf("%i, %i\n", m.from_x, m.from_y);
    // printf("%i, %i\n", m.to_x, m.to_y);

    // std::cout << "Vector elements: ";
    // for (square_t& element : board.line(m)) {
    //     std::cout << element.x << "," << element.y << " ";
    // }
    // std::cout << std::endl;

    // string best_move; // the magnum opus
    // best_move = "e3e4";
    // write_move_to_output_file(&output_file_name, &best_move);

    return 0;
}
