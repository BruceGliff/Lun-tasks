CFLAGS = -O3 -Wall -pthread -lm

all : program.out

program.out : main.o cpu_optimization.o task.o
	gcc -o $@ $^ $(CFLAGS)

clean:
	rm *.o *.out