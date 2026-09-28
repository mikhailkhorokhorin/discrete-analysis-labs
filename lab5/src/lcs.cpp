#include "lcs.hpp"

#include <istream>
#include <ostream>
#include <string>

#include "suffix_tree.hpp"

namespace {

std::string readLine(std::istream& input) {
    std::string line;
    if (!std::getline(input, line)) {
        return "";
    }
    if (!line.empty() && line.back() == '\r') {
        line.pop_back();
    }
    return line;
}

}

LcsResult longestCommonSubstrings(const std::string& first, const std::string& second) {
    if (first.empty() || second.empty()) {
        return LcsResult{};
    }
    const SuffixTree tree(buildCombinedText(first, second));
    return tree.findLcs(first.size());
}

void runLcs(std::istream& input, std::ostream& output) {
    const std::string first = readLine(input);
    const std::string second = readLine(input);
    const LcsResult result = longestCommonSubstrings(first, second);
    output << result.length << '\n';
    for (const std::string& substring : result.substrings) {
        output << substring << '\n';
    }
}
