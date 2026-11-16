# Lab 1. Radix Sort

## Task

Sort key-value pairs by key in ascending order with a linear-time algorithm.
Variant: radix sort, the key is a date `DD.MM.YYYY`, the value is a string of up to 64 characters.
Pairs with equal keys keep their input order.

## Input and output

- Input: one pair per line, the key and the value are separated by a tab. Empty and whitespace-only lines are skipped, a trailing `\r` is ignored.
- Output: the same lines in sorted order.
- Invalid input (a bad date, no tab, a value longer than 64 characters) stops the program with `ERROR: line N: <reason>` on stderr and exit code 1.

## Algorithm

A date is encoded as the number $10000 \cdot year + 100 \cdot month + day$, which keeps the order of dates and fits in `std::uint32_t`.
The keys are sorted by four stable counting-sort passes, one per byte, from the least significant byte to the most significant one.
Each pass moves the pairs into a preallocated buffer, and the buffer and the array are swapped afterwards.

| | Complexity |
| --- | --- |
| Time | $O(n)$: 4 passes of $O(n + 256)$ |
| Memory | $O(n)$: the input array and one buffer of the same size |

## Files

| Path | Content |
| --- | --- |
| `include/vector.hpp` | `Vector<T>`, a dynamic array with the rule of five |
| `include/pair.hpp`, `src/pair.cpp` | `Pair`, date parsing, reading and writing pairs |
| `include/radix_sort.hpp`, `src/radix_sort.cpp` | `countingPass` and `radixSort` |
| `app/main.cpp` | `lab1_main`: reads stdin, sorts, writes stdout |
| `solution.cpp` | `lab1_solution`: the same program in one file for the checker |
| `tests/` | unit tests and e2e tests on `tests/data/NN.in` and `NN.out` for both binaries |
| `benchmark/` | `lab1_benchmark` (radix sort against `std::stable_sort`), `generator.py`, `run.sh` |

## Build and run

```bash
make build LAB=lab1
make run LAB=lab1 < lab1/tests/data/01.in
make run LAB=lab1 APP=solution < lab1/tests/data/01.in
make test LAB=lab1
make bench LAB=lab1
```

`make bench` builds the release preset and runs `benchmark/run.sh`: for $N$ from $10^2$ to $10^6$ it generates `benchmark/tests.txt` with a fixed seed and prints the median of 5 runs for both sorts.
A single input can be generated and measured by hand:

```bash
python3 lab1/benchmark/generator.py 100000
make run LAB=lab1 APP=benchmark PRESET=release < lab1/benchmark/tests.txt
```

## Example

Input (`tests/data/01.in`, the tab is shown as spaces):

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
