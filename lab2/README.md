# Lab 2. PATRICIA Trie Dictionary

## Task

Build a dictionary on top of a PATRICIA trie.
Keys are case-insensitive words of English letters up to 256 characters long, values are numbers from $0$ to $2^{64} - 1$.
The dictionary can be saved to and loaded from a binary file.

## Input and output

Each input line is one command, and each command prints one line:

| Command | Meaning | Output |
| --- | --- | --- |
| `+ word 34` | add `word` with the value 34 | `OK` or `Exist` |
| `- word` | remove `word` | `OK` or `NoSuchWord` |
| `word` | find `word` | `OK: 34` or `NoSuchWord` |
| `! Save path` | write the dictionary to `path` | `OK` or `ERROR: ...` |
| `! Load path` | replace the dictionary with the contents of `path` | `OK` or `ERROR: ...` |

- Empty and whitespace-only lines are skipped, a trailing `\r` is ignored.
- Invalid input is answered with `ERROR: invalid command`, `ERROR: unknown command`, `ERROR: invalid key` or `ERROR: invalid value`.
- A failed `Save` prints `ERROR: cannot open file for writing` or `ERROR: failed to write file`.
- A failed `Load` prints `ERROR: cannot open file for reading`, `ERROR: invalid file format` or `ERROR: unsupported file version` and keeps the current dictionary.

## Algorithm

A PATRICIA trie is a binary trie that keeps only the bits where keys differ.
Every node stores a key, a value and the index of its critical bit; a header node with bit 0 points to the root.
A search follows the critical bits of the key until it meets a back edge (an edge to a node with a smaller or equal bit index) and compares the key with the node it reached.
Insertion finds the first bit where the new key differs from that node and links a new node at the matching depth.
Removal either unlinks a node with a self loop or moves the key of the node that holds its back edge into it and unlinks that node instead.
Keys are converted to lower case before any operation.

The file starts with the magic `PATRICIA`, a 4-byte format version and an 8-byte number of entries, followed by `length, key, value` for every node in preorder.
`Load` reads into a temporary trie and swaps it in only after the whole file has been validated.

| | Complexity |
| --- | --- |
| Find, insert, remove | $O(L)$ for a key of $L$ characters: at most $8L + 1$ nodes on the path and one key comparison |
| `Save` | $O(S)$, where $S$ is the total length of the keys |
| `Load` | $O(n \cdot L)$: every entry is inserted again |
| Memory | $O(n + S)$: one node per key |

## Files

| Path | Content |
| --- | --- |
| `include/patricia_trie.hpp`, `src/patricia_trie.cpp` | `PatriciaTrie`, key validation and normalization, the binary file format |
| `include/dictionary.hpp`, `src/dictionary.cpp` | `Dictionary`: command parsing and responses, `runDictionary` |
| `app/main.cpp` | `lab2_main`: runs the dictionary on stdin and stdout |
| `solution.cpp` | `lab2_solution`: the same program in one file for the checker |
| `tests/` | unit tests (including random operations against `std::map`) and e2e tests on `tests/data/NN.in` and `NN.out` for both binaries |
| `benchmark/` | `lab2_benchmark` (PATRICIA against `std::map`), `generator.py`, `run.sh` |

## Build and run

```bash
make build LAB=lab2
make run LAB=lab2 < lab2/tests/data/01.in
make run LAB=lab2 APP=solution < lab2/tests/data/01.in
make test LAB=lab2
make bench LAB=lab2
```

`make bench` builds the release preset and runs `benchmark/run.sh`: for $N$ from $10^3$ to $10^6$ unique words it generates $N$ insertions, $N$ lookups and $N$ removals with a fixed seed and prints the median of 5 runs for every operation.
A single input can be generated and measured by hand:

```bash
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
