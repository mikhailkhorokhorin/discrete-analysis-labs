#!/usr/bin/env bash
set -euo pipefail

dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
benchmark="$1"

for length in 10000 100000 500000 1000000 5000000; do
    python3 "$dir/generator.py" "$length" 8 42 2
    echo "N=$length random"
    "$benchmark" < "$dir/tests.txt"
done

for length in 10000 100000 500000 1000000 5000000; do
    python3 "$dir/generator_worst.py" "$length"
    echo "N=$length worst"
    "$benchmark" < "$dir/tests.txt"
done
