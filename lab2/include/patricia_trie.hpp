#pragma once

#include <cstddef>
#include <cstdint>
#include <iosfwd>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>

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
