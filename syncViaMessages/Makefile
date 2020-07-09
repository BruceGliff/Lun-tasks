all : program.out

program.out : message.o
	gcc -o $@ $^
CFLAGS = -O0

clean:
	rm *.o *.out
