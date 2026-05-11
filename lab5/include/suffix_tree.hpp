#pragma once

#include <map>
#include <set>
#include <string>
#include <vector>

const int FIRST_SEPARATOR = 256;
const int SECOND_SEPARATOR = 257;
const int NO_CHILD = -1;
const int LEAF_END = -1;

struct TNode {
    std::map<int, int> children;
    int suffixLink;
    int start;
    int end;

    TNode(int start, int end);
};

class TSuffixTree {
  public:
    explicit TSuffixTree(const std::vector<int> &text);

    void FindLcs(int sep, int &outLen, std::set<std::string> &outResults) const;

  private:
    std::vector<int> text;
    std::vector<TNode> nodes;
    int leafEnd;

    int activeNode;
    int activeEdge;
    int activeLength;
    int remaining;

    int NewNode(int start, int end);
    int EdgeLen(int idx) const;
    int GetChild(int node, int ch) const;
    void SetChild(int node, int ch, int child);
    void Extend(int pos);
    std::string MakeString(int start, int len) const;

    int Dfs(
        int idx,
        int depth,
        int sep,
        int &bestLen,
        std::set<std::string> &results,
        int &firstStart
    ) const;
};

std::vector<int> BuildCombinedText(const std::string &first, const std::string &second);
