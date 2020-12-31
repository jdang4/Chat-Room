#include <bits/stdc++.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <string>
#include <arpa/inet.h>
#include <unistd.h>
#include <thread>
#include <mutex>

#include "../utils/client_info.h"

using namespace std;

#define MAX_LEN 200

int id = 0;
int serverSocket;

mutex print_mtx, clients_mtx;  // used to prevent race condition

vector<ClientInfo> clients;

void setClientName(int id, string name);
int broadcastMessage(string message, int senderID);
void updateClient(int clientSocket, int id);
void synchronizedPrint(string printMsg);
void endConnection(int id);


int main()
{
	/*
	AF_INET: IPv4 Protocol
	SOCK_STREAM: TCP
	0: Internet Protocol (IP) 
	*/
	serverSocket = socket(AF_INET, SOCK_STREAM, 0);

	// check if error occured when creating the socket
	if (serverSocket == -1)
	{
		perror("Socket Error: ");
		exit(EXIT_FAILURE);
	}

	// initializing the server info
	struct sockaddr_in server;
	server.sin_family = AF_INET;
	server.sin_port = htons(10000);
	server.sin_addr.s_addr = INADDR_ANY;
	bzero(&server.sin_zero, 0);

	if ( (bind(serverSocket, (struct sockaddr *)&server, sizeof(struct sockaddr_in))) < 0 )
	{
		perror("Bind Error: ");
		exit(EXIT_FAILURE);
	}

	if ( (listen(serverSocket, 8)) < 0 )
	{
		perror("Listen Error: ");
		exit(EXIT_FAILURE);
	}

	struct sockaddr_in clientAddress;

	int clientSocket;

	unsigned int addrlen = sizeof(sockaddr_in);

	cout << "\n\t  ====== Welcome to the Chat Room! ======   " << endl;

	// logic when running the server
	while (1) 
	{
		clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddress, &addrlen);

		if (clientSocket < 0)
		{
			perror("Accept Error: ");
			exit(EXIT_FAILURE);
		}

		id++;

		thread t(updateClient, clientSocket, id);

		lock_guard<mutex> guard(clients_mtx);

		// idea is to add the thread to the struct so I can join it later by iterating through clients
		clients.push_back({ id, "foobar", clientSocket, (move(t)) });
	}
	
	for (unsigned int i = 0; i < clients.size(); i++)
	{
		ClientInfo* currInfo = &clients[i];

		if (currInfo->th.joinable())
		{
			currInfo->th.join();
		}
	}

	close(serverSocket);

	return 0;
}

/* sets the client's username to name entered by client */
void setClientName(int id, string name)
{
	for (unsigned int i = 0; i < clients.size(); i++)
	{
		ClientInfo* currInfo = &clients[i];

		if (currInfo->id == id)
		{
			currInfo->username = name;
			break;
		}
	}
}

/* broadcasts a message to all clients except the sender client */
int broadcastMessage(string msg, int senderID)
{
	char tmpMsg[MAX_LEN];

	strncpy(tmpMsg, msg.c_str(), sizeof(msg));

	for (unsigned int i = 0; i < clients.size(); i++)
	{
		ClientInfo* currInfo = &clients[i];

		if (currInfo->id != senderID)
		{
			send(currInfo->socketNum, tmpMsg, sizeof(tmpMsg), 0);
		}
	}

	return 0;
}

/* recovers data from client's socket and makes the proper updates based on the recieved message from the client side */
void updateClient(int clientSocket, int id)
{
	char name[MAX_LEN], msg[MAX_LEN];

	recv(clientSocket, name, sizeof(name), 0);

	setClientName(id, string(name));

	string joined_message = string(name) + string(" has joined");

	broadcastMessage("#NULL", id);	

	broadcastMessage(joined_message, id);

	synchronizedPrint(joined_message);

	while (1)
	{
		int recievedBytes = recv(clientSocket, msg, sizeof(msg), 0);

		if (recievedBytes <= 0)
		{
			return;
		}

		// check for a client exit condition
		if (strncmp(msg, "#exit", sizeof("#exit")) == 0)
		{
			string left_message = string(name).append(" has left");

			broadcastMessage("#NULL", id);

			broadcastMessage(left_message, id);

			synchronizedPrint(left_message);

			endConnection(id);

			return;
		}
		
		broadcastMessage(string(name), id);

		broadcastMessage(string(msg), id);

		string full_terminal_text = string(name) + ": " + string(msg);
		synchronizedPrint(full_terminal_text);
	}	
}

/* synchronization of print messages */
void synchronizedPrint(string printMsg)
{
	lock_guard<mutex> guard(print_mtx);

	cout << printMsg << endl;
}

/* ends the connection for the specified client id */
void endConnection(int id)
{
	for (unsigned int i = 0; clients.size(); i++)
	{
		ClientInfo* currInfo = &clients[i];

		if (currInfo->id == id)
		{
			lock_guard<mutex> guard(clients_mtx);

			currInfo->th.detach();

			clients.erase(clients.begin()+i);

			close(currInfo->socketNum);

			break;
		}
	}
}




