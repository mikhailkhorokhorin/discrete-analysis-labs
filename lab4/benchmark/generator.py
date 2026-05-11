import sys
import random

MAX_UINT32 = 2 ** 32 - 1


def main():
    if len(sys.argv) < 2:
        print("Usage: python generator.py [text_len] [pattern_len] [seed] [alphabet_size]")
        sys.exit(1)

    text_len = int(sys.argv[1])
    pattern_len = int(sys.argv[2]) if len(sys.argv) > 2 else 4
    seed = int(sys.argv[3]) if len(sys.argv) > 3 else 42
    alphabet_size = int(sys.argv[4]) if len(sys.argv) > 4 else 10

    if text_len < 0 or pattern_len < 1 or alphabet_size < 1:
        print("text_len must be non-negative; pattern_len and alphabet_size must be positive")
        sys.exit(1)

    random.seed(seed)

    alphabet = [random.randint(0, MAX_UINT32) for _ in range(alphabet_size)]

    pattern = [random.choice(alphabet) for _ in range(pattern_len)]
    text = [random.choice(alphabet) for _ in range(text_len)]

    words_per_line = 20

    with open("tests.txt", "w", encoding="utf-8") as f:
        f.write(" ".join(str(x) for x in pattern) + "\n")

        i = 0
        while i < len(text):
            chunk = text[i:i + words_per_line]
            f.write(" ".join(str(x) for x in chunk) + "\n")
            i += words_per_line


if __name__ == "__main__":
    main()
