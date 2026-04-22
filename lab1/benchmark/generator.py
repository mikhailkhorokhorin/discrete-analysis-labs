import sys
import random
import string

def main():
    if len(sys.argv) < 2:
        print("Usage: python generator.py [num_tests] [year_from] [year_to]")
        sys.exit(1)

    num_tests = int(sys.argv[1])
    year_from = int(sys.argv[2]) if len(sys.argv) > 2 else 1900
    year_to = int(sys.argv[3]) if len(sys.argv) > 3 else 2100

    with open("tests.txt", "w", encoding="utf-8") as f:
        for _ in range(num_tests):
            day = random.randint(1, 28)
            month = random.randint(1, 12)
            year = random.randint(year_from, year_to)

            value_len = random.randint(5, 100)
            value = ''.join(random.choices(string.ascii_letters + string.digits, k=value_len))

            f.write(f"{day:02d}.{month:02d}.{year}\t{value}\n")


if __name__ == "__main__":
    main()
