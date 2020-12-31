CFLAGS = -g -std=c++11 -Wall

all: server

server: server.o node.o linked_list.o
	g++ $(CFLAGS) server.o node.o linked_list.o -lpthread -o server

server.o: src/server.cpp utils/node.h utils/linked_list.h
	g++ $(CFLAGS) -c src/server.cpp 

node.o: utils/node.cpp utils/node.h utils/client_info.h 
	g++ $(CFLAGS) -c utils/node.cpp 

linked_list.o: utils/linked_list.cpp utils/linked_list.h utils/node.h 
	g++ $(CFLAGS) -c utils/linked_list.cpp 

clean:
	rm -f server.o node.o linked_list.o server