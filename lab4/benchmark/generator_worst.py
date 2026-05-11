import sys


def main():
    if len(sys.argv) < 2:
        print("Usage: python generator_worst.py [text_len] [pattern_len]")
        sys.exit(1)

    text_len = int(sys.argv[1])
    pattern_len = int(sys.argv[2]) if len(sys.argv) > 2 else 20

    if text_len < 0 or pattern_len < 1:
        print("text_len must be non-negative; pattern_len must be positive")
        sys.exit(1)

    a = 1
    b = 2
    words_per_line = 20

    pattern = [a] * (pattern_len - 1) + [b]
    text = [a] * text_len

    with open("tests.txt", "w", encoding="utf-8") as f:
        f.write(" ".join(str(x) for x in pattern) + "\n")
        i = 0
        while i < len(text):
            chunk = text[i:i + words_per_line]
            f.write(" ".join(str(x) for x in chunk) + "\n")
            i += words_per_line


if __name__ == "__main__":
    main()
