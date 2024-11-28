#include "board_t.hpp"
#include "file_util.hpp"

int main(int argc, char const* argv[]) // ./BlueHerring -H input.txt -m output_example.txt
{
    if (argc != 5) {
        printf("Wrong input size!, %i", argc);
        return -1;
    }
    string input_file_name = argv[2];
    string output_file_name = argv[4];
    vector<string> moves = read_moves_from_input_file(&input_file_name);

    board_t board{};
    board.initialize_starting_board();
    board.pretty_print_board();

    string best_move; // the magnum opus
    best_move = "e3e4";
    write_move_to_output_file(&output_file_name, &best_move);

    // For printing the input file line by line (testing)
    /*
    for (auto& move : moves) {
        cout << move << endl;
    }
    */

    return 0;
}
