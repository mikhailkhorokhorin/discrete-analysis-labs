import sys
import random
import string


def main():
    if len(sys.argv) < 2:
        print("Usage: python generator.py [len1] [len2] [alphabet_size] [seed]")
        sys.exit(1)

    len1 = int(sys.argv[1])
    len2 = int(sys.argv[2]) if len(sys.argv) > 2 else len1
    alphabet_size = int(sys.argv[3]) if len(sys.argv) > 3 else 4
    seed = int(sys.argv[4]) if len(sys.argv) > 4 else 42

    if len1 < 0 or len2 < 0 or alphabet_size < 1 or alphabet_size > len(string.ascii_lowercase):
        print("len1 and len2 must be non-negative; alphabet_size must be in [1, 26]")
        sys.exit(1)

    random.seed(seed)
    alphabet = string.ascii_lowercase[:alphabet_size]

    s1 = ''.join(random.choices(alphabet, k=len1))
    s2 = ''.join(random.choices(alphabet, k=len2))

    with open("tests.txt", "w", encoding="utf-8") as f:
        f.write(s1 + "\n")
        f.write(s2 + "\n")


if __name__ == "__main__":
    main()
