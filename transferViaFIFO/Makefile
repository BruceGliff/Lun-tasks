all : reader.out writer.out

reader.out : reader.o
	gcc  -o $@ $^
writer.out : writer.o
	gcc  -o $@ $^
CFLAGS = -O0 -pthread

clean:
	rm *.o *.out
