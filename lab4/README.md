# B. 1-2

|                  |                                      |
|------------------|--------------------------------------|
| Time limit       | 10 seconds                           |
| Memory limit     | 64 MB                                |
| Input            | standard input or `input.txt`        |
| Output           | standard output or `output.txt`      |

Implement the Knuth-Morris-Pratt pattern matching algorithm for one pattern.

**Alphabet:** numbers in the range from `0` to `2^32 - 1`.

## Input format

The pattern is given on the first line. Then follows the text consisting of numbers, in which the pattern must be found.

There are no limits on line length or on the number of numbers in a line.

## Output format

For each occurrence of the pattern, output two numbers separated by a comma: the line number and the word number in the line where the pattern starts.

Numbering starts from 1. Line numbers are counted from the start of the text, not including the pattern line.

## Example

**Input:**

```text
11 45 11 45 90
0011 45 011 0045 11 45 90    11
45 11 45 90
```

**Output:**

```text
1, 3
1, 8
```
