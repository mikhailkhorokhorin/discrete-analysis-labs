# Lab 3. Software Quality Research

## Task

Investigate the execution speed and memory consumption of the PATRICIA dictionary from [lab 2](../lab2/) and fix the defects that were found.
The minimum toolset is `gprof` and a memory checker; this work uses `gprof`, Valgrind Memcheck and Valgrind Massif together with a test generator and a benchmark against `std::map`.
The result is a report with a work diary, the defects found, the fixes and the conclusions.
The full statement and the report are in [report/report.pdf](report/report.pdf) (LaTeX sources in [report/](report/), rebuilt with `make report LAB=lab3`).

## Build and run

The lab has no code of its own: `lab3_main` and `lab3_profile` (built with `-pg`) are compiled from the lab 2 sources.

```bash
python3 lab3/benchmark/generator.py 100000 16 42
make run LAB=lab3 PRESET=release < lab3/benchmark/tests.txt > /dev/null
make run LAB=lab3 APP=profile PRESET=release < lab3/benchmark/tests.txt
gprof build/release/lab3/lab3_profile gmon.out > gprof.txt
valgrind --leak-check=full build/release/lab3/lab3_main < lab3/benchmark/tests.txt > /dev/null
valgrind --tool=massif --massif-out-file=massif.out build/release/lab3/lab3_main < lab3/benchmark/tests.txt > /dev/null
make research
```

`make research` runs all of these steps and stores `gprof.txt`, `memcheck.txt` and `massif.txt` in `build/artifacts/lab3/`.

## Example

`lab3_profile` prints the benchmark results for PATRICIA and `std::map`:

```text
N=100000
PATRICIA  Insert=...us Find=...us Remove=...us
std::map  Insert=...us Find=...us Remove=...us
```

## Notes

- The dictionary itself is tested in lab 2; the defects found here are fixed in the lab 2 sources and covered by its tests.
