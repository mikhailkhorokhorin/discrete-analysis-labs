import random
import string
import sys
from pathlib import Path

OUTPUT_PATH = Path(__file__).resolve().parent / "tests.txt"
DEFAULT_ALPHABET_SIZE = 4
DEFAULT_SEED = 42


def main() -> None:
    if len(sys.argv) < 2:
        print("Usage: python3 generator.py <len1> [len2] [alphabet_size] [seed]")
        sys.exit(1)

    first_length = int(sys.argv[1])
    second_length = int(sys.argv[2]) if len(sys.argv) > 2 else first_length
    alphabet_size = int(sys.argv[3]) if len(sys.argv) > 3 else DEFAULT_ALPHABET_SIZE
    seed = int(sys.argv[4]) if len(sys.argv) > 4 else DEFAULT_SEED

    if first_length < 0 or second_length < 0 or not 1 <= alphabet_size <= 26:
        print("len1 and len2 must be non-negative; alphabet_size must be in [1, 26]")
        sys.exit(1)

    rng = random.Random(seed)
    alphabet = string.ascii_lowercase[:alphabet_size]
    first = "".join(rng.choices(alphabet, k=first_length))
    second = "".join(rng.choices(alphabet, k=second_length))
    OUTPUT_PATH.write_text(f"{first}\n{second}\n", encoding="utf-8")


if __name__ == "__main__":
    main()
