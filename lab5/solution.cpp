#include <iostream>
#include <map>
#include <set>
#include <string>
#include <vector>

const int FIRST_SEPARATOR = 256;
const int SECOND_SEPARATOR = 257;
const int ROOT = 0;
const int NO_CHILD = -1;
const int NO_LINK = 0;
const int LEAF_END = -1;

const int COV_NONE = 0;
const int COV_FIRST = 1;
const int COV_SECOND = 2;
const int COV_BOTH = 3;

struct TNode {
    std::map<int, int> children;
    int suffixLink;
    int start;
    int end;

    TNode(int start, int end) : suffixLink(NO_LINK), start(start), end(end) {
    }
};

class TSuffixTree {
  public:
    explicit TSuffixTree(const std::vector<int> &inputText)
        : text(inputText), leafEnd(-1), activeNode(ROOT), activeEdge(0), activeLength(0),
          remaining(0) {
        nodes.reserve(text.size() * 2 + 1);
        NewNode(-1, -1);
        for (int i = 0; i < static_cast<int>(text.size()); ++i) {
            Extend(i);
        }
    }

    void FindLcs(int sep, int &outLen, std::set<std::string> &outResults) const {
        outLen = 0;
        outResults.clear();
        int firstStart = NO_CHILD;
        Dfs(ROOT, 0, sep, outLen, outResults, firstStart);
    }

  private:
    std::vector<int> text;
    std::vector<TNode> nodes;
    int leafEnd;
    int activeNode;
    int activeEdge;
    int activeLength;
    int remaining;

    int NewNode(int start, int end) {
        nodes.push_back(TNode(start, end));
        return static_cast<int>(nodes.size()) - 1;
    }

    int EdgeLen(int idx) const {
        int edgeEnd = nodes[idx].end == LEAF_END ? leafEnd : nodes[idx].end;
        return edgeEnd - nodes[idx].start + 1;
    }

    int GetChild(int node, int ch) const {
        std::map<int, int>::const_iterator it = nodes[node].children.find(ch);
        if (it == nodes[node].children.end()) {
            return NO_CHILD;
        }
        return it->second;
    }

    void SetChild(int node, int ch, int child) {
        nodes[node].children[ch] = child;
    }

    void Extend(int pos) {
        leafEnd = pos;
        ++remaining;
        int lastInternal = NO_CHILD;

        while (remaining > 0) {
            if (activeLength == 0) {
                activeEdge = pos;
            }

            int edgeCh = text[activeEdge];
            int next = GetChild(activeNode, edgeCh);

            if (next == NO_CHILD) {
                SetChild(activeNode, edgeCh, NewNode(pos, LEAF_END));
                if (lastInternal != NO_CHILD) {
                    nodes[lastInternal].suffixLink = activeNode;
                    lastInternal = NO_CHILD;
                }
            } else {
                int edgeLength = EdgeLen(next);
                if (activeLength >= edgeLength) {
                    activeEdge += edgeLength;
                    activeLength -= edgeLength;
                    activeNode = next;
                    continue;
                }

                int splitCh = text[nodes[next].start + activeLength];
                int curCh = text[pos];

                if (splitCh == curCh) {
                    ++activeLength;
                    if (lastInternal != NO_CHILD) {
                        nodes[lastInternal].suffixLink = activeNode;
                    }
                    break;
                }

                int split = NewNode(nodes[next].start, nodes[next].start + activeLength - 1);
                SetChild(activeNode, edgeCh, split);
                SetChild(split, curCh, NewNode(pos, LEAF_END));
                nodes[next].start += activeLength;
                SetChild(split, splitCh, next);

                if (lastInternal != NO_CHILD) {
                    nodes[lastInternal].suffixLink = split;
                }
                lastInternal = split;
            }

            --remaining;

            if (activeNode == ROOT && activeLength > 0) {
                --activeLength;
                activeEdge = pos - remaining + 1;
            } else if (activeNode != ROOT) {
                activeNode = nodes[activeNode].suffixLink;
            }
        }
    }

    std::string MakeString(int start, int len) const {
        std::string result;
        result.reserve(static_cast<size_t>(len));
        for (int i = 0; i < len; ++i) {
            result.push_back(static_cast<char>(text[start + i]));
        }
        return result;
    }

    int Dfs(int idx, int depth, int sep, int &bestLen, std::set<std::string> &results,
            int &firstStart) const {
        const TNode &node = nodes[idx];
        firstStart = NO_CHILD;

        if (node.children.empty()) {
            int suffixStart = leafEnd + 1 - depth;
            if (suffixStart < sep) {
                firstStart = suffixStart;
                return COV_FIRST;
            }
            if (suffixStart > sep && suffixStart < static_cast<int>(text.size()) - 1) {
                return COV_SECOND;
            }
            return COV_NONE;
        }

        int coverage = COV_NONE;
        for (std::map<int, int>::const_iterator it = node.children.begin();
             it != node.children.end(); ++it) {
            int childFirstStart = NO_CHILD;
            int childCoverage = Dfs(it->second, depth + EdgeLen(it->second), sep, bestLen, results,
                                    childFirstStart);
            coverage |= childCoverage;
            if (firstStart == NO_CHILD && childFirstStart != NO_CHILD) {
                firstStart = childFirstStart;
            }
        }

        if (coverage == COV_BOTH && idx != ROOT && depth > 0 && firstStart != NO_CHILD
            && firstStart + depth <= sep) {
            if (depth > bestLen) {
                bestLen = depth;
                results.clear();
            }
            if (depth == bestLen) {
                results.insert(MakeString(firstStart, depth));
            }
        }

        return coverage;
    }
};

std::vector<int> BuildCombinedText(const std::string &first, const std::string &second) {
    std::vector<int> result;
    result.reserve(first.size() + second.size() + 2);

    for (unsigned char ch : first) {
        result.push_back(static_cast<int>(ch));
    }
    result.push_back(FIRST_SEPARATOR);
    for (unsigned char ch : second) {
        result.push_back(static_cast<int>(ch));
    }
    result.push_back(SECOND_SEPARATOR);

    return result;
}

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    std::string first;
    std::string second;

    if (!std::getline(std::cin, first)) {
        return 0;
    }
    if (!std::getline(std::cin, second)) {
        return 0;
    }

    if (!first.empty() && first.back() == '\r') {
        first.pop_back();
    }
    if (!second.empty() && second.back() == '\r') {
        second.pop_back();
    }

    if (first.empty() || second.empty()) {
        std::cout << 0 << "\n";
        return 0;
    }

    int sep = static_cast<int>(first.size());
    std::vector<int> combined = BuildCombinedText(first, second);
    TSuffixTree tree(combined);

    int lcsLen = 0;
    std::set<std::string> results;
    tree.FindLcs(sep, lcsLen, results);

    std::cout << lcsLen << "\n";
    for (const std::string &result : results) {
        std::cout << result << "\n";
    }

    return 0;
}
