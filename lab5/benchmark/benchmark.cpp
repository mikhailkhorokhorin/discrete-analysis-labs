#include "suffix_tree.hpp"

#include <chrono>
#include <iostream>
#include <set>
#include <string>
#include <vector>

using TDuration = std::chrono::microseconds;
const std::string DURATION_SUFFIX = "us";
const long long DP_CELL_LIMIT = 25000000LL;

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    std::string s1;
    std::string s2;

    if (!std::getline(std::cin, s1)) {
        return 0;
    }
    if (!std::getline(std::cin, s2)) {
        return 0;
    }

    if (!s1.empty() && s1.back() == '\r') {
        s1.pop_back();
    }
    if (!s2.empty() && s2.back() == '\r') {
        s2.pop_back();
    }

    int sep = static_cast<int>(s1.size());
    std::vector<int> combined = BuildCombinedText(s1, s2);

    auto start = std::chrono::high_resolution_clock::now();

    TSuffixTree tree(combined);
    int lcsLen = 0;
    std::set<std::string> results;
    tree.FindLcs(sep, lcsLen, results);

    auto end = std::chrono::high_resolution_clock::now();
    long long elapsed = std::chrono::duration_cast<TDuration>(end - start).count();

    std::cout << "Suffix tree LCS time: " << elapsed << DURATION_SUFFIX << "\n";
    std::cout << "LCS length: " << lcsLen << "\n";
    std::cout << "LCS count: " << results.size() << "\n";

    int n1 = static_cast<int>(s1.size());
    int n2 = static_cast<int>(s2.size());
    long long dpCells = static_cast<long long>(n1 + 1) * static_cast<long long>(n2 + 1);

    if (dpCells <= DP_CELL_LIMIT) {
        auto startDp = std::chrono::high_resolution_clock::now();

        int dpBest = 0;
        std::set<std::string> dpResults;
        std::vector<std::vector<int>> dp(n1 + 1, std::vector<int>(n2 + 1, 0));

        for (int i = 1; i <= n1; ++i) {
            for (int j = 1; j <= n2; ++j) {
                if (s1[i - 1] == s2[j - 1]) {
                    dp[i][j] = dp[i - 1][j - 1] + 1;
                    if (dp[i][j] > dpBest) {
                        dpBest = dp[i][j];
                        dpResults.clear();
                    }
                    if (dp[i][j] == dpBest) {
                        dpResults.insert(s1.substr(static_cast<size_t>(i - dpBest),
                                                   static_cast<size_t>(dpBest)));
                    }
                }
            }
        }

        auto endDp = std::chrono::high_resolution_clock::now();
        long long elapsedDp = std::chrono::duration_cast<TDuration>(endDp - startDp).count();

        std::cout << "DP LCS time: " << elapsedDp << DURATION_SUFFIX << "\n";
        std::cout << "DP LCS length: " << dpBest << "\n";
        std::cout << "DP LCS count: " << dpResults.size() << "\n";
        std::cout << "Results equal: "
                  << (lcsLen == dpBest && results == dpResults ? "yes" : "no") << "\n";

        return (lcsLen == dpBest && results == dpResults) ? 0 : 1;
    }

    std::cout << "DP LCS skipped: input is too large\n";

    return 0;
}
