import random
import sys
from pathlib import Path

OUTPUT_PATH = Path(__file__).resolve().parent / "tests.txt"
MAX_TOKEN = 2**32 - 1
WORDS_PER_LINE = 20
DEFAULT_PATTERN_LENGTH = 4
DEFAULT_SEED = 42
DEFAULT_ALPHABET_SIZE = 10


def format_lines(pattern: list[int], text: list[int]) -> str:
    lines = [" ".join(str(token) for token in pattern)]
    for start in range(0, len(text), WORDS_PER_LINE):
        lines.append(" ".join(str(token) for token in text[start : start + WORDS_PER_LINE]))
    return "".join(f"{line}\n" for line in lines)


def main() -> None:
    if len(sys.argv) < 2:
        print("Usage: python3 generator.py <text_len> [pattern_len] [seed] [alphabet_size]")
        sys.exit(1)

    text_length = int(sys.argv[1])
    pattern_length = int(sys.argv[2]) if len(sys.argv) > 2 else DEFAULT_PATTERN_LENGTH
    seed = int(sys.argv[3]) if len(sys.argv) > 3 else DEFAULT_SEED
    alphabet_size = int(sys.argv[4]) if len(sys.argv) > 4 else DEFAULT_ALPHABET_SIZE

    if text_length < 0 or pattern_length < 1 or alphabet_size < 1:
        print("text_len must be non-negative; pattern_len and alphabet_size must be positive")
        sys.exit(1)

    rng = random.Random(seed)
    alphabet = [rng.randint(0, MAX_TOKEN) for _ in range(alphabet_size)]
    pattern = [rng.choice(alphabet) for _ in range(pattern_length)]
    text = [rng.choice(alphabet) for _ in range(text_length)]
    OUTPUT_PATH.write_text(format_lines(pattern, text), encoding="utf-8")


if __name__ == "__main__":
    main()
