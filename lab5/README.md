# Lab 5. Longest Common Substring

## Task

Find the longest common substrings of two strings with a suffix tree.

## Input and output

- Input: two strings, one per line. A trailing `\r` is ignored, a missing second line is treated as an empty string.
- Output: the length of the longest common substring, then every common substring of that length on its own line, in lexicographic order and without duplicates.

## Algorithm

A generalized suffix tree of the text `first#second$` is built with Ukkonen's algorithm; `#` and `$` are two separators that do not occur in the input.
An iterative post-order traversal with an explicit stack marks every node with the strings its leaves come from: a leaf belongs to the first string if its suffix starts before `#`, and to the second one if it starts after it.
The deepest nodes marked by both strings spell the longest common substrings; each one is restored from a suffix of the first string and collected in a sorted set.

| | Complexity |
| --- | --- |
| Time | $O(n \log \sigma)$ to build the tree and $O(n)$ to traverse it, where $n = \lvert S_1 \rvert + \lvert S_2 \rvert$ and $\sigma \le 258$ (256 bytes and two separators); the answers add $O(k \cdot \ell \log k)$ for $k$ substrings of length $\ell$ |
| Memory | $O(n)$: at most $2n + 1$ nodes, each with a map of its children |

## Files

| Path | Content |
| --- | --- |
| `include/suffix_tree.hpp`, `src/suffix_tree.cpp` | `SuffixTree` (Ukkonen's construction and `findLcs`) and `buildCombinedText` |
| `include/lcs.hpp`, `src/lcs.cpp` | `longestCommonSubstrings` and `runLcs`, which reads the input and prints the answer |
| `app/main.cpp` | `lab5_main`: runs the search on stdin and stdout |
| `solution.cpp` | `lab5_solution`: the same program in one file for the checker |
| `tests/` | unit tests (including random strings against brute force) and e2e tests on `tests/data/NN.in` and `NN.out` for both binaries |
| `benchmark/` | `lab5_benchmark` (suffix tree against dynamic programming), `generator.py`, `run.sh` |

## Build and run

```bash
make build LAB=lab5
make run LAB=lab5 < lab5/tests/data/01.in
make run LAB=lab5 APP=solution < lab5/tests/data/01.in
make test LAB=lab5
make bench LAB=lab5
```

`make bench` builds the release preset and runs `benchmark/run.sh`: for two random strings of length $10^3$ to $10^5$ over 4 letters (fixed seed) it prints the median of 5 runs for the suffix tree and for the $O(\lvert S_1 \rvert \cdot \lvert S_2 \rvert)$ dynamic programming, and checks that both give the same answer.
The dynamic programming is skipped when the table would have more than $25 \cdot 10^6$ cells.
A single input can be generated and measured by hand:

```bash
python3 lab5/benchmark/generator.py 4000 4000 4 42
make run LAB=lab5 APP=benchmark PRESET=release < lab5/benchmark/tests.txt
```

## Example

Input (`tests/data/01.in`):

```text
xabay
xabcbay
```

Output:

```text
3
bay
xab
```
