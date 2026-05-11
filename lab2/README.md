# 3. 3 PATRICIA

|                  |                                      |
|------------------|--------------------------------------|
| Time limit       | 15 seconds                           |
| Memory limit     | 512 MB                               |
| Input            | standard input or `input.txt`        |
| Output           | standard output or `output.txt`      |

You need to create a program library implementing the specified data structure, on the basis of which to develop a dictionary program. In the dictionary, each key, which is a case-insensitive sequence of letters of the English alphabet of no more than 256 characters, is associated with a number from 0 to 2⁶⁴ − 1. Different words can be assigned the same number.

The program must process lines of the input file until its end. Each line can have the following format:

**`+ word 34`** — add the word "word" with number 34 to the dictionary. The program must output "OK" if the operation was successful, "Exist" if the word is already in the dictionary.

**`- word`** — remove the word "word" from the dictionary. The program must output "OK" if the word existed and was removed, "NoSuchWord" if the word was not found in the dictionary.

**`word`** — find the word "word" in the dictionary. The program must output "OK: 34" if the word was found (the number following "OK:" is the number assigned to the word when it was added). If the word was not found in the dictionary, output "NoSuchWord".

**`! Save /path/to/file`** — save the dictionary to a file in binary compact representation, at the path specified by the command parameter. If successful, the program must output "OK"; if the operation fails, output a description of the error.

**`! Load /path/to/file`** — load the dictionary from a file. It is assumed that the file was previously prepared using the Save command. If successful, the program must output "OK" and the loaded dictionary replaces the current one; if unsuccessful, diagnostics must be output and the working dictionary must remain unchanged. In addition to system errors, the program must correctly handle cases where the format of the specified file does not match the dictionary data representation in the external file.

For all operations, if a system error occurs (out of memory, no write permissions, etc.), the program must output a line starting with "ERROR:" and describing the error in English.

The difference between variants lies only in the data structures used: **PATRICIA**.

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
