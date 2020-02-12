all : program.out

program.out : main.o HashTable.o
	gcc --coverage -o $@ $^
CFLAGS = -O0 -MD --coverage
-include *.d

clean:
	rm *.o *.d *.out *.gc*
