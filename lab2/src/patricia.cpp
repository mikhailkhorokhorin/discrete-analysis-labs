#include "patricia.hpp"

#include <cctype>
#include <cstdint>
#include <fstream>
#include <string>
#include <utility>

const uint16_t MAX_KEY_LENGTH = 256;

static int ExtractBit(const std::string &key, int pos) {
    int byteIdx = (pos - 1) / 8;
    int bitOff = 7 - ((pos - 1) % 8);
    if (byteIdx >= static_cast<int>(key.size())) {
        return 0;
    }
    return (static_cast<unsigned char>(key[byteIdx]) >> bitOff) & 1;
}

static std::string NormalizeKey(const std::string &s) {
    std::string result(s.size(), '\0');
    for (int i = 0; i < static_cast<int>(s.size()); i++) {
        result[i] = static_cast<char>(tolower(static_cast<unsigned char>(s[i])));
    }
    return result;
}

static int FindFirstDiffBit(const std::string &a, const std::string &b) {
    int maxLen = static_cast<int>(a.size() > b.size() ? a.size() : b.size());
    for (int i = 0; i < maxLen; i++) {
        unsigned char x = (i < static_cast<int>(a.size())) ? static_cast<unsigned char>(a[i]) : 0;
        unsigned char y = (i < static_cast<int>(b.size())) ? static_cast<unsigned char>(b[i]) : 0;
        if (x != y) {
            unsigned char diff = x ^ y;
            int shift = 0;
            while ((diff & 0x80) == 0) {
                diff <<= 1;
                shift++;
            }
            return i * 8 + shift + 1;
        }
    }
    return maxLen * 8 + 1;
}

TNode::TNode(const std::string &k, uint64_t v, int b)
    : key(k), value(v), checkBit(b), left(nullptr), right(nullptr) {}

TPatriciaTrie::TPatriciaTrie() : header(nullptr), size(0) {}

TNode *TPatriciaTrie::SearchNode(const std::string &key) const {
    TNode *prev = header;
    TNode *cur = header->left;
    while (cur->checkBit > prev->checkBit) {
        prev = cur;
        if (ExtractBit(key, cur->checkBit)) {
            cur = cur->right;
        } else {
            cur = cur->left;
        }
    }
    return cur;
}

bool TPatriciaTrie::Find(const std::string &key, uint64_t &value) const {
    if (!header) {
        return false;
    }
    TNode *found = SearchNode(NormalizeKey(key));
    if (found->key != NormalizeKey(key)) {
        return false;
    }
    value = found->value;
    return true;
}

bool TPatriciaTrie::Insert(const std::string &key, uint64_t value) {
    std::string lkey = NormalizeKey(key);

    if (!header) {
        header = new TNode(lkey, value, 0);
        header->left = header;
        header->right = nullptr;
        size++;
        return true;
    }

    TNode *existing = SearchNode(lkey);
    if (existing->key == lkey) {
        return false;
    }

    int diffBit = FindFirstDiffBit(lkey, existing->key);

    TNode *prev = header;
    TNode *cur = header->left;
    while (cur->checkBit > prev->checkBit && cur->checkBit < diffBit) {
        prev = cur;
        if (ExtractBit(lkey, cur->checkBit)) {
            cur = cur->right;
        } else {
            cur = cur->left;
        }
    }

    TNode *newNode = new TNode(lkey, value, diffBit);
    if (ExtractBit(lkey, diffBit)) {
        newNode->right = newNode;
        newNode->left = cur;
    } else {
        newNode->left = newNode;
        newNode->right = cur;
    }

    if (prev == header) {
        header->left = newNode;
    } else if (ExtractBit(lkey, prev->checkBit)) {
        prev->right = newNode;
    } else {
        prev->left = newNode;
    }

    size++;
    return true;
}

TNode **TPatriciaTrie::FindParentPtr(TNode *target, const std::string &key) const {
    TNode **ptr = &header->left;
    TNode *par = header;
    TNode *cur = header->left;
    while (cur->checkBit > par->checkBit) {
        if (cur == target) {
            return ptr;
        }
        par = cur;
        if (ExtractBit(key, cur->checkBit)) {
            ptr = &cur->right;
            cur = cur->right;
        } else {
            ptr = &cur->left;
            cur = cur->left;
        }
    }
    return ptr;
}

