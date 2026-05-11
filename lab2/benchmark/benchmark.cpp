#include "patricia.hpp"

#include <chrono>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

using TDuration = std::chrono::microseconds;
const std::string DURATION_SUFFIX = "us";

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    std::vector<std::pair<std::string, uint64_t>> inserts;
    std::vector<std::string> lookups;
    std::vector<std::string> removes;

    std::string line;
    while (std::getline(std::cin, line)) {
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }
        if (line.empty()) {
            continue;
        }
        if (line[0] == '+') {
            std::string word;
            uint64_t val = 0;
            std::istringstream ss(line.substr(2));
            ss >> word >> val;
            inserts.emplace_back(word, val);
        } else if (line[0] == '?') {
            lookups.push_back(line.substr(2));
        } else if (line[0] == '-') {
            removes.push_back(line.substr(2));
        } else {
            lookups.push_back(line);
        }
    }

    long long patriciaInsert = 0;
    long long patriciaFind = 0;
    long long patriciaRemove = 0;

    {
        TPatriciaTrie dict;

        auto start = std::chrono::high_resolution_clock::now();
        for (const std::pair<std::string, uint64_t> &p : inserts) {
            dict.Insert(p.first, p.second);
        }
        auto end = std::chrono::high_resolution_clock::now();
        patriciaInsert = std::chrono::duration_cast<TDuration>(end - start).count();

        volatile size_t sink = 0;
        start = std::chrono::high_resolution_clock::now();
        for (const std::string &w : lookups) {
            uint64_t val = 0;
            sink += dict.Find(w, val) ? 1 : 0;
        }
        end = std::chrono::high_resolution_clock::now();
        patriciaFind = std::chrono::duration_cast<TDuration>(end - start).count();

        start = std::chrono::high_resolution_clock::now();
        for (const std::string &w : removes) {
            dict.Remove(w);
        }
        end = std::chrono::high_resolution_clock::now();
        patriciaRemove = std::chrono::duration_cast<TDuration>(end - start).count();
    }

    long long mapInsert = 0;
    long long mapFind = 0;
    long long mapRemove = 0;

    {
        std::map<std::string, uint64_t> dict;

        auto start = std::chrono::high_resolution_clock::now();
        for (const std::pair<std::string, uint64_t> &p : inserts) {
            dict.emplace(p.first, p.second);
        }
        auto end = std::chrono::high_resolution_clock::now();
        mapInsert = std::chrono::duration_cast<TDuration>(end - start).count();

        volatile size_t sink = 0;
        start = std::chrono::high_resolution_clock::now();
        for (const std::string &w : lookups) {
            sink += (dict.find(w) != dict.end()) ? 1 : 0;
        }
        end = std::chrono::high_resolution_clock::now();
        mapFind = std::chrono::duration_cast<TDuration>(end - start).count();

        start = std::chrono::high_resolution_clock::now();
        for (const std::string &w : removes) {
            dict.erase(w);
        }
        end = std::chrono::high_resolution_clock::now();
        mapRemove = std::chrono::duration_cast<TDuration>(end - start).count();
    }

    std::cout << "N=" << inserts.size() << "\n";
    std::cout << "PATRICIA  Insert=" << patriciaInsert << DURATION_SUFFIX
              << " Find=" << patriciaFind << DURATION_SUFFIX
              << " Remove=" << patriciaRemove << DURATION_SUFFIX << "\n";
    std::cout << "std::map  Insert=" << mapInsert << DURATION_SUFFIX
              << " Find=" << mapFind << DURATION_SUFFIX
              << " Remove=" << mapRemove << DURATION_SUFFIX << "\n";

    return 0;
}
