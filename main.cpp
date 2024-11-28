#include <fstream>
#include <iostream>
#include <vector>

using namespace std;

vector<string> read_moves_from_input_file(string* file_name) {
    fstream file_stream;
    file_stream.open(*file_name);

    string line;
    vector<string> out;

    while (getline(file_stream, line)) {
        out.push_back(line);
    }

    return out;
}

void write_move_to_output_file(string* file_name, string* move) {
    ofstream output_file;
    output_file.open(*file_name, ios::app);
    output_file << *move;
    output_file.close();
}

int main(int argc, char const* argv[]) // ./BlueHerring -H input.txt -m output_example.txt
{
    if (argc != 5) {
        printf("Wrong input size!, %i", argc);
        return -1;
    }

    string input_moves_file = argv[2];  // could be deleted and passed directly for memory efficiency
    string output_moves_file = argv[4]; // likewise

    vector<string> moves = read_moves_from_input_file(&input_moves_file);

    /*
    - for printing the input file line by line (testing)


    for (auto& move : moves) {
        cout << move << endl;
    }
    */

    string best_move; // the magnum opus
    best_move = "e3e4";
    write_move_to_output_file(&output_moves_file, &best_move);

    return 0;
}
