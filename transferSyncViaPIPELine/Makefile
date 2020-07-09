all : program.out

program.out : main.o
	gcc -o $@ $^
CFLAGS = -O0

clean:
	rm *.o *.out
