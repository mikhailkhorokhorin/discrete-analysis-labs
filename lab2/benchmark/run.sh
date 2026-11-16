#!/usr/bin/env bash
set -euo pipefail

dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
benchmark="$1"

for count in 1000 10000 100000 500000 1000000; do
    python3 "$dir/generator.py" "$count"
    echo "N=$count"
    "$benchmark" < "$dir/tests.txt"
done
