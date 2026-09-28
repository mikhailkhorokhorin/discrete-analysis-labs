# Lab 1. Radix Sort

## Task

Read key-value pairs, sort them by key in ascending order with a linear-time algorithm and print them in the sorted order.
Variant: radix sort, keys are dates in the `DD.MM.YYYY` format (for example `1.1.1` or `31.12.2009`), values are strings of up to 64 characters.
Each input line holds a key, a tab and a value; empty lines are ignored and equal keys keep their input order.
The full statement and the report are in [report/report.pdf](report/report.pdf) (LaTeX sources in [report/](report/), rebuilt with `make report LAB=lab1`).

## Build and run

```bash
make run LAB=lab1 < lab1/tests/data/01.in
make run LAB=lab1 APP=solution < lab1/tests/data/01.in
python3 lab1/benchmark/generator.py 100000
make run LAB=lab1 APP=benchmark PRESET=release < lab1/benchmark/tests.txt
```

## Example

Input (the key and the value are separated by a tab, shown here as spaces):

```text
1.1.1    n399tann9nnt3ttnaaan9nann93na9t3a3t9999na3aan9antt3tn93aat3naatt
01.02.2008    n399tann9nnt3ttnaaan9nann93na9t3a3t9999na3aan9antt3tn93aat3naat
1.1.1    n399tann9nnt3ttnaaan9nann93na9t3a3t9999na3aan9antt3tn93aat3naa
01.02.2008    n399tann9nnt3ttnaaan9nann93na9t3a3t9999na3aan9antt3tn93aat3na
```

Output:

```text
1.1.1    n399tann9nnt3ttnaaan9nann93na9t3a3t9999na3aan9antt3tn93aat3naatt
1.1.1    n399tann9nnt3ttnaaan9nann93na9t3a3t9999na3aan9antt3tn93aat3naa
01.02.2008    n399tann9nnt3ttnaaan9nann93na9t3a3t9999na3aan9antt3tn93aat3naat
01.02.2008    n399tann9nnt3ttnaaan9nann93na9t3a3t9999na3aan9antt3tn93aat3na
```

## Notes

- A date is encoded as the number $\text{key} = 10000 \cdot \text{year} + 100 \cdot \text{month} + \text{day} < 2^{27}$, which keeps the order of dates. It is sorted with four stable counting passes over its bytes, so the sort takes $O(n)$ time and $O(n)$ extra memory.
- Lines with an invalid date, without a tab or with a value longer than 64 characters stop the program with `ERROR: line N: ...` on stderr and exit code 1.
- `solution.cpp` is the single-file version for the checker; the e2e tests run the same cases against `lab1_main` and `lab1_solution`.
