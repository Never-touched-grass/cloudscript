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
#include <windows.h>
using namespace std;

extern "C" {
    char* eval_expr(const char* expr);
    void free_string(char* ptr);
    unsigned char is_expr(const char* s);
}
void setColor(int color) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
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

int main() {
    vector<string> lines;
    vector<any> varVals;
    vector<string> varNames;
    vector<string> vecNames;
    vector<vector<any>> vecs;
    string line;

    while (getline(cin, line)) {
        lines.push_back(line);
    }

    for (int i = 0; i < lines.size(); i++) {
        string ln = lines[i];

        if (ln.starts_with("print ")) {
            string str = ln.substr(6);
            if (str.starts_with('"') && str.ends_with('"')) {
                str.erase(remove(str.begin(), str.end(), '"'), str.end());
                str = interpolate(str, varNames, varVals);
                cout << str;
            } else if (isInt(str) || isFloat(str)) {
                cout << str;
            } else if (str.starts_with("vecitem ")){
                vector<string> args = split(str.substr(8), ' ');
                string name = args[0];
                int index = stoi(args[1]);
                for(int j = 0; j < vecNames.size(); j++){
                    if(vecNames[j] == name){
                        printAny(vecs[j][index]);
                    }
                }
            } else {
                bool found = false;
                for (int j = 0; j < varNames.size(); j++) {
                    if (varNames[j] == str) {
                        printAny(varVals[j]);
                        found = true;
                        break;
                    }
                }
                if (!found) {
                    for(int j = 0; j < vecNames.size(); j++){
                        if(vecNames[j] == str){
                            cout << "( ";
                            for(any s : vecs[j]){
                                cout << any_cast<string>(s) << " ";
                                found = true;
                            }
                            cout << " )";
                        }
                    }
                    if(!found){
                        setColor(12);
                        cout << "ERROR: Variable not found: " << str << "\n";
                        setColor(7);
                    }
                }
            }
        }

        else if (ln.starts_with("println ")) {
            string str = ln.substr(8);
            if (str.starts_with('"') && str.ends_with('"')) {
                str.erase(remove(str.begin(), str.end(), '"'), str.end());
                str = interpolate(str, varNames, varVals);
                cout << str << "\n";
            } else if (isInt(str) || isFloat(str)) {
                cout << str << "\n";
            } else if (str.starts_with("vecitem ")){
                vector<string> args = split(str.substr(8), ' ');
                string name = args[0];
                int index = stoi(args[1]);
                for(int j = 0; j < vecNames.size(); j++){
                    if(vecNames[j] == name){
                        printAny(vecs[j][index]);
                        cout << "\n";
                    }
                }
            } else {
                bool found = false;
                for (int j = 0; j < varNames.size(); j++) {
                    if (varNames[j] == str) {
                        printAny(varVals[j]);
                        cout << "\n";
                        found = true;
                        break;
                    }
                }
                if (!found) {
                    for(int j = 0; j < vecNames.size(); j++){
                        if(vecNames[j] == str){
                            cout << "( ";
                            for(any s : vecs[j]){
                                printAny(s);
                                cout << ", ";
                            }
                            found = true;
                            cout << ")" << "\n";
                        }
                    }
                    if(!found){
                        setColor(12);
                        cout << "ERROR: Variable not found: " << str << "\n";
                        setColor(7);
                    }
                }
            }
        } else if (ln.starts_with("let ")) {
    string str = ln.substr(4);
    vector<string> prs = split(str, ' ');
    if (prs.size() < 3 || prs[1] != "=") {
        setColor(12);
        cout << "ERROR: Expected assignment. Perhaps you meant 'vec' instead of 'let'?\n";
        setColor(7);
    } else {
        string varName = prs[0];
        auto itVec = find(vecNames.begin(), vecNames.end(), varName);
        if (itVec != vecNames.end()) {
            int idx = distance(vecNames.begin(), itVec);
            vecNames.erase(itVec);
            vecs.erase(vecs.begin() + idx);
        }
        auto itVar = find(varNames.begin(), varNames.end(), varName);
        if (itVar != varNames.end()) {
            int idx = distance(varNames.begin(), itVar);
            varNames.erase(itVar);
            varVals.erase(varVals.begin() + idx);
        }
        varNames.push_back(varName);
        vector<string> valueTokens(prs.begin() + 2, prs.end());
        string val = join(valueTokens, " ");
        if (val.starts_with('"') && val.ends_with('"')) {
            if (val.length() >= 2) {
                val = val.substr(1, val.length() - 2);
            }
            varVals.push_back(val);
        } else if (val.starts_with("readfile ")) {
            string filename = val.substr(9);
            ifstream file(filename);
            if (!file) {
                setColor(12);
                cout << "ERROR: File not found: " << filename << "\n";
                setColor(7);   
            } else {
                stringstream buffer;
                buffer << file.rdbuf();
                varVals.push_back(buffer.str());
            }
        } else if (val.starts_with("vecitem ")) {
            vector<string> vecitemTokens = split(val.substr(8), ' ');
            if (vecitemTokens.size() >= 2) {
                string vecName = vecitemTokens[0];
                int idx = stoi(vecitemTokens[1]);
                bool foundVec = false;
                for (int j = 0; j < vecNames.size(); j++) {
                    if (vecNames[j] == vecName) {
                        foundVec = true;
                        if (idx >= 0 && idx < (int)vecs[j].size()) {
                            varVals.push_back(vecs[j][idx]);
                        } else {
                            setColor(12);
                            cout << "ERROR: vecitem index out of range\n";
                            setColor(7);
                            varVals.push_back(string(""));
                        }
                        break;
                    }
                }
                if (!foundVec) {
                    setColor(12);
                    cout << "ERROR: Vector not found: " << vecName << "\n";
                    setColor(7);
                    varVals.push_back(string(""));
                }
            } else {
                setColor(12);
                cout << "ERROR: vecitem requires vector name and index\n";
                setColor(7);
                varVals.push_back(string(""));
            }
        }
        else if (is_expr(val.c_str()) != 0) {
            string expr = interpolate(val, varNames, varVals);
            char* result = eval_expr(expr.c_str());
            if (result) {
                string res_str = result;
                free_string(result);
                if (isInt(res_str)) {
                    varVals.push_back(stoi(res_str));
                } else if (isFloat(res_str)) {
                    varVals.push_back(stof(res_str));
                } else {
                    varVals.push_back(res_str);
                }
            } else {
                setColor(12);
                cout << "ERROR: Failed to evaluate expression\n";
                setColor(7);
                varVals.push_back(string(""));
            }
        } else if (isInt(val)) {
            varVals.push_back(stoi(val));
        } else if (isFloat(val)) {
            varVals.push_back(stof(val));
        } else {
            setColor(12);
            cout << "ERROR: Unsupported value type in let statement.\n";
            setColor(7);
        }
    }
} else if (ln.starts_with("re ")) {
    string str = ln.substr(3);
    vector<string> prs = split(str, ' ');
    if (prs.size() < 3 || prs[1] != "=") {
        setColor(12);
        cout << "ERROR: Expected assignment.\n";
        setColor(7);
    } else {
        string varName = prs[0];
        vector<string> valueTokens(prs.begin() + 2, prs.end());
        string val = join(valueTokens, " ");
        if (varName.starts_with("vecitem ")) {
            vector<string> vecitemTokens = split(varName.substr(8), ' ');
            if (vecitemTokens.size() >= 2) {
                string vecName = vecitemTokens[0];
                int idx = stoi(vecitemTokens[1]);
                bool foundVec = false;
                for (int j = 0; j < vecNames.size(); j++) {
                    if (vecNames[j] == vecName) {
                        foundVec = true;
                        if (idx >= 0 && idx < (int)vecs[j].size()) {
                            if (val.starts_with('"') && val.ends_with('"')) {
                                val = val.substr(1, val.length() - 2);
                                vecs[j][idx] = val;
                            } else if (is_expr(val.c_str()) != 0) {
                                string expr = interpolate(val, varNames, varVals);
                                char* result = eval_expr(expr.c_str());
                                if (result) {
                                    string res_str = result;
                                    free_string(result);
                                    vecs[j][idx] = res_str;
                                } else {
                                    setColor(12);
                                    cout << "ERROR: Failed to evaluate expression for vecitem.\n";
                                    setColor(7);
                                }
                            } else if (isInt(val)) {
                                vecs[j][idx] = val;
                            } else if (isFloat(val)) {
                                vecs[j][idx] = val;
                            } else {
                                setColor(12);
                                cout << "ERROR: Unsupported value type for vecitem.\n";
                                setColor(7);
                            }
                        } else {
                            setColor(12);
                            cout << "ERROR: vecitem index out of range.\n";
                            setColor(7);
                        }
                        break;
                    }
                }
                if (!foundVec) {
                    setColor(12);
                    cout << "ERROR: Vector not found: " << vecName << "\n";
                    setColor(7);
                }
            } else {
                setColor(12);
                cout << "ERROR: vecitem requires vector name and index.\n";
                setColor(7);
            }
        } else {
            bool found = false;
            for (int j = 0; j < varNames.size(); j++) {
                if (varName == varNames[j]) {
                    if (val.starts_with('"') && val.ends_with('"')) {
                        val = val.substr(1, val.length() - 2);
                        varVals[j] = val;
                    } else if (is_expr(val.c_str()) != 0) {
                        string expr = interpolate(val, varNames, varVals);
                        char* result = eval_expr(expr.c_str());
                        if (result) {
                            string res_str = result;
                            free_string(result);
                            if (isInt(res_str)) {
                                varVals[j] = stoi(res_str);
                            } else if (isFloat(res_str)) {
                                varVals[j] = stof(res_str);
                            } else {
                                varVals[j] = res_str;
                            }
                        } else {
                            setColor(12);
                            cout << "ERROR: Failed to evaluate expression.\n";
                            setColor(7);
                            varVals[j] = string("");
                        }
                    } else if (isInt(val)) {
                        varVals[j] = stoi(val);
                    } else if (isFloat(val)) {
                        varVals[j] = stof(val);
                    } else {
                        setColor(12);
                        cout << "ERROR: Unsupported reassignment value type.\n";
                        setColor(7);
                    }
                    found = true;
                    break;
                }
            }
            if (!found) {
                setColor(12);
                cout << "ERROR: Variable not declared: " << varName << "\n";
                setColor(7);
            }
        }
    }
} else if (ln.starts_with("if ")) {
            string cond = ln.substr(3);
            vector<string> prs = split(cond, ' ');
            for (int x = 0; x < (int)prs.size(); x++) {
                string pr = prs[x];
                for (int j = 0; j < (int)varNames.size(); j++) {
                    if (pr == varNames[j]) {
                        if (varVals[j].type() == typeid(int)) {
                            prs[x] = to_string(any_cast<int>(varVals[j]));
                        } else if (varVals[j].type() == typeid(float)) {
                            prs[x] = to_string(any_cast<float>(varVals[j]));
                        } else if (varVals[j].type() == typeid(string)) {
                            prs[x] = any_cast<string>(varVals[j]);
                        } else {
                            prs[x] = "";
                        }
                    break;
                }
            }
        }
        cond = join(prs, " ");
        char* result = eval_expr(cond.c_str());
        bool skip = true;

        if (result) {
            string val = result;
            free_string(result);
            if (val == "true" || val == "1") {
                skip = false; 
            }
        }

        if (skip) {
            i++;
            while (i < (int)lines.size() && lines[i] != "endif") {
                i++;
            }
        }
    } else if (ln.starts_with("read ")) {
    string name = ln.substr(5);
    for (int j = 0; j < varNames.size(); j++) {
        if (name == varNames[j]) {
            string val;
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            getline(cin, val);
            if (is_expr(val.c_str()) != 0) {
                string result = eval_expr(val.c_str());
                varVals[j] = result;
            } else {
                varVals[j] = val;
            }
        }
    }
}
 else if (ln.starts_with("vec ")) {
    string str = ln.substr(4);
    vector<string> prs = split(str, ' ');
    if (prs.empty()) {
        setColor(12);
        cout << "ERROR: Expected vector name and values.\n";
        setColor(7);
        continue;
    }
    string vecName = prs[0];
    auto itVar = find(varNames.begin(), varNames.end(), vecName);
    if (itVar != varNames.end()) {
        int idx = distance(varNames.begin(), itVar);
        varNames.erase(itVar);
        varVals.erase(varVals.begin() + idx);
    }
    auto itVec = find(vecNames.begin(), vecNames.end(), vecName);
    if (itVec != vecNames.end()) {
        int idx = distance(vecNames.begin(), itVec);
        vecNames.erase(itVec);
        vecs.erase(vecs.begin() + idx);
    }

    vecNames.push_back(vecName);
    prs.erase(prs.begin());
    vector<any> anyVec;
    for (const auto& s : prs) {
        if (isInt(s)) {
            anyVec.push_back(stoi(s));
        } else if (isFloat(s)) {
            anyVec.push_back(stof(s));
        } else {
            anyVec.push_back(s);
        }
    }
    vecs.push_back(anyVec);
    } else if (ln.starts_with("push ")){
        vector<string> prs = split(ln.substr(5), ' ');
        string name = prs[0];
        for(int j = 0; j < vecNames.size(); j++){
            if(vecNames[j] == name){
                prs.erase(prs.begin());
                any val = join(prs);
                vecs[j].push_back(val);
            }
        }
    }
}
}