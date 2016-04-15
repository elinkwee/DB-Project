#proj makefile
CFLAG=g++ -Wall -std=c++11 -pedantic
server: server.cpp
	$(CFLAG) -lpthread server.cpp -o server
client: client.cpp
	g++ client.cpp -o client
demo: server client
clean:
	rm -f server client

	