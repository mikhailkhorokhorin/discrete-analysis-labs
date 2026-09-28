#pragma once

#include <cstddef>
#include <map>
#include <string>
#include <vector>

inline constexpr int FIRST_SEPARATOR = 256;
inline constexpr int SECOND_SEPARATOR = 257;

struct LcsResult {
    std::size_t length = 0;
    std::vector<std::string> substrings;

    bool operator==(const LcsResult& other) const = default;
};

std::vector<int> buildCombinedText(const std::string& first, const std::string& second);

class SuffixTree {
public:
    explicit SuffixTree(std::vector<int> text);

    [[nodiscard]] LcsResult findLcs(std::size_t separator) const;
    [[nodiscard]] std::size_t nodeCount() const { return nodes_.size(); }

private:
    struct Node {
        std::map<int, int> children;
        int suffixLink = 0;
        int start = 0;
        int end = 0;
    };

    int newNode(int start, int end);
    [[nodiscard]] int edgeLength(int node) const;
    [[nodiscard]] int child(int node, int symbol) const;
    void extend(int position);
    [[nodiscard]] std::string makeString(int start, int length) const;

    std::vector<int> text_;
    std::vector<Node> nodes_;
    int leafEnd_ = -1;
    int activeNode_ = 0;
    int activeEdge_ = 0;
    int activeLength_ = 0;
    int remaining_ = 0;
};
