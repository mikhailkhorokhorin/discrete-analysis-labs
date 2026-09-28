import random
import string
import sys
from pathlib import Path

OUTPUT_PATH = Path(__file__).resolve().parent / "tests.txt"
DEFAULT_MAX_WORD_LENGTH = 16
DEFAULT_SEED = 42
MAX_VALUE = 2**64 - 1


def generate_words(count: int, max_length: int, rng: random.Random) -> list[str]:
    words: list[str] = []
    seen: set[str] = set()
    while len(words) < count:
        word = "".join(rng.choices(string.ascii_lowercase, k=rng.randint(1, max_length)))
        if word not in seen:
            seen.add(word)
            words.append(word)
    return words


def build_commands(words: list[str], rng: random.Random) -> list[str]:
    commands = [f"+ {word} {rng.randint(0, MAX_VALUE)}" for word in words]
    commands.extend(words)
    commands.extend(f"- {word}" for word in words)
    return commands


def main() -> None:
    if len(sys.argv) < 2:
        print("Usage: python3 generator.py <num_words> [max_word_len] [seed]")
        sys.exit(1)

    count = int(sys.argv[1])
    max_length = int(sys.argv[2]) if len(sys.argv) > 2 else DEFAULT_MAX_WORD_LENGTH
    seed = int(sys.argv[3]) if len(sys.argv) > 3 else DEFAULT_SEED

    rng = random.Random(seed)
    commands = build_commands(generate_words(count, max_length, rng), rng)
    OUTPUT_PATH.write_text("".join(f"{command}\n" for command in commands), encoding="utf-8")


if __name__ == "__main__":
    main()
