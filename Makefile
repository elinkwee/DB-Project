#proj makefile
CFLAG=g++ -Wall -std=c++11 -pedantic
all: server client
server: server.cpp partition.h gendata.h expdata.h
	$(CFLAG) server.cpp -o server -lpthread 
client: client.cpp
	$(CFLAG) client.cpp -o client
demo: server client
clean:
	rm -f server client CarData

	