#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <iostream>
#include <string>
#include <algorithm>
#include <vector>
#include <any>
#include <sstream>
#include <cstring>
#include <fstream>
#include <limits>
#include <cstdlib>
#include <windows.h>
#include <unordered_map>
using namespace std;

extern "C" {
    char* eval_expr(const char* expr);
    void free_string(char* ptr);
    unsigned char is_expr(const char* s);
}
void setColor(int color) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}
string trim(const string& str) {
  size_t first = str.find_first_not_of(" \t\r\n");
  if (first == string::npos) return "";
  size_t last = str.find_last_not_of(" \t\r\n");
  return str.substr(first, last - first + 1);
}

string os() {
    #if defined(_WIN32)
        return "Windows";
    #elif defined(__APPLE__) && defined(__MACH__)
        return "macOS";
    #elif defined(__linux__)
        return "Linux";
    #elif defined(__unix__)
        return "Unix";
    #else
        return "Unknown";
    #endif
}
string interpolate(const string& input, const vector<string>& varNames, const vector<any>& varVals) {
    string result;
    for (size_t i = 0; i < input.length(); ++i) {
        if (input[i] == '&') {
            size_t j = i + 1;
            string varName;
            while (j < input.length() && (isalnum(input[j]) || input[j] == '_')) {
                varName += input[j++];
            }
            bool found = false;
            for (int k = 0; k < varNames.size(); ++k) {
                if (varNames[k] == varName) {
                    if (varVals[k].type() == typeid(string)) {
                        result += any_cast<string>(varVals[k]);
                    } else if (varVals[k].type() == typeid(int)) {
                        result += to_string(any_cast<int>(varVals[k]));
                    } else if (varVals[k].type() == typeid(float)) {
                        result += to_string(any_cast<float>(varVals[k]));
                    }
                    found = true;
                    break;
                }
            }
            if (!found) {
                result += "&" + varName;
            }
            i = j - 1;
        } else {
            result += input[i];
        }
    }
    return result;
}

vector<string> split(const string& str, char delimiter) {
    vector<string> tokens;
    stringstream ss(str);
    string item;
    while (getline(ss, item, delimiter)) {
        tokens.push_back(item);
    }
    return tokens;
}

bool isInt(const string& str) {
    try {
        stoi(str);
        return true;
    } catch (...) {
        return false;
    }
}

bool isFloat(const string& str) {
    try {
        stof(str);
        return true;
    } catch (...) {
        return false;
    }
}

string join(const vector<string>& vec, const string& delim = "") {
    ostringstream oss;
    for (size_t i = 0; i < vec.size(); ++i) {
        oss << vec[i];
        if (i != vec.size() - 1)
            oss << delim;
    }
    return oss.str();
}

void printAny(const any& val) {
    if (val.type() == typeid(int)) {
        cout << any_cast<int>(val);
    } else if (val.type() == typeid(float)) {
        cout << any_cast<float>(val);
    } else if (val.type() == typeid(string)) {
        cout << any_cast<string>(val);
    } else {
        setColor(12);
        cout << "ERROR: Unknown value type\n";
        setColor(7);
    }
}