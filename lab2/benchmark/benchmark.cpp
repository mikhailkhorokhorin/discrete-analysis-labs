#include <algorithm>
#include <array>
#include <chrono>
#include <cstddef>
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

constexpr std::size_t RUNS = 5;

struct Workload {
    std::vector<std::pair<std::string, std::uint64_t>> inserts;
    std::vector<std::string> lookups;
    std::vector<std::string> removes;
};

struct Timings {
    std::array<long long, RUNS> insert{};
    std::array<long long, RUNS> find{};
    std::array<long long, RUNS> remove{};
};

long long elapsedSince(Clock::time_point start) {
    return std::chrono::duration_cast<Duration>(Clock::now() - start).count();
}

long long median(std::array<long long, RUNS> timings) {
    std::ranges::sort(timings);
    return timings[RUNS / 2];
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

std::size_t runPatricia(const Workload& workload, Timings& timings, std::size_t run) {
    PatriciaTrie trie;

    auto start = Clock::now();
    for (const auto& [word, value] : workload.inserts) {
        trie.insert(word, value);
    }
    timings.insert[run] = elapsedSince(start);

    std::size_t found = 0;
    start = Clock::now();
    for (const std::string& word : workload.lookups) {
        found += trie.find(word) ? 1 : 0;
    }
    timings.find[run] = elapsedSince(start);

    start = Clock::now();
    for (const std::string& word : workload.removes) {
        trie.remove(word);
    }
    timings.remove[run] = elapsedSince(start);
    return found;
}

std::size_t runMap(const Workload& workload, Timings& timings, std::size_t run) {
    std::map<std::string, std::uint64_t> map;

    auto start = Clock::now();
    for (const auto& [word, value] : workload.inserts) {
        map.emplace(word, value);
    }
    timings.insert[run] = elapsedSince(start);

    std::size_t found = 0;
    start = Clock::now();
    for (const std::string& word : workload.lookups) {
        found += map.contains(word) ? 1 : 0;
    }
    timings.find[run] = elapsedSince(start);

    start = Clock::now();
    for (const std::string& word : workload.removes) {
        map.erase(word);
    }
    timings.remove[run] = elapsedSince(start);
    return found;
}

void printTimings(const std::string& name, const Timings& timings) {
    std::cout << name << " Insert=" << median(timings.insert) << "us Find=" << median(timings.find)
              << "us Remove=" << median(timings.remove) << "us\n";
}

}

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    const Workload workload = readWorkload(std::cin);

    Timings patricia;
    Timings map;
    bool equal = true;
    for (std::size_t run = 0; run < RUNS; ++run) {
        const std::size_t patriciaFound = runPatricia(workload, patricia, run);
        const std::size_t mapFound = runMap(workload, map, run);
        equal = equal && patriciaFound == mapFound;
    }

    printTimings("PATRICIA ", patricia);
    printTimings("std::map ", map);
    std::cout << "Results equal: " << (equal ? "yes" : "no") << "\n";
    return equal ? 0 : 1;
}
