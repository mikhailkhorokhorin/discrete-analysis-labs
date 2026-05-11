#include "patricia.hpp"

#include <iostream>
#include <sstream>
#include <string>

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    TPatriciaTrie dict;
    std::string line;

    while (std::getline(std::cin, line)) {
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }
        if (line.empty()) {
            continue;
        }

        std::istringstream ss(line);
        std::string cmd;
        ss >> cmd;

        if (cmd == "+") {
            std::string word;
            uint64_t val = 0;
            ss >> word >> val;
            std::cout << (dict.Insert(word, val) ? "OK" : "Exist") << "\n";

        } else if (cmd == "-") {
            std::string word;
            ss >> word;
            std::cout << (dict.Remove(word) ? "OK" : "NoSuchWord") << "\n";

        } else if (cmd == "!") {
            std::string subcmd, path;
            ss >> subcmd >> path;
            if (subcmd == "Save") {
                std::cout << (dict.Save(path) ? "OK" : "ERROR: cannot save") << "\n";
            } else if (subcmd == "Load") {
                std::cout << (dict.Load(path) ? "OK" : "ERROR: cannot load") << "\n";
            }

        } else {
            uint64_t val = 0;
            if (dict.Find(cmd, val)) {
                std::cout << "OK: " << val << "\n";
            } else {
                std::cout << "NoSuchWord\n";
            }
        }
    }

    return 0;
}
