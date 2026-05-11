# 2-3. PATRICIA and Quality Research

|                  |                                      |
|------------------|--------------------------------------|
| Time limit       | 15 seconds                           |
| Memory limit     | 512 MB                               |
| Input            | standard input or `input.txt`        |
| Output           | standard output or `output.txt`      |

## Laboratory work 2

You need to create a program library implementing the specified data structure, on the basis of which to develop a dictionary program. In the dictionary, each key, which is a case-insensitive sequence of letters of the English alphabet of no more than 256 characters, is associated with a number from 0 to 2^64 - 1. Different words can be assigned the same number.

The program must process lines of the input file until its end. Each line can have the following format:

**`+ word 34`** - add the word `word` with number 34 to the dictionary. The program must output `OK` if the operation was successful, `Exist` if the word is already in the dictionary.

**`- word`** - remove the word `word` from the dictionary. The program must output `OK` if the word existed and was removed, `NoSuchWord` if the word was not found in the dictionary.

**`word`** - find the word `word` in the dictionary. The program must output `OK: 34` if the word was found. If the word was not found in the dictionary, output `NoSuchWord`.

**`! Save /path/to/file`** - save the dictionary to a file in binary compact representation, at the path specified by the command parameter. If successful, the program must output `OK`; if the operation fails, output a description of the error.

**`! Load /path/to/file`** - load the dictionary from a file. It is assumed that the file was previously prepared using the `Save` command. If successful, the program must output `OK` and the loaded dictionary replaces the current one; if unsuccessful, diagnostics must be output and the working dictionary must remain unchanged.

For all operations, if a system error occurs, the program must output a line starting with `ERROR:` and describing the error in English.

The difference between variants lies only in the data structures used: **PATRICIA**.

## Laboratory work 3: A. Software Quality Research

For the dictionary implementation from the previous laboratory work, it is necessary to investigate execution speed and RAM consumption. If errors or obvious shortcomings are found, they must be fixed.

The result of the work is a report containing:

- a work diary describing what was done and when, which tools were used, and what results were achieved at each step of the laboratory work;
- conclusions about the shortcomings found;
- comparison of the fixed program with the previous version;
- general conclusions about the completed laboratory work and the experience gained.

The minimum set of tools used must include the `gprof` utility and the `dmalloc` library. However, they may be replaced with any similar or more advanced tools, for example Valgrind or Shark, and additional tools may also be used, for example `gcov`.

### Additional Information

### gprof

The `gprof` utility should be studied independently using the operational documentation of the Unix operating system.

### dmalloc

Below is a short description of how to use it for a small test program.

Suppose a small program is written with two dynamic memory usage errors that cannot be checked at compile time and can only be detected at run time:

```c
#include <stdio.h>
#include <stdlib.h>

#ifdef DMALLOC
#include "dmalloc.h"
#endif

void *foo() {
    char *p = malloc(1023);
    p[1023] = '\0';
    return p;
}

int main() {
    foo();
    return 0;
}
```

The program has two errors:

1. In line 10, the program accesses the 1024th element of the dynamic array `p`, even though only 1023 elements were allocated. When the program is executed, this error will most likely not be detected, because the actual allocated memory size is usually aligned to the page size and therefore will be larger than 1023 elements.
2. The memory allocated in line 9 is never freed.

If the program is compiled and run, most likely no problems will occur either during compilation or during execution:

```bash
gcc -Wall dmalloc-test.c -o dtest
./dtest
```

To use the `dmalloc` library, the special header file `dmalloc.h` must be included in the program. It replaces calls to memory allocation functions with its own versions. The `DMALLOC` macro must also be defined to enable the main functionality of the library. In addition, the `DMALLOC_FUNC_CHECK` macro can be defined to enable additional checking of parameters passed to various functions from the C standard library. Finally, the `dmalloc` library must be linked during compilation.

Thus, compilation can be performed as follows:

```bash
gcc -Wall -DDMALLOC -DDMALLOC_FUNC_CHECK \
    -I/usr/local/include -L/usr/local/lib dmalloc-test.c \
    -o dtest -ldmalloc
```

Before running the program under investigation, the `DMALLOC_OPTIONS` environment variable must be defined. It passes information about how the library should work. The full list of options and their meaning should be found in the library documentation. To correctly set the environment in the `bash` command interpreter, the following command can be used:

```bash
eval `command dmalloc -b -l logfile -i 100 low`
```

In this case, the check report will be written to the file `logfile`.

Now, if the program is run, it will terminate abnormally:

```bash
./dtest
```

```text
debug-malloc library: dumping program, fatal error
   Error: failed OVER picket-fence magic-number check (err 27)
Abort trap: 6 (core dumped)
```

Thus, the `dmalloc` library detected an out-of-bounds access. The abnormal program termination also made it possible to create a core dump, which can be examined using the `gdb` debugger.

If the source program is modified by fixing the out-of-bounds access, for example by replacing index `1023` with `1022`, the program will run normally. The report will look approximately as follows:

```text
1192785155: 1: Dmalloc version '5.5.2' from 'http://dmalloc.com/'
1192785155: 1: flags = 0x4e48503, logfile 'logfile'
1192785155: 1: interval = 100, addr = 0, seen # = 0, limit = 0
1192785155: 1: starting time = 1192785155
1192785155: 1: process pid = 23403
1192785155: 1: Dumping Chunk Statistics:
1192785155: 1: basic-block 8192 bytes, alignment 8 bytes
1192785155: 1: heap address range: 0x16005a000 to 0x160060000, 24576 bytes
1192785155: 1:
1192785155: 1:
1192785155: 1:
1192785155: 1: heap checked 1
1192785155: 1: alloc calls: malloc 1, calloc 0, realloc 0, free 0
1192785155: 1: alloc calls: recalloc 0, memalign 0, valloc 0
1192785155: 1: alloc calls: new 0, delete 0
 user blocks: 1 blocks, 7167 bytes (29%)
admin blocks: 2 blocks, 16384 bytes (66%)
total blocks: 3 blocks, 24576 bytes
1192785155: 1:   current memory in use: 1023 bytes (1 pnts)
1192785155: 1:  total memory allocated: 1023 bytes (1 pnts)
1192785155: 1:  max in use at one time: 1023 bytes (1 pnts)
1192785155: 1: max alloced with 1 call: 1023 bytes
1192785155: 1: max unused memory space: 1025 bytes (50%)
1192785155: 1: top 10 allocations:
1192785155: 1: total-size count in-use-size count source
1192785155: 1: 1023 1 1023 1 dmalloc-test.c:9
1192785155: 1: 1023 1 1023 1 Total of 1
1192785155: 1: Dumping Not-Freed Pointers Changed Since Start:
1192785155: 1: not freed: '0x16005b808|s1' (1023 bytes) from 'dmalloc-test.c:9'
1192785155: 1: total-size count source
1192785155: 1: 1023 1 dmalloc-test.c:9
1192785155: 1: 1023 1 Total of 1
1192785155: 1: ending time = 1192785155, elapsed since start = 0:00:00
```

At the end of the report there is a `Dumping Not-Freed Pointers` section, where it can be seen that one memory block of 1023 bytes, allocated in line 9, was not freed before the program finished. Thus, memory leaks in programs can be tracked.

Before performing the laboratory work, it is strongly recommended to independently study the documentation for the `dmalloc` library. Using additional options not described above will improve the impression of the completed work.

In this laboratory work, `dmalloc` is replaced by more advanced tools: Valgrind Memcheck and Valgrind Massif. The research also uses `gprof`, a test generator, and a benchmark that compares PATRICIA with `std::map`.

## Input format

Each non-empty line of the input contains one of the commands described above.

## Output format

For each input line, the program outputs one response line as specified above.

## Example

**Input:**

```text
+ a 1
+ A 2
+ aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa 18446744073709551615
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
A
- A
a
```

**Output:**

```text
OK
Exist
OK
OK: 18446744073709551615
OK: 1
OK
NoSuchWord
```

## Build

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

## Research Run

```bash
cd lab3/benchmark
python3 generator.py 100000 16 42
../../build/lab3/lab3_profile < tests.txt
gprof ../../build/lab3/lab3_profile gmon.out > gprof.txt
valgrind --leak-check=full ../../build/lab3/lab3_main < tests.txt
valgrind --tool=massif --massif-out-file=massif.out ../../build/lab3/lab3_main < tests.txt
```
