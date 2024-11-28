#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

vector<string> read_moves(string input_file) {
    fstream file_stream;
    file_stream.open(input_file);

    string line;
    vector<string> out;

    while (getline(file_stream, line)) {
        out.push_back(line);
    }

    return out;
}


int main(int argc, char const *argv[])
{
    if (argc != 5) {
        printf("Wrong input size!, %i", argc);
        return -1;
    }

    string input_file = argv[2];
    string output_file = argv[4];

    vector<string> moves = read_moves(input_file);

    for (auto& move : moves) {
        cout << move << endl;
    }

    return 0;
}
