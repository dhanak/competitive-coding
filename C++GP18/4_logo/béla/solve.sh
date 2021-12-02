#!/bin/sh

rm -f a.out
gcc wat.c 2>/dev/null
./a.out > o

echo output
cat o
echo

echo
echo diff
diff logo o
