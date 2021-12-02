#!/bin/sh
mkdir -p out
for f in in/*.txt; do
	echo $f
	time ./kapuk <$f >out/$(basename $f .txt).out
	../checker/checker $f out/$(basename $f .txt).out >out/$(basename $f .txt).score
done
