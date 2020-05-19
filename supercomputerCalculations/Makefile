all:
	g++ -std=c++98 -pthread worker.cc -o worker
	g++ -std=c++98 -pthread SWorkerReciever.cc serverMain.cc Task.cc STaskSender.cc -o server

clear:
	rm worker server