bool TPatriciaTrie::Remove(const std::string &key) {
    if (!header) {
        return false;
    }
    std::string lkey = NormalizeKey(key);

    TNode *target = SearchNode(lkey);
    if (target->key != lkey) {
        return false;
    }

    if (target->left == target || target->right == target) {
        if (target == header) {
            delete header;
            header = nullptr;
            size--;
            return true;
        }
        TNode *other = (target->left == target) ? target->right : target->left;
        *FindParentPtr(target, lkey) = other;
        delete target;
        size--;
        return true;
    }

    TNode *grandPrev = header;
    TNode *prev = header;
    TNode *cur = header->left;
    while (cur->checkBit > prev->checkBit) {
        grandPrev = prev;
        prev = cur;
        if (ExtractBit(lkey, cur->checkBit)) {
            cur = cur->right;
        } else {
            cur = cur->left;
        }
    }
    TNode *backEdgeHolder = prev;

    std::string holderKey = backEdgeHolder->key;
    uint64_t holderVal = backEdgeHolder->value;

    TNode *holderChild;
    if (backEdgeHolder->left == backEdgeHolder || backEdgeHolder->right == backEdgeHolder) {
        holderChild = (backEdgeHolder->left == target) ? backEdgeHolder->left : backEdgeHolder->right;
    } else {
        holderChild = (backEdgeHolder->left == target) ? backEdgeHolder->right : backEdgeHolder->left;
    }

    TNode *rPrev = header;
    TNode *rCur = header->left;
    while (rCur->checkBit > rPrev->checkBit) {
        rPrev = rCur;
        if (ExtractBit(holderKey, rCur->checkBit)) {
            rCur = rCur->right;
        } else {
            rCur = rCur->left;
        }
    }
    TNode *backEdgeSource = rPrev;

    target->key = holderKey;
    target->value = holderVal;

    if (backEdgeSource->left == backEdgeHolder) {
        backEdgeSource->left = target;
    }
    if (backEdgeSource->right == backEdgeHolder) {
        backEdgeSource->right = target;
    }
    if (target->left == backEdgeHolder) {
        target->left = holderChild;
    }
    if (target->right == backEdgeHolder) {
        target->right = holderChild;
    }
    if (grandPrev->left == backEdgeHolder) {
        grandPrev->left = holderChild;
    } else if (grandPrev->right == backEdgeHolder) {
        grandPrev->right = holderChild;
    }

    delete backEdgeHolder;
    size--;
    return true;
}

void TPatriciaTrie::SaveSubtree(std::ofstream &file, TNode *cur) const {
    uint16_t klen = static_cast<uint16_t>(cur->key.size());
    file.write(reinterpret_cast<const char *>(&klen), sizeof(klen));
    file.write(cur->key.data(), klen);
    file.write(reinterpret_cast<const char *>(&cur->value), sizeof(cur->value));
    if (cur->left && cur->left->checkBit > cur->checkBit) {
        SaveSubtree(file, cur->left);
    }
    if (cur->right && cur->right->checkBit > cur->checkBit) {
        SaveSubtree(file, cur->right);
    }
}

void TPatriciaTrie::DestroySubtree(TNode *cur) {
    if (cur->left && cur->left->checkBit > cur->checkBit) {
        DestroySubtree(cur->left);
    }
    if (cur->right && cur->right->checkBit > cur->checkBit) {
        DestroySubtree(cur->right);
    }
    delete cur;
}

TPatriciaTrie::~TPatriciaTrie() {
    if (header) {
        DestroySubtree(header);
    }
    header = nullptr;
}

bool TPatriciaTrie::Save(const std::string &path) const {
    std::ofstream file(path, std::ios::binary);
    if (!file) {
        return false;
    }
    if (header) {
        SaveSubtree(file, header);
    }
    return true;
}

bool TPatriciaTrie::Load(const std::string &path) {
    std::ifstream file(path, std::ios::binary);
    if (!file) {
        return false;
    }

    TPatriciaTrie tmp;
    while (true) {
        uint16_t klen = 0;
        if (!file.read(reinterpret_cast<char *>(&klen), sizeof(klen))) {
            if (!file.eof()) {
                return false;
            }
            break;
        }
        if (klen == 0 || klen > MAX_KEY_LENGTH) {
            return false;
        }
        std::string key(klen, '\0');
        if (!file.read(&key[0], klen)) {
            return false;
        }
        uint64_t val = 0;
        if (!file.read(reinterpret_cast<char *>(&val), sizeof(val))) {
            return false;
        }
        if (!tmp.Insert(key, val)) {
            return false;
        }
    }

    std::swap(header, tmp.header);
    std::swap(size, tmp.size);
    return true;
}
