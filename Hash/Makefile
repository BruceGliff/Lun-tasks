all : program.out

program.out : main.o HashTable.o
	gcc -o $@ $^

CFLAGS = -O0 -MD

-include *.d

clean:
	rm *.o *.d program.out
