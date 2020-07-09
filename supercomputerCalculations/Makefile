all:
	g++ -std=c++98 -pthread worker.cc -o worker.out
	g++ -std=c++98 -pthread SWorkerReciever.cc serverMain.cc Task.cc STaskSender.cc -o server.out

clear:
	rm *.out
