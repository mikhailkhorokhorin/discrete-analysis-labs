import sys
import random
import string


def main():
    if len(sys.argv) < 2:
        print("Usage: python generator.py [num_words] [max_word_len]")
        sys.exit(1)

    num_words = int(sys.argv[1])
    max_word_len = int(sys.argv[2]) if len(sys.argv) > 2 else 16

    alphabet = string.ascii_lowercase
    words = list()
    seen = set()

    while len(words) < num_words:
        length = random.randint(1, max_word_len)
        word = ''.join(random.choices(alphabet, k=length))
        if word not in seen:
            seen.add(word)
            words.append(word)

    with open("tests.txt", "w", encoding="utf-8") as f:
        for word in words:
            value = random.randint(0, 2**63 - 1)
            f.write(f"+ {word} {value}\n")
        for word in words:
            f.write(f"? {word}\n")
        for word in words:
            f.write(f"- {word}\n")


if __name__ == "__main__":
    main()
