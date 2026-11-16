#!/usr/bin/env bash
set -euo pipefail

dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
benchmark="$1"

for length in 1000 2000 4000 10000 50000 100000; do
    python3 "$dir/generator.py" "$length" "$length" 4 42
    echo "N=$length"
    "$benchmark" < "$dir/tests.txt"
done
