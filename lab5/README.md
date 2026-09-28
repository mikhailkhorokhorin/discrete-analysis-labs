# Lab 5. Longest Common Substring

## Task

Find the longest common substring of two strings with a suffix tree (the variant of this lab).
The input contains two strings, one per line.
Print the length of the longest common substring and then all common substrings of that length in lexicographic order without duplicates.
The full statement and the report are in [report/report.pdf](report/report.pdf) (LaTeX sources in [report/](report/), rebuilt with `make report LAB=lab5`).

## Build and run

```bash
make run LAB=lab5 < lab5/tests/data/01.in
make run LAB=lab5 APP=solution < lab5/tests/data/01.in
python3 lab5/benchmark/generator.py 100000
make run LAB=lab5 APP=benchmark PRESET=release < lab5/benchmark/tests.txt
```

## Example

Input:

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

## Notes

- The generalized suffix tree of `first#second$` is built with Ukkonen's algorithm in $O(n \log \sigma)$ time, where $n$ is the total length and $\sigma \le 258$ is the alphabet size (256 bytes and two separators).
- The tree is traversed iteratively with an explicit stack, so long inputs such as `aaaa...` do not overflow the call stack.
- A missing second line is treated as an empty string and the answer is `0`.
- `solution.cpp` is the single-file version for the checker; the e2e tests run the same cases against `lab5_main` and `lab5_solution`.
