# Lab 3. Software Quality Research

## Task

Investigate the execution speed and the memory consumption of the PATRICIA dictionary from [lab 2](../lab2/) and fix the defects that were found.
The minimum toolset is `gprof` and a memory checker; this work uses `gprof`, Valgrind Memcheck and Valgrind Massif together with the lab 2 test generator and benchmark.

## Input and output

The programs take the input of lab 2: `lab3_main` answers the dictionary commands, and `lab3_profile` prints the benchmark timings for PATRICIA and `std::map`.

## Method

| Tool | What it shows |
| --- | --- |
| `gprof` | the share of time and the number of calls for every function of `lab3_profile`, built with `-pg` |
| Valgrind Memcheck | invalid reads and writes, uninitialized values and leaks of `lab3_main` |
| Valgrind Massif | the heap profile and the peak heap usage of `lab3_main` |

The defects found here are fixed in the lab 2 sources and covered by the lab 2 tests.

## Files

The lab has no code of its own, both targets are built from the lab 2 sources.

| Path | Content |
| --- | --- |
| `CMakeLists.txt` | `lab3_main` (`lab2/app/main.cpp` linked with `lab2_core`) and `lab3_profile` (`lab2/benchmark/benchmark.cpp` and `lab2/src/patricia_trie.cpp` with `-pg`) |

## Build and run

```bash
make build LAB=lab3
make run LAB=lab3 < lab2/tests/data/01.in
make research
```

`make research` builds the release preset, generates 100 000 words with `lab2/benchmark/generator.py` (seed 42, words up to 16 letters) and stores the results in `build/artifacts/lab3/`:

| File | Content |
| --- | --- |
| `gprof.txt` | the flat profile and the call graph |
| `memcheck.txt` | the Memcheck log; `make research` fails if Memcheck reports an error |
| `massif.txt` | the Massif heap profile printed by `ms_print` |

The number of words is set with `make research RESEARCH_WORDS=1000000`.
