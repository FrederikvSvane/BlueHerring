#ifndef file_util_hpp
#define file_util_hpp

#include <vector>
#include <fstream>
#include <string>
#include "book.hpp"

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

string get_move_from_book(vector<string> string_moves) {
    string history_str = "";
    for (const auto& move : string_moves) {
        history_str = history_str + move + " ";
    }

    vector<string> out;
    for (const auto& line : book) {
        if (line[0] == '#') continue;

        if (history_str.size() <= line.size() + 5) {
            if (history_str == line.substr(0, history_str.size())) {
                return line.substr(history_str.size(), 4);
            }
        }
    }

    return "";
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


