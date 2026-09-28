#include <algorithm>
#include <array>
#include <charconv>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <iosfwd>
#include <iostream>
#include <istream>
#include <new>
#include <optional>
#include <ostream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <system_error>
#include <utility>
#include <vector>

inline constexpr std::size_t MAX_KEY_LENGTH = 256;
inline constexpr std::uint32_t FILE_VERSION = 1;

class PatriciaError : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};

bool isValidKey(std::string_view key);
std::string normalizeKey(std::string_view key);

class PatriciaTrie {
public:
    PatriciaTrie() = default;
    ~PatriciaTrie();

    PatriciaTrie(const PatriciaTrie&) = delete;
    PatriciaTrie& operator=(const PatriciaTrie&) = delete;
    PatriciaTrie(PatriciaTrie&& other) noexcept;
    PatriciaTrie& operator=(PatriciaTrie&& other) noexcept;

    [[nodiscard]] std::optional<std::uint64_t> find(std::string_view key) const;
    bool insert(std::string_view key, std::uint64_t value);
    bool remove(std::string_view key);

    [[nodiscard]] std::size_t size() const { return size_; }
    [[nodiscard]] bool empty() const { return size_ == 0; }
    void clear();
    void swap(PatriciaTrie& other) noexcept;

    void save(std::ostream& output) const;
    static PatriciaTrie load(std::istream& input);
    void saveToFile(const std::string& path) const;
    void loadFromFile(const std::string& path);

private:
    struct Node {
        std::string key;
        std::uint64_t value = 0;
        std::size_t checkBit = 0;
        Node* left = nullptr;
        Node* right = nullptr;
    };

    [[nodiscard]] Node* searchNode(std::string_view key) const;
    [[nodiscard]] Node** findParentLink(const Node* target, std::string_view key) const;
    void removeNodeWithSelfLoop(Node* target, std::string_view key);
    void removeNodeWithoutSelfLoop(Node* target, std::string_view key);
    static void saveSubtree(std::ostream& output, const Node* node);
    static void destroySubtree(Node* node);

    Node* header_ = nullptr;
    std::size_t size_ = 0;
};

class Dictionary {
public:
    std::optional<std::string> execute(std::string_view line);

private:
    std::string add(std::string_view key, std::string_view value);
    std::string erase(std::string_view key);
    std::string lookup(std::string_view key) const;
    std::string runFileCommand(std::string_view command, const std::string& path);

    PatriciaTrie trie_;
};

void runDictionary(std::istream& input, std::ostream& output);

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

namespace {

bool isSpace(char symbol) {
    return symbol == ' ' || symbol == '\t' || symbol == '\r' || symbol == '\n' || symbol == '\v' ||
           symbol == '\f';
}

std::vector<std::string_view> splitTokens(std::string_view line) {
    std::vector<std::string_view> tokens;
    std::size_t position = 0;
    while (position < line.size()) {
        while (position < line.size() && isSpace(line[position])) {
            ++position;
        }
        const std::size_t start = position;
        while (position < line.size() && !isSpace(line[position])) {
            ++position;
        }
        if (position > start) {
            tokens.push_back(line.substr(start, position - start));
        }
    }
    return tokens;
}

std::optional<std::uint64_t> parseValue(std::string_view text) {
    const bool allDigits =
        std::ranges::all_of(text, [](char symbol) { return symbol >= '0' && symbol <= '9'; });
    if (text.empty() || !allDigits) {
        return std::nullopt;
    }
    std::uint64_t value = 0;
    const auto [end, error] = std::from_chars(text.data(), text.data() + text.size(), value);
    if (error != std::errc{} || end != text.data() + text.size()) {
        return std::nullopt;
    }
    return value;
}

constexpr std::string_view INVALID_COMMAND = "ERROR: invalid command";
constexpr std::string_view UNKNOWN_COMMAND = "ERROR: unknown command";
constexpr std::string_view INVALID_KEY = "ERROR: invalid key";
constexpr std::string_view INVALID_VALUE = "ERROR: invalid value";

}

std::optional<std::string> Dictionary::execute(std::string_view line) {
    const std::vector<std::string_view> tokens = splitTokens(line);
    if (tokens.empty()) {
        return std::nullopt;
    }
    const std::string_view command = tokens.front();
    if (command == "+") {
        if (tokens.size() != 3) {
            return std::string(INVALID_COMMAND);
        }
        return add(tokens[1], tokens[2]);
    }
    if (command == "-") {
        if (tokens.size() != 2) {
            return std::string(INVALID_COMMAND);
        }
        return erase(tokens[1]);
    }
    if (command == "!") {
        if (tokens.size() < 2 || (tokens[1] != "Save" && tokens[1] != "Load")) {
            return std::string(UNKNOWN_COMMAND);
        }
        if (tokens.size() != 3) {
            return std::string(INVALID_COMMAND);
        }
        return runFileCommand(tokens[1], std::string(tokens[2]));
    }
    if (tokens.size() != 1) {
        return std::string(UNKNOWN_COMMAND);
    }
    return lookup(command);
}

std::string Dictionary::add(std::string_view key, std::string_view value) {
    if (!isValidKey(key)) {
        return std::string(INVALID_KEY);
    }
    const std::optional<std::uint64_t> number = parseValue(value);
    if (!number) {
        return std::string(INVALID_VALUE);
    }
    return trie_.insert(normalizeKey(key), *number) ? "OK" : "Exist";
}

std::string Dictionary::erase(std::string_view key) {
    if (!isValidKey(key)) {
        return std::string(INVALID_KEY);
    }
    return trie_.remove(normalizeKey(key)) ? "OK" : "NoSuchWord";
}

std::string Dictionary::lookup(std::string_view key) const {
    if (!isValidKey(key)) {
        return std::string(INVALID_KEY);
    }
    const std::optional<std::uint64_t> value = trie_.find(normalizeKey(key));
    if (!value) {
        return "NoSuchWord";
    }
    return "OK: " + std::to_string(*value);
}

std::string Dictionary::runFileCommand(std::string_view command, const std::string& path) {
    try {
        if (command == "Save") {
            trie_.saveToFile(path);
        } else {
            trie_.loadFromFile(path);
        }
    } catch (const PatriciaError& error) {
        return std::string("ERROR: ") + error.what();
    }
    return "OK";
}

void runDictionary(std::istream& input, std::ostream& output) {
    Dictionary dictionary;
    std::string line;
    while (std::getline(input, line)) {
        std::optional<std::string> response;
        try {
            response = dictionary.execute(line);
        } catch (const std::bad_alloc&) {
            response = "ERROR: out of memory";
        }
        if (response) {
            output << *response << '\n';
        }
    }
}

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    runDictionary(std::cin, std::cout);
    return 0;
}
