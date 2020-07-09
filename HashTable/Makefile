all : program.out

program.out : main.o HashTable.o
	gcc --coverage -o $@ $^
CFLAGS = -O0 -MD -g --coverage -Wno-implicit-function-declaration
-include *.d

clean:
	rm *.o *.d *.out *.gc* *.dump
