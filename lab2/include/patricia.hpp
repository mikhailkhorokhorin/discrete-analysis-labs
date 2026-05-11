#pragma once

#include <cstdint>
#include <fstream>
#include <string>

struct TNode {
    std::string key;
    uint64_t value;
    int checkBit;
    TNode *left;
    TNode *right;

    TNode(const std::string &k, uint64_t v, int b);
};

class TPatriciaTrie {
  public:
    TPatriciaTrie();
    ~TPatriciaTrie();

    bool Find(const std::string &key, uint64_t &value) const;
    bool Insert(const std::string &key, uint64_t value);
    bool Remove(const std::string &key);
    bool Save(const std::string &path) const;
    bool Load(const std::string &path);

  private:
    TNode *header;
    int size;

    TNode *SearchNode(const std::string &key) const;
    TNode **FindParentPtr(TNode *target, const std::string &key) const;
    void SaveSubtree(std::ofstream &file, TNode *cur) const;
    void DestroySubtree(TNode *cur);
};
