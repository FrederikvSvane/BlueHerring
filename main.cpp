#include "board_t.hpp"
#include "chrono"
#include "engine.hpp"
#include "eval.hpp"
#include "file_util.hpp"
#include "move_t.hpp"
#include "moves.hpp"
#include "tests.hpp"
#include <unistd.h>
#include <chrono>

std::chrono::_V2::system_clock::time_point t0 = std::chrono::high_resolution_clock::now();
std::chrono::_V2::system_clock::time_point t = std::chrono::high_resolution_clock::now();
std::chrono::_V2::system_clock::rep duration; // std::chrono::duration_cast<std::chrono::milliseconds>(t - t0).count()

bitboard_move_t unique_best_move;

int main(int argc, char const* argv[]) // ./BlueHerring -H history.csv -m move.csv {std::locale::global(std::locale("en_US.UTF-8")); // To enable printing of unicode characters}
{
    if (argc != 5) {
        printf("Wrong input size!, %i", argc);
        return -1;
    }
    string input_file_name  = argv[2];
    string output_file_name = argv[4];

    // tests::run_rules_test_suite();
    // tests::run_perft_suite();
    // tests::run_speed_test_suite();
    // tests::run_eval_test_suite();

    // Pause execution for 2 seconds to better follow a chess game
    // sleep(2);

    bitboard_t bitboard;
    bitboard.initialize_starting_board();
    vector<string> string_moves   = read_moves_from_input_file(&input_file_name);

    string book_move_str = get_move_from_book(string_moves);
    if (book_move_str != "") {
        write_move_to_output_file(&output_file_name, &book_move_str);
        return 0;
    }

    vector<bitboard_move_t> moves = translate_to_bitboard_moves(string_moves);
    for (const bitboard_move_t& move : moves) {
        moves::make_move(bitboard, move);
    }
    Color color_to_move = (moves.size() % 2 == 0) ? Color::WHITE : Color::BLACK;

    // Timing fail-safe move
    bitboard_move_t best_move_return = engine::get_best_move(bitboard, 2, color_to_move).first;

    for (int depth = 5; depth < 100; depth++) {
        // Testing the time
        t = std::chrono::high_resolution_clock::now();
        duration = std::chrono::duration_cast<std::chrono::milliseconds>(t - t0).count();
        if (duration > 9500) {
            break;
        }
        best_move_return = engine::get_best_move(bitboard, depth, color_to_move).first;        
    }   

    string best_move_str      = encode_move(bitboard_move_to_coordinate_move(unique_best_move));
    write_move_to_output_file(&output_file_name, &best_move_str);
    return 0;
}

