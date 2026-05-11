import random
import string
import sys


def main():
    if len(sys.argv) < 2:
        print("Usage: python3 generator.py [num_words] [max_word_len] [seed]")
        sys.exit(1)

    num_words = int(sys.argv[1])
    max_word_len = int(sys.argv[2]) if len(sys.argv) > 2 else 16
    seed = int(sys.argv[3]) if len(sys.argv) > 3 else 42

    random.seed(seed)
    alphabet = string.ascii_lowercase
    words = []
    seen = set()

    while len(words) < num_words:
        length = random.randint(1, max_word_len)
        word = "".join(random.choices(alphabet, k=length))
        if word not in seen:
            seen.add(word)
            words.append(word)

    with open("tests.txt", "w", encoding="utf-8") as file:
        for index, word in enumerate(words):
            file.write(f"+ {word} {index + 1}\n")
        for word in words:
            file.write(f"{word}\n")
        for word in words:
            file.write(f"- {word}\n")


if __name__ == "__main__":
    main()
