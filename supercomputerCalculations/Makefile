all:
	g++ -std=c++98 worker.cc -o worker
	#gcc -std=c99 server.c -o server

new:
	g++ -std=c++98 -pthread SWorkerReciever.cc serverMain.cc Task.cc STaskSender.cc -o test
