#include "patricia_trie.hpp"

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <istream>
#include <optional>
#include <ostream>
#include <string>
#include <string_view>
#include <utility>

namespace {

constexpr std::size_t BITS_PER_BYTE = 8;
constexpr unsigned HIGH_BIT = 0x80U;
constexpr unsigned BYTE_MASK = 0xFFU;
constexpr std::array<char, 8> FILE_MAGIC = {'P', 'A', 'T', 'R', 'I', 'C', 'I', 'A'};

bool extractBit(std::string_view key, std::size_t position) {
    const std::size_t byteIndex = (position - 1) / BITS_PER_BYTE;
    const std::size_t bitOffset = BITS_PER_BYTE - 1 - (position - 1) % BITS_PER_BYTE;
    if (byteIndex >= key.size()) {
        return false;
    }
    return ((static_cast<unsigned char>(key[byteIndex]) >> bitOffset) & 1U) != 0;
}

std::size_t firstDifferentBit(std::string_view lhs, std::string_view rhs) {
    const std::size_t maxLength = std::max(lhs.size(), rhs.size());
    for (std::size_t i = 0; i < maxLength; ++i) {
        const unsigned left = i < lhs.size() ? static_cast<unsigned char>(lhs[i]) : 0U;
        const unsigned right = i < rhs.size() ? static_cast<unsigned char>(rhs[i]) : 0U;
        if (left != right) {
            unsigned diff = left ^ right;
            std::size_t shift = 0;
            while ((diff & HIGH_BIT) == 0) {
                diff <<= 1U;
                ++shift;
            }
            return i * BITS_PER_BYTE + shift + 1;
        }
    }
    return maxLength * BITS_PER_BYTE + 1;
}

void writeUnsigned(std::ostream& output, std::uint64_t value, std::size_t bytes) {
    for (std::size_t i = 0; i < bytes; ++i) {
        output.put(static_cast<char>((value >> (i * BITS_PER_BYTE)) & BYTE_MASK));
    }
}

std::uint64_t readUnsigned(std::istream& input, std::size_t bytes) {
    std::uint64_t value = 0;
    for (std::size_t i = 0; i < bytes; ++i) {
        const int symbol = input.get();
        if (symbol == std::char_traits<char>::eof()) {
            throw PatriciaError("invalid file format");
        }
        value |= static_cast<std::uint64_t>(static_cast<unsigned char>(symbol))
                 << (i * BITS_PER_BYTE);
    }
    return value;
}

}

bool isValidKey(std::string_view key) {
    return !key.empty() && key.size() <= MAX_KEY_LENGTH &&
           std::ranges::all_of(key, [](char symbol) {
               return (symbol >= 'a' && symbol <= 'z') || (symbol >= 'A' && symbol <= 'Z');
           });
}

std::string normalizeKey(std::string_view key) {
    std::string result(key);
    for (char& symbol : result) {
        if (symbol >= 'A' && symbol <= 'Z') {
            symbol = static_cast<char>(symbol - 'A' + 'a');
        }
    }
    return result;
}

PatriciaTrie::~PatriciaTrie() {
    clear();
}

PatriciaTrie::PatriciaTrie(PatriciaTrie&& other) noexcept
    : header_(std::exchange(other.header_, nullptr)), size_(std::exchange(other.size_, 0)) {
}

PatriciaTrie& PatriciaTrie::operator=(PatriciaTrie&& other) noexcept {
    if (this != &other) {
        clear();
        header_ = std::exchange(other.header_, nullptr);
        size_ = std::exchange(other.size_, 0);
    }
    return *this;
}

void PatriciaTrie::clear() {
    if (header_ != nullptr) {
        destroySubtree(header_);
    }
    header_ = nullptr;
    size_ = 0;
}

void PatriciaTrie::swap(PatriciaTrie& other) noexcept {
    std::swap(header_, other.header_);
    std::swap(size_, other.size_);
}

PatriciaTrie::Node* PatriciaTrie::searchNode(std::string_view key) const {
    const Node* previous = header_;
    Node* current = header_->left;
    while (current->checkBit > previous->checkBit) {
        previous = current;
        current = extractBit(key, current->checkBit) ? current->right : current->left;
    }
    return current;
}

std::optional<std::uint64_t> PatriciaTrie::find(std::string_view key) const {
    if (header_ == nullptr) {
        return std::nullopt;
    }
    const Node* found = searchNode(key);
    if (found->key != key) {
        return std::nullopt;
    }
    return found->value;
}

bool PatriciaTrie::insert(std::string_view key, std::uint64_t value) {
    if (header_ == nullptr) {
        header_ = new Node{.key = std::string(key), .value = value};
        header_->left = header_;
        size_ = 1;
        return true;
    }

    const Node* existing = searchNode(key);
    if (existing->key == key) {
        return false;
    }

    const std::size_t diffBit = firstDifferentBit(key, existing->key);

    Node* previous = header_;
    Node* current = header_->left;
    while (current->checkBit > previous->checkBit && current->checkBit < diffBit) {
        previous = current;
        current = extractBit(key, current->checkBit) ? current->right : current->left;
    }

    Node* node = new Node{.key = std::string(key), .value = value, .checkBit = diffBit};
    if (extractBit(key, diffBit)) {
        node->right = node;
        node->left = current;
    } else {
        node->left = node;
        node->right = current;
    }

    if (previous == header_) {
        header_->left = node;
    } else if (extractBit(key, previous->checkBit)) {
        previous->right = node;
    } else {
        previous->left = node;
    }

    ++size_;
    return true;
}

