#include "util.hpp"
using namespace std;

int main() {
  vector<string> lines;
  vector<any> varVals;
  vector<string> varNames;
  vector<string> vecNames;
  vector<vector<any>> vecs;
  unordered_map<string, vector<string>> functions;
  unordered_map<string, vector<string>> functionArgs;
  string line;

  while (getline(cin, line)) {
    lines.push_back(line);
  }
  cin.clear();
  cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
  vector<string> newLines;
  for (size_t i = 0; i < lines.size(); ++i) {
    if (lines[i].starts_with("fnc ")) {
      string name = trim(lines[i].substr(4));
      functions[name] = {};
      functionArgs[name] = {};

      size_t j = i + 1;

      if (j < lines.size() && lines[j].starts_with("args ")) {
        vector<string> args = split(trim(lines[j].substr(5)), ' ');
        functionArgs[name] = args;
        ++j;
      }

      while (j < lines.size() && trim(lines[j]) != "end") {
        functions[name].push_back(lines[j]);
        ++j;
      }

      i = j;
    } else {
      newLines.push_back(lines[i]);
    }
  }

  lines = newLines;
  int i = 0;
  while (i < lines.size()) {
    string ln = lines[i];

    if (ln.starts_with("print ")) {
      string str = ln.substr(6);
      if (str.starts_with('"') && str.ends_with('"')) {
        str.erase(remove(str.begin(), str.end(), '"'), str.end());
        str = interpolate(str, varNames, varVals);
        cout << str;
      } else if (isInt(str) || isFloat(str)) {
        cout << str;
      } else if (str.starts_with("vecitem ")) {
        vector<string> args = split(str.substr(8), ' ');
        string name = args[0];
        int index = stoi(args[1]);
        for (int j = 0; j < vecNames.size(); j++) {
          if (vecNames[j] == name) {
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
          for (int j = 0; j < vecNames.size(); j++) {
            if (vecNames[j] == str) {
              cout << "( ";
              for (any s : vecs[j]) {
                cout << any_cast<string>(s) << " ";
                found = true;
              }
              cout << " )";
            }
          }
          if (!found) {
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
      } else if (str.starts_with("vecitem ")) {
        vector<string> args = split(str.substr(8), ' ');
        string name = args[0];
        int index = stoi(args[1]);
        for (int j = 0; j < vecNames.size(); j++) {
          if (vecNames[j] == name) {
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
          for (int j = 0; j < vecNames.size(); j++) {
            if (vecNames[j] == str) {
              cout << "( ";
              for (any s : vecs[j]) {
                printAny(s);
                cout << ", ";
              }
              found = true;
              cout << ")" << "\n";
            }
          }
          if (!found) {
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
        cout << "ERROR: Expected assignment. Perhaps you meant 'vec' instead "
                "of 'let'?\n";
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
        if (val == "os") {
          varVals.push_back(os());
        } else if (val.starts_with('"') && val.ends_with('"')) {
          if (val.length() >= 2) {
            val = val.substr(1, val.length() - 2);
          }
          varVals.push_back(val);
        } else if (val.starts_with("readfile ")) {
          string filename = val.substr(9);
          filename = interpolate(filename, varNames, varVals);
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
        } else if (is_expr(val.c_str()) != 0) {
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
                      cout << "ERROR: Failed to evaluate expression for "
                              "vecitem.\n";
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
              if (val == "os") {
                varVals[j] = os();
              } else if (val.starts_with('"') && val.ends_with('"')) {
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
      bool found = false;
      for (int j = 0; j < varNames.size(); j++) {
        if (name == varNames[j]) {
          found = true;
          std::string val;

#ifdef _WIN32
          FILE* console = fopen("CON", "r");
          if (console) {
            char buffer[1024];
            if (fgets(buffer, sizeof(buffer), console)) {
              val = std::string(buffer);
              val.erase(val.find_last_not_of("\r\n") + 1);
            }
            fclose(console);
          }
#else
          std::ifstream tty("/dev/tty");
          if (tty.is_open()) {
            std::getline(tty, val);
          }
#endif

          if (val.empty()) {
            cerr << "Input for variable '" << name << "' was empty.\n";
          } else if (is_expr(val.c_str()) != 0) {
            varVals[j] = eval_expr(val.c_str());
          } else {
            varVals[j] = val;
          }
          break;
        }
      }
      if (!found) {
        setColor(12);
        cout << "ERROR: Variable not declared: " << name << "\n";
        setColor(7);
      }
    } else if (ln.starts_with("vec ")) {
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
    } else if (ln.starts_with("push ")) {
      vector<string> prs = split(ln.substr(5), ' ');
      string name = prs[0];
      for (int j = 0; j < vecNames.size(); j++) {
        if (vecNames[j] == name) {
          prs.erase(prs.begin());
          any val = join(prs);
          vecs[j].push_back(val);
        }
      }
    } else if (ln.starts_with("for ")) {
      vector<string> cond =
          split(interpolate(ln.substr(4), varNames, varVals), ' ');
      if (cond.size() < 3) {
        setColor(12);
        cout << "ERROR: Invalid condition.\n";
        setColor(7);
        continue;
      }

      if (cond[1] != "in") {
        setColor(12);
        cout << "ERROR: Expected keyword 'in'\n";
        setColor(7);
        continue;
      }

      string varName = cond[0];
      string rangeStr;
      if (cond.size() == 3) {
        rangeStr = cond[2];
      }
      if (cond.size() == 4 && cond[2] == "vecsize") {
        string name = trim(cond[3]);
        for (int j = 0; j < vecNames.size(); j++) {
          if (vecNames[j] == name) {
            rangeStr = to_string(vecs[j].size());
            break;
          }
        }
      }
      int rangeLimit = 0;
      try {
        rangeLimit = stoi(rangeStr);
      } catch (...) {
        setColor(12);
        cout << "ERROR: Loop range must be an integer.\n";
        setColor(7);
        continue;
      }

      vector<string> loopBlock;
      int loopStart = i + 1;
      i++;
      while (i < lines.size() && lines[i] != "end") {
        loopBlock.push_back(lines[i]);
        i++;
      }
      int loopEnd = i;
      int varIndex = -1;
      for (int j = 0; j < varNames.size(); j++) {
        if (varNames[j] == varName) {
          varIndex = j;
          break;
        }
      }
      if (varIndex == -1) {
        varNames.push_back(varName);
        varVals.push_back("0");
        varIndex = varNames.size() - 1;
      }
      vector<string> repeated;
      for (int j = 0; j < rangeLimit; j++) {
        varVals[varIndex] = to_string(j);
        for (string line : loopBlock) {
          line = interpolate(line, varNames, varVals);
          repeated.push_back(line);
        }
      }

      lines.erase(lines.begin() + loopStart, lines.begin() + loopEnd);
      lines.insert(lines.begin() + loopStart, repeated.begin(), repeated.end());
      i = loopStart - 1;
    } else if (ln.starts_with("// ")) {
      i++;
    } else if (ln.starts_with("/* ")) {
      while (i < lines.size()) {
        ln = lines[i];
        if (ln.find("*/") != std::string::npos) {
          i++;
          break;
        }
        i++;
      }
    } else if (ln.starts_with("system ")) {
      string cmd = ln.substr(7);
      cmd = interpolate(cmd, varNames, varVals);
      system(cmd.c_str());
    } else if (ln.starts_with("writefile ")) {
      vector<string> prs =
          split(interpolate(ln.substr(10), varNames, varVals), ' ');
      if (prs.size() < 3) {
        setColor(12);
        cout << "ERROR: Invalid writefile syntax.\n";
        setColor(7);
        continue;
      }

      string mode = prs[0];
      string name = prs[1];
      prs.erase(prs.begin(), prs.begin() + 2);
      string content = join(prs, " ");

      if (content.starts_with('"') && content.ends_with('"')) {
        content.erase(remove(content.begin(), content.end(), '"'),
                      content.end());
      }

      ofstream outfile;
      if (mode == "<") {
        outfile.open(name);
      } else if (mode == ">") {
        outfile.open(name, std::ios::app);
      } else {
        setColor(12);
        cout << "ERROR: Invalid mode '" << mode
             << "'. Use < to overwrite, > to append.\n";
        setColor(7);
        continue;
      }

      if (!outfile) {
        setColor(12);
        cout << "ERROR: Cannot open file '" << name << "'.\n";
        setColor(7);
        continue;
      }

      outfile << content;
      outfile.close();
    } else if (ln.starts_with("fnc ")) {
      while (i < lines.size() && lines[i] != "end") i++;
      continue;
    } else if (ln.starts_with("call ")) {
      vector<string> parts = split(trim(ln.substr(5)), ' ');
      if (parts.empty()) continue;

      string name = parts[0];
      vector<string> argVals(parts.begin() + 1, parts.end());

      if (functions.find(name) == functions.end()) {
        setColor(12);
        cout << "ERROR: Function not found: " << name << "\n";
        setColor(7);
        continue;
      }
      vector<string> body = functions[name];
      size_t start = 0;
      vector<string> args = functionArgs[name];
      if (argVals.size() != args.size()) {
        setColor(12);
        cout << "ERROR: Function '" << name << "' expects " << args.size()
             << " arguments, got " << argVals.size() << ".\n";
        setColor(7);
        continue;
      }
      for (size_t j = 0; j < args.size(); ++j) {
        varNames.push_back(args[j]);
        if (isInt(argVals[j])) {
          varVals.push_back(stoi(argVals[j]));
        } else if (isFloat(argVals[j])) {
          varVals.push_back(stof(argVals[j]));
        } else {
          varVals.push_back(argVals[j]);
        }
      }
      lines.erase(lines.begin() + i);
      lines.insert(lines.begin() + i, body.begin() + start, body.end());
      i--;
    } else {
      if (ln != "end") {
        vector<string> prs = split(ln, ' ');
        setColor(12);
        cout << "ERROR: " << prs[0] << " is not defined.\n";
        setColor(7);
        i++;
      }
    }
    i++;
  }
}
