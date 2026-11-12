#!/bin/bash
# Usage: ./repro.sh [file.c] [tries]   -- exit 0 if nvcpfe segfaults at least once.
# Also usable as a cvise interestingness test:  cvise ./repro.sh ILL_IN6.c
f=${1:-ILL_IN6.c}; n=${2:-20}; hits=0
for i in $(seq "$n"); do
  nvc -c -o /dev/null "$f" -fast -Minfo=accel -acc=gpu -gpu=cc80,mem:managed -DOPENACC 2>&1 \
    | grep -q 'nvcpfe TERMINATED by signal 11' && hits=$((hits+1))
done
echo "$f: $hits/$n nvcpfe segfaults"
[ "$hits" -gt 0 ]