PatriciaTrie::Node** PatriciaTrie::findParentLink(const Node* target, std::string_view key) const {
    Node** link = &header_->left;
    const Node* parent = header_;
    Node* current = header_->left;
    while (current->checkBit > parent->checkBit && current != target) {
        parent = current;
        link = extractBit(key, current->checkBit) ? &current->right : &current->left;
        current = *link;
    }
    return link;
}

void PatriciaTrie::removeNodeWithSelfLoop(Node* target, std::string_view key) {
    if (target == header_) {
        delete header_;
        header_ = nullptr;
        return;
    }
    Node* other = target->left == target ? target->right : target->left;
    *findParentLink(target, key) = other;
    delete target;
}

void PatriciaTrie::removeNodeWithoutSelfLoop(Node* target, std::string_view key) {
    Node* grandParent = header_;
    Node* holder = header_;
    Node* current = header_->left;
    while (current->checkBit > holder->checkBit) {
        grandParent = holder;
        holder = current;
        current = extractBit(key, current->checkBit) ? current->right : current->left;
    }

    const bool holderHasSelfLoop = holder->left == holder || holder->right == holder;
    Node* holderChild = nullptr;
    if (holderHasSelfLoop) {
        holderChild = target;
    } else {
        holderChild = holder->left == target ? holder->right : holder->left;
    }

    Node* source = header_;
    current = header_->left;
    while (current->checkBit > source->checkBit) {
        source = current;
        current = extractBit(holder->key, current->checkBit) ? current->right : current->left;
    }

    target->key = std::move(holder->key);
    target->value = holder->value;

    if (source->left == holder) {
        source->left = target;
    }
    if (source->right == holder) {
        source->right = target;
    }
    if (target->left == holder) {
        target->left = holderChild;
    }
    if (target->right == holder) {
        target->right = holderChild;
    }
    if (grandParent->left == holder) {
        grandParent->left = holderChild;
    } else if (grandParent->right == holder) {
        grandParent->right = holderChild;
    }

    delete holder;
}

bool PatriciaTrie::remove(std::string_view key) {
    if (header_ == nullptr) {
        return false;
    }
    Node* target = searchNode(key);
    if (target->key != key) {
        return false;
    }
    if (target->left == target || target->right == target) {
        removeNodeWithSelfLoop(target, key);
    } else {
        removeNodeWithoutSelfLoop(target, key);
    }
    --size_;
    return true;
}

void PatriciaTrie::saveSubtree(std::ostream& output, const Node* node) {
    writeUnsigned(output, node->key.size(), sizeof(std::uint16_t));
    output.write(node->key.data(), static_cast<std::streamsize>(node->key.size()));
    writeUnsigned(output, node->value, sizeof(std::uint64_t));
    if (node->left != nullptr && node->left->checkBit > node->checkBit) {
        saveSubtree(output, node->left);
    }
    if (node->right != nullptr && node->right->checkBit > node->checkBit) {
        saveSubtree(output, node->right);
    }
}

void PatriciaTrie::destroySubtree(Node* node) {
    if (node->left != nullptr && node->left->checkBit > node->checkBit) {
        destroySubtree(node->left);
    }
    if (node->right != nullptr && node->right->checkBit > node->checkBit) {
        destroySubtree(node->right);
    }
    delete node;
}

void PatriciaTrie::save(std::ostream& output) const {
    output.write(FILE_MAGIC.data(), static_cast<std::streamsize>(FILE_MAGIC.size()));
    writeUnsigned(output, FILE_VERSION, sizeof(std::uint32_t));
    writeUnsigned(output, size_, sizeof(std::uint64_t));
    if (header_ != nullptr) {
        saveSubtree(output, header_);
    }
    output.flush();
    if (!output.good()) {
        throw PatriciaError("failed to write file");
    }
}

PatriciaTrie PatriciaTrie::load(std::istream& input) {
    std::array<char, FILE_MAGIC.size()> magic{};
    if (!input.read(magic.data(), static_cast<std::streamsize>(magic.size())) ||
        magic != FILE_MAGIC) {
        throw PatriciaError("invalid file format");
    }
    if (readUnsigned(input, sizeof(std::uint32_t)) != FILE_VERSION) {
        throw PatriciaError("unsupported file version");
    }
    const std::uint64_t count = readUnsigned(input, sizeof(std::uint64_t));

    PatriciaTrie trie;
    for (std::uint64_t i = 0; i < count; ++i) {
        const std::uint64_t length = readUnsigned(input, sizeof(std::uint16_t));
        if (length == 0 || length > MAX_KEY_LENGTH) {
            throw PatriciaError("invalid file format");
        }
        std::string key(length, '\0');
        if (!input.read(key.data(), static_cast<std::streamsize>(length)) || !isValidKey(key) ||
            normalizeKey(key) != key) {
            throw PatriciaError("invalid file format");
        }
        const std::uint64_t value = readUnsigned(input, sizeof(std::uint64_t));
        if (!trie.insert(key, value)) {
            throw PatriciaError("invalid file format");
        }
    }
    if (input.peek() != std::char_traits<char>::eof()) {
        throw PatriciaError("invalid file format");
    }
    return trie;
}

void PatriciaTrie::saveToFile(const std::string& path) const {
    std::ofstream file(path, std::ios::binary | std::ios::trunc);
    if (!file) {
        throw PatriciaError("cannot open file for writing");
    }
    save(file);
    file.close();
    if (!file) {
        throw PatriciaError("failed to write file");
    }
}

void PatriciaTrie::loadFromFile(const std::string& path) {
    std::ifstream file(path, std::ios::binary);
    if (!file) {
        throw PatriciaError("cannot open file for reading");
    }
    PatriciaTrie loaded = load(file);
    swap(loaded);
}
