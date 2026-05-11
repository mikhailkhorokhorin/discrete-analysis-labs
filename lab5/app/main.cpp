#include "suffix_tree.hpp"

#include <iostream>
#include <set>
#include <string>
#include <vector>

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    std::string s1;
    std::string s2;

    if (!std::getline(std::cin, s1)) {
        return 0;
    }
    if (!std::getline(std::cin, s2)) {
        return 0;
    }

    if (!s1.empty() && s1.back() == '\r') {
        s1.pop_back();
    }
    if (!s2.empty() && s2.back() == '\r') {
        s2.pop_back();
    }

    if (s1.empty() || s2.empty()) {
        std::cout << 0 << "\n";
        return 0;
    }

    int sep = static_cast<int>(s1.size());
    std::vector<int> combined = BuildCombinedText(s1, s2);

    TSuffixTree tree(combined);

    int lcsLen = 0;
    std::set<std::string> results;
    tree.FindLcs(sep, lcsLen, results);

    std::cout << lcsLen << "\n";
    for (const std::string &s : results) {
        std::cout << s << "\n";
    }

    return 0;
}
