#!/bin/bash
# Launch program several times with different number of threads
for ((a = 1; a < 13; a++))
do
	echo "Threads $a"
	time ./program.out $a
	echo -e "\n\n"
done
