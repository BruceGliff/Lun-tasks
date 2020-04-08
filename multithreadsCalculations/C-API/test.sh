#!/bin/bash

for ((a = 1; a < 13; a++))
do
	echo "Threads $a"
	time ./program.out $a
	echo -e "\n\n"
done
