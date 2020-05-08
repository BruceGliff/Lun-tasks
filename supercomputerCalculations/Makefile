all:
	g++ -std=c++98 worker.cc -o worker
	g++ -std=c++98 -pthread SWorkerReciever.cc serverMain.cc Task.cc STaskSender.cc -o test

