#!/bin/sh

ss=0.0
for t in 01 02 03 04 05 06 07 08 09 10; do
	echo $t
	./a.out < in/$t.in > out/$t.out 2> err
	cat err
	fgrep 'err=' err > out/$t.err
	python3 ../custom_checker.py -input_file=in/$t.in -output_file=out/$t.out \
		| head -1 > out/$t.score
	s=`cat out/$t.score`
	sf=`printf '%.6f' $s`
	echo "$t,"`cat out/$t.err`",score=$sf"
	ss=`perl -e "printf(\"%f\",$ss+$s)"`
done
perl -e "printf(\"ss=%.6f\n\",$ss/10)"

