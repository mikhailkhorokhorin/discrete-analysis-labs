# Lab 2. PATRICIA Trie Dictionary

## Task

Implement a dictionary on top of a PATRICIA trie (the variant of this lab).
Keys are case-insensitive words of English letters up to 256 characters long, values are numbers from $0$ to $2^{64} - 1$.
The program processes the commands `+ word 34` (add), `- word` (remove), `word` (find), `! Save path` and `! Load path` (binary dump) and prints `OK`, `Exist`, `NoSuchWord`, `OK: 34` or `ERROR: ...` for each of them.
The full statement and the report are in [report/report.pdf](report/report.pdf) (LaTeX sources in [report/](report/), rebuilt with `make report LAB=lab2`).

## Build and run

```bash
make run LAB=lab2 < lab2/tests/data/01.in
make run LAB=lab2 APP=solution < lab2/tests/data/01.in
python3 lab2/benchmark/generator.py 100000
make run LAB=lab2 APP=benchmark PRESET=release < lab2/benchmark/tests.txt
```

## Example

Input:

```text
+ a 1
+ A 2
+ word 18446744073709551615
word
A
- A
a
```

Output:

```text
OK
Exist
OK
OK: 18446744073709551615
OK: 1
OK
NoSuchWord
```

## Notes

- Invalid input is answered with `ERROR: invalid command`, `ERROR: unknown command`, `ERROR: invalid key` or `ERROR: invalid value`.
- The dump starts with the magic `PATRICIA`, a format version and the number of entries; a broken file gives `ERROR: invalid file format` and keeps the current dictionary.
- `solution.cpp` is the single-file version for the checker; the e2e tests run the same cases against `lab2_main` and `lab2_solution`.
