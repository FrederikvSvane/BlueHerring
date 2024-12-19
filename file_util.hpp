#ifndef file_util_hpp
#define file_util_hpp

#include <vector>
#include <fstream>

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
    output_file.open(*file_name, fstream::out);
    output_file << *move;
    output_file.close();
}

void append_move_to_input_file(string* file_name, string* move) {
    ofstream input_file;
    input_file.open(*file_name, fstream::out);
    input_file << *move << endl;
    input_file.close();
}

#endif