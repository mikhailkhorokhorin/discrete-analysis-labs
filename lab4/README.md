# Lab 4. Knuth-Morris-Pratt Search

## Task

Find all occurrences of a pattern in a text with the Knuth-Morris-Pratt algorithm.
Variant: the alphabet consists of numbers from $0$ to $2^{32} - 1$.

## Input and output

- Input: the pattern on the first line, the text on the following lines. Numbers are separated by any whitespace, may have leading zeros, and an occurrence may span several lines.
- Output: one line `line, word` for every occurrence, the position of its first number. Lines and words are counted from 1, the pattern line is not counted.
- A token that is not a number in the allowed range stops the program with `ERROR: pattern: invalid token '...'` or `ERROR: line N: invalid token '...'` on stderr and exit code 1; the occurrences found before it are already printed.

## Algorithm

The prefix function $\pi$ of the pattern $P$ is built first: $\pi[i]$ is the length of the longest proper border of $P[0..i]$.
`KmpMatcher` then reads the text one number at a time and keeps the length of the current match; on a mismatch it falls back along $\pi$ instead of moving back in the text.
The text is processed as a stream: the matcher stores only the positions of the last $m$ numbers in a ring buffer, so it reports the start of an occurrence without keeping the text.

| | Complexity |
| --- | --- |
| Time | $O(m + n)$ for a pattern of length $m$ and a text of length $n$ |
| Memory | $O(m)$: the pattern, the prefix function and the ring buffer of positions |

## Files

| Path | Content |
| --- | --- |
| `include/kmp.hpp`, `src/kmp.cpp` | `buildPrefixFunction`, the streaming `KmpMatcher` and `kmpSearch` over a whole text |
| `include/search.hpp`, `src/search.cpp` | token parsing and `runSearch`, which reads the input line by line |
| `app/main.cpp` | `lab4_main`: runs the search on stdin and stdout |
| `solution.cpp` | `lab4_solution`: the same program in one file for the checker |
| `tests/` | unit tests (including random texts against naive search) and e2e tests on `tests/data/NN.in` and `NN.out` for both binaries |
| `benchmark/` | `lab4_benchmark` (KMP against naive search), `generator.py` (random text), `generator_worst.py` (worst case for naive search), `run.sh` |

## Build and run

```bash
make build LAB=lab4
make run LAB=lab4 < lab4/tests/data/01.in
make run LAB=lab4 APP=solution < lab4/tests/data/01.in
make test LAB=lab4
make bench LAB=lab4
```

`make bench` builds the release preset and runs `benchmark/run.sh` with two series for texts from $10^4$ to $5 \cdot 10^6$ numbers: a random text over 2 numbers with a pattern of 8 (fixed seed), and the worst case for naive search, a pattern of 19 ones and a two in a text of ones.
It prints the median of 5 runs for both algorithms and checks that they find the same occurrences.
A single input can be generated and measured by hand:

```bash
python3 lab4/benchmark/generator.py 1000000 8 42 2
make run LAB=lab4 APP=benchmark PRESET=release < lab4/benchmark/tests.txt
```

## Example

Input (`tests/data/01.in`):

```text
11 45 11 45 90
0011 45 011 0045 11 45 90    11
45 11 45 90
```

Output:

```text
1, 3
1, 8
```
