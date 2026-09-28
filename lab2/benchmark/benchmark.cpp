#include <chrono>
#include <cstdint>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include "patricia_trie.hpp"

namespace {

using Clock = std::chrono::steady_clock;
using Duration = std::chrono::microseconds;

struct Workload {
    std::vector<std::pair<std::string, std::uint64_t>> inserts;
    std::vector<std::string> lookups;
    std::vector<std::string> removes;
};

struct Timings {
    long long insert = 0;
    long long find = 0;
    long long remove = 0;
};

long long elapsedSince(Clock::time_point start) {
    return std::chrono::duration_cast<Duration>(Clock::now() - start).count();
}

Workload readWorkload(std::istream& input) {
    Workload workload;
    std::string line;
    while (std::getline(input, line)) {
        std::istringstream stream(line);
        std::string command;
        if (!(stream >> command)) {
            continue;
        }
        if (command == "+") {
            std::string word;
            std::uint64_t value = 0;
            stream >> word >> value;
            workload.inserts.emplace_back(normalizeKey(word), value);
        } else if (command == "-") {
            std::string word;
            stream >> word;
            workload.removes.push_back(normalizeKey(word));
        } else {
            workload.lookups.push_back(normalizeKey(command));
        }
    }
    return workload;
}

Timings measurePatricia(const Workload& workload) {
    Timings timings;
    PatriciaTrie trie;

    auto start = Clock::now();
    for (const auto& [word, value] : workload.inserts) {
        trie.insert(word, value);
    }
    timings.insert = elapsedSince(start);

    std::size_t found = 0;
    start = Clock::now();
    for (const std::string& word : workload.lookups) {
        found += trie.find(word) ? 1 : 0;
    }
    timings.find = elapsedSince(start);

    start = Clock::now();
    for (const std::string& word : workload.removes) {
        trie.remove(word);
    }
    timings.remove = elapsedSince(start);

    std::cerr << "PATRICIA found " << found << " words\n";
    return timings;
}

Timings measureMap(const Workload& workload) {
    Timings timings;
    std::map<std::string, std::uint64_t> map;

    auto start = Clock::now();
    for (const auto& [word, value] : workload.inserts) {
        map.emplace(word, value);
    }
    timings.insert = elapsedSince(start);

    std::size_t found = 0;
    start = Clock::now();
    for (const std::string& word : workload.lookups) {
        found += map.contains(word) ? 1 : 0;
    }
    timings.find = elapsedSince(start);

    start = Clock::now();
    for (const std::string& word : workload.removes) {
        map.erase(word);
    }
    timings.remove = elapsedSince(start);

    std::cerr << "std::map found " << found << " words\n";
    return timings;
}

void printTimings(const std::string& name, const Timings& timings) {
    std::cout << name << " Insert=" << timings.insert << "us Find=" << timings.find
              << "us Remove=" << timings.remove << "us\n";
}

}

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    const Workload workload = readWorkload(std::cin);
    const Timings patricia = measurePatricia(workload);
    const Timings map = measureMap(workload);

    std::cout << "N=" << workload.inserts.size() << "\n";
    printTimings("PATRICIA ", patricia);
    printTimings("std::map ", map);
    return 0;
}
