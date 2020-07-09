all : reader.out writer.out

reader.out : reader.o
	gcc -o $@ $^
writer.out : writer.o
	gcc -o $@ $^
CFLAGS = -O0

clean:
	rm *.o *.out
