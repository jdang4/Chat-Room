CFLAGS = -g -std=c++11 -Wall

all: server client

server: server.o
	g++ $(CFLAGS) server.o -lpthread -o server

server.o: src/server.cpp utils/client_info.h
	g++ $(CFLAGS) -c src/server.cpp 

client: client.o
	g++ $(CFLAGS) client.o -lpthread -o client

client.o: src/client.cpp utils/client_info.h
	g++ $(CFLAGS) -c src/client.cpp 


clean:
	rm -f server.o client.o server client