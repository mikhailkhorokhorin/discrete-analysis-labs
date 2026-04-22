# J. 4-1

|                  |                                      |
|------------------|--------------------------------------|
| Time limit       | 3 seconds                            |
| Memory limit     | 300 MB                               |
| Input            | standard input or `input.txt`        |
| Output           | standard output or `output.txt`      |

Implement a program that reads key-value pairs, sorts them by key in ascending order using a linear-time sorting algorithm, and outputs the sorted sequence. The variant is defined by the key type (and the corresponding sorting method) and the value type: **Radix Sort**.

**Key type:** dates in the format `DD.MM.YYYY`, e.g. `1.1.1`, `1.9.2009`, `01.09.2009`, `31.12.2009`.

**Value type:** fixed-length strings of 64 characters. Input strings may be shorter — in that case they are padded with null characters up to 64, which are not printed.

## Input format

Each non-empty line of the input contains a key-value pair: the key as specified above, followed by a tab character, followed by the corresponding value.

## Output format

The output contains the same lines as the input (excluding empty ones), in sorted order.

## Example

**Input:**

```text
1.1.1       n399tann9nnt3ttnaaan9nann93na9t3a3t9999na3aan9antt3tn93aat3naatt
01.02.2008  n399tann9nnt3ttnaaan9nann93na9t3a3t9999na3aan9antt3tn93aat3naat
1.1.1       n399tann9nnt3ttnaaan9nann93na9t3a3t9999na3aan9antt3tn93aat3naa
01.02.2008  n399tann9nnt3ttnaaan9nann93na9t3a3t9999na3aan9antt3tn93aat3na
```

**Output:**

```text
1.1.1       n399tann9nnt3ttnaaan9nann93na9t3a3t9999na3aan9antt3tn93aat3naatt
1.1.1       n399tann9nnt3ttnaaan9nann93na9t3a3t9999na3aan9antt3tn93aat3naa
01.02.2008  n399tann9nnt3ttnaaan9nann93na9t3a3t9999na3aan9antt3tn93aat3naat
01.02.2008  n399tann9nnt3ttnaaan9nann93na9t3a3t9999na3aan9antt3tn93aat3na
```
