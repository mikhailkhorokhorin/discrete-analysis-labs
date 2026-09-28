import random
import string
import sys
from pathlib import Path

OUTPUT_PATH = Path(__file__).resolve().parent / "tests.txt"
VALUE_LENGTH = 64
DEFAULT_YEAR_FROM = 1900
DEFAULT_YEAR_TO = 2100
DEFAULT_SEED = 42


def generate(count: int, year_from: int, year_to: int, rng: random.Random) -> list[str]:
    alphabet = string.ascii_letters + string.digits
    lines: list[str] = []
    for _ in range(count):
        day = rng.randint(1, 28)
        month = rng.randint(1, 12)
        year = rng.randint(year_from, year_to)
        value = "".join(rng.choices(alphabet, k=rng.randint(1, VALUE_LENGTH)))
        lines.append(f"{day:02d}.{month:02d}.{year}\t{value}")
    return lines


def main() -> None:
    if len(sys.argv) < 2:
        print("Usage: python3 generator.py <count> [year_from] [year_to] [seed]")
        sys.exit(1)

    count = int(sys.argv[1])
    year_from = int(sys.argv[2]) if len(sys.argv) > 2 else DEFAULT_YEAR_FROM
    year_to = int(sys.argv[3]) if len(sys.argv) > 3 else DEFAULT_YEAR_TO
    seed = int(sys.argv[4]) if len(sys.argv) > 4 else DEFAULT_SEED

    lines = generate(count, year_from, year_to, random.Random(seed))
    OUTPUT_PATH.write_text("".join(f"{line}\n" for line in lines), encoding="utf-8")


if __name__ == "__main__":
    main()
