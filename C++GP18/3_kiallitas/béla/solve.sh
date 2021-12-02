#!/bin/zsh

rm -f times
for ((k=1; k<=100; ++k))
do
	i=`printf '%02d' $k`;
	echo $i
	rm -f .dt
	/usr/bin/timeout -s9 5.0s /usr/bin/time -f 't=%U' ./a.out < "in/$i.in" > "out/$i.out" 2> .dt
	cat .dt
	echo $i `cat .dt` >> times
	#/usr/bin/timeout -s9 1.1s /usr/bin/time -f 't=%U' ./a.out < "$n" > "$op" 2> "$tp"
	#time ./a.out < "in/$i.in" > "out/$i.out"

	if [ ! -s "out/$i.out" ]; then rm "out/$i.out"; fi
	#python2 ../checker.py in/78.in out/78.out
done
rm -f .dt
