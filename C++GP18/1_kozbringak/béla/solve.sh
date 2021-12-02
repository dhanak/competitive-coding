#!/bin/sh

for i in `fgrep -v \# list`; do
	echo solving $i
	/usr/bin/time -v -o .ot ./a.out < in/$i.in > out/$i.out 2>/dev/null;

	echo judging $i
	rm -f $i.score
	./judge in/$i.in out/$i.out > out/$i.score
	cat out/$i.score

	touch scores
	grep -v ^$i scores > .scores.fospunpa
	mv .scores.fospunpa scores
	echo $i `cat out/$i.score` `fgrep 'User time' .ot|cut -f 2 -d ':'|cut -f 2 -d ' '` >> scores
	rm -f .ot
done
