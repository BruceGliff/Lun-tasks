all:
	gcc -std=c99 worker.c -o worker
	gcc -std=c99 server.c -o server

new:
	gcc -std=c99 -pthread SWorkerReciever.c serverMain.c -o test
