import sys
from pathlib import Path

OUTPUT_PATH = Path(__file__).resolve().parent / "tests.txt"
WORDS_PER_LINE = 20
DEFAULT_PATTERN_LENGTH = 20
REPEATED_TOKEN = 1
LAST_TOKEN = 2


def format_lines(pattern: list[int], text: list[int]) -> str:
    lines = [" ".join(str(token) for token in pattern)]
    for start in range(0, len(text), WORDS_PER_LINE):
        lines.append(" ".join(str(token) for token in text[start : start + WORDS_PER_LINE]))
    return "".join(f"{line}\n" for line in lines)


def main() -> None:
    if len(sys.argv) < 2:
        print("Usage: python3 generator_worst.py <text_len> [pattern_len]")
        sys.exit(1)

    text_length = int(sys.argv[1])
    pattern_length = int(sys.argv[2]) if len(sys.argv) > 2 else DEFAULT_PATTERN_LENGTH

    if text_length < 0 or pattern_length < 1:
        print("text_len must be non-negative; pattern_len must be positive")
        sys.exit(1)

    pattern = [REPEATED_TOKEN] * (pattern_length - 1) + [LAST_TOKEN]
    text = [REPEATED_TOKEN] * text_length
    OUTPUT_PATH.write_text(format_lines(pattern, text), encoding="utf-8")


if __name__ == "__main__":
    main()
