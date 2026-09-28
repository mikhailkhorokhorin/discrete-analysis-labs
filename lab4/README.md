# Lab 4. Knuth-Morris-Pratt Search

## Task

Find all occurrences of one pattern in a text with the Knuth-Morris-Pratt algorithm.
Variant: the alphabet consists of numbers from $0$ to $2^{32} - 1$, the pattern is on the first line and the text follows on the next lines.
For every occurrence print the line number and the word number where it starts, both counted from 1 and without the pattern line.
The full statement and the report are in [report/report.pdf](report/report.pdf) (LaTeX sources in [report/](report/), rebuilt with `make report LAB=lab4`).

## Build and run

```bash
make run LAB=lab4 < lab4/tests/data/01.in
make run LAB=lab4 APP=solution < lab4/tests/data/01.in
python3 lab4/benchmark/generator.py 1000000
make run LAB=lab4 APP=benchmark PRESET=release < lab4/benchmark/tests.txt
```

## Example

Input:

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

## Notes

- The text is processed as a stream by `KmpMatcher`, which keeps only the prefix function and the positions of the last `m` words, so memory does not depend on the text length.
- A token that is not a number in the allowed range stops the program with `ERROR: line N: invalid token '...'` on stderr and exit code 1.
- `solution.cpp` is the single-file version for the checker; the e2e tests run the same cases against `lab4_main` and `lab4_solution`.
