#include <cstddef>
#include <iosfwd>
#include <iostream>
#include <istream>
#include <map>
#include <ostream>
#include <ranges>
#include <set>
#include <string>
#include <utility>
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

LcsResult longestCommonSubstrings(const std::string& first, const std::string& second);
void runLcs(std::istream& input, std::ostream& output);

namespace {

constexpr int ROOT = 0;
constexpr int NONE = -1;
constexpr int LEAF_END = -1;

constexpr unsigned COVERS_FIRST = 1U;
constexpr unsigned COVERS_SECOND = 2U;
constexpr unsigned COVERS_BOTH = COVERS_FIRST | COVERS_SECOND;

}

std::vector<int> buildCombinedText(const std::string& first, const std::string& second) {
    std::vector<int> result;
    result.reserve(first.size() + second.size() + 2);
    for (const unsigned char symbol : first) {
        result.push_back(symbol);
    }
    result.push_back(FIRST_SEPARATOR);
    for (const unsigned char symbol : second) {
        result.push_back(symbol);
    }
    result.push_back(SECOND_SEPARATOR);
    return result;
}

SuffixTree::SuffixTree(std::vector<int> text) : text_(std::move(text)) {
    nodes_.reserve(text_.size() * 2 + 1);
    newNode(NONE, NONE);
    for (int position = 0; position < static_cast<int>(text_.size()); ++position) {
        extend(position);
    }
}

int SuffixTree::newNode(int start, int end) {
    nodes_.push_back(Node{.children = {}, .suffixLink = 0, .start = start, .end = end});
    return static_cast<int>(nodes_.size()) - 1;
}

int SuffixTree::edgeLength(int node) const {
    const int edgeEnd = nodes_[node].end == LEAF_END ? leafEnd_ : nodes_[node].end;
    return edgeEnd - nodes_[node].start + 1;
}

int SuffixTree::child(int node, int symbol) const {
    const auto iterator = nodes_[node].children.find(symbol);
    return iterator == nodes_[node].children.end() ? NONE : iterator->second;
}

void SuffixTree::extend(int position) {
    leafEnd_ = position;
    ++remaining_;
    int lastInternal = NONE;

    while (remaining_ > 0) {
        if (activeLength_ == 0) {
            activeEdge_ = position;
        }

        const int edgeSymbol = text_[activeEdge_];
        const int next = child(activeNode_, edgeSymbol);

        if (next == NONE) {
            nodes_[activeNode_].children[edgeSymbol] = newNode(position, LEAF_END);
            if (lastInternal != NONE) {
                nodes_[lastInternal].suffixLink = activeNode_;
                lastInternal = NONE;
            }
        } else {
            const int nextLength = edgeLength(next);
            if (activeLength_ >= nextLength) {
                activeEdge_ += nextLength;
                activeLength_ -= nextLength;
                activeNode_ = next;
                continue;
            }

            const int splitSymbol = text_[nodes_[next].start + activeLength_];
            const int currentSymbol = text_[position];
            if (splitSymbol == currentSymbol) {
                ++activeLength_;
                if (lastInternal != NONE) {
                    nodes_[lastInternal].suffixLink = activeNode_;
                }
                break;
            }

            const int split = newNode(nodes_[next].start, nodes_[next].start + activeLength_ - 1);
            nodes_[activeNode_].children[edgeSymbol] = split;
            const int leaf = newNode(position, LEAF_END);
            nodes_[split].children[currentSymbol] = leaf;
            nodes_[next].start += activeLength_;
            nodes_[split].children[splitSymbol] = next;

            if (lastInternal != NONE) {
                nodes_[lastInternal].suffixLink = split;
            }
            lastInternal = split;
        }

        --remaining_;

        if (activeNode_ == ROOT && activeLength_ > 0) {
            --activeLength_;
            activeEdge_ = position - remaining_ + 1;
        } else if (activeNode_ != ROOT) {
            activeNode_ = nodes_[activeNode_].suffixLink;
        }
    }
}

std::string SuffixTree::makeString(int start, int length) const {
    std::string result;
    result.reserve(static_cast<std::size_t>(length));
    for (int i = 0; i < length; ++i) {
        result.push_back(static_cast<char>(text_[start + i]));
    }
    return result;
}

LcsResult SuffixTree::findLcs(std::size_t separator) const {
    const int separatorIndex = static_cast<int>(separator);
    const int lastIndex = static_cast<int>(text_.size()) - 1;
    const std::size_t count = nodes_.size();

    std::vector<int> order;
    order.reserve(count);
    std::vector<int> parent(count, NONE);
    std::vector<int> depth(count, 0);
    std::vector<int> stack = {ROOT};
    while (!stack.empty()) {
        const int node = stack.back();
        stack.pop_back();
        order.push_back(node);
        for (const auto& [symbol, next] : nodes_[node].children) {
            parent[next] = node;
            depth[next] = depth[node] + edgeLength(next);
            stack.push_back(next);
        }
    }

    std::vector<unsigned> coverage(count, 0U);
    std::vector<int> firstStart(count, NONE);
    int bestLength = 0;
    std::set<std::string> found;

    for (const int node : std::views::reverse(order)) {
        if (nodes_[node].children.empty()) {
            const int suffixStart = leafEnd_ + 1 - depth[node];
            if (suffixStart < separatorIndex) {
                coverage[node] = COVERS_FIRST;
                firstStart[node] = suffixStart;
            } else if (suffixStart > separatorIndex && suffixStart < lastIndex) {
                coverage[node] = COVERS_SECOND;
            }
        } else if (node != ROOT && coverage[node] == COVERS_BOTH && firstStart[node] != NONE &&
                   firstStart[node] + depth[node] <= separatorIndex && depth[node] >= bestLength) {
            if (depth[node] > bestLength) {
                bestLength = depth[node];
                found.clear();
            }
            found.insert(makeString(firstStart[node], depth[node]));
        }

        const int up = parent[node];
        if (up != NONE) {
            coverage[up] |= coverage[node];
            if (firstStart[up] == NONE) {
                firstStart[up] = firstStart[node];
            }
        }
    }

    return LcsResult{.length = static_cast<std::size_t>(bestLength),
                     .substrings = std::vector<std::string>(found.begin(), found.end())};
}

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

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    runLcs(std::cin, std::cout);
    return 0;
}
