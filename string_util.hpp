#ifndef string_util_hpp
#define string_util_hpp
#include <sstream>
#include <string>
#include <vector>

using namespace std;

vector<string> split(const string& str, char delimiter) { // from stackoverflow :)
    vector<string> result;
    stringstream ss(str);
    string token;
    while (getline(ss, token, delimiter)) {
        result.push_back(token);
    }
    return result;
}

#endif