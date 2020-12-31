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

#include "../utils/linked_list.h"

using namespace std;

#define MAX_LEN 200

int id = 0;

mutex print_mtx, clients_mtx;  // used to prevent race condition

LinkedList* clients = new LinkedList();

void setClientName(int id, string name);
int broadcastNum(int num, int senderID);
int broadcastMessage(string message, int senderID);
void updateClient(int clientSocket, int id);
void synchronizedPrint(string printMsg, bool endLineFlag);
void endConnection(int id);


int main()
{
	int serverSocket = socket(AF_INET, SOCK_STREAM, 0);

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

	cout << "\n\t  ====== Welcome to the Chat Room! ======   "<< endl;

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

		ClientInfo new_client = { id, "Anonymous", clientSocket, (move(t)) };

		clients->append(new_client);
	}

	Node* nodeptr = clients->getHead();

	while (nodeptr != NULL)
	{
		if (nodeptr->clientInfo->th.joinable())
		{
			nodeptr->clientInfo->th.join();
		}

		nodeptr = nodeptr->next;
	}

	close(serverSocket);
	return 0;
}

/* sets the client's username to name entered by client */
void setClientName(int id, string name)
{
	Node* nodeptr = clients->getHead();

	while (nodeptr != NULL)
	{
		if (nodeptr->clientInfo->id == id)
		{
			nodeptr->clientInfo->username = name;
			break;
		}

		nodeptr = nodeptr->next;
	}
}

/* broadcasts a number to all clients except the sender client */
int broadcastNum(int num, int senderID)
{
	Node* nodeptr = clients->getHead();

	while (nodeptr != NULL)
	{
		if (nodeptr->clientInfo->id != senderID)
		{
			send(nodeptr->clientInfo->socketNum, &num, sizeof(num), 0);
		}

		nodeptr = nodeptr->next;
	}

	return 0;
}

/* broadcasts a message to all clients except the sender client */
int broadcastMessage(string msg, int senderID)
{
	Node* nodeptr = clients->getHead();

	const char* tmp = msg.c_str();

	while (nodeptr != NULL)
	{
		if (nodeptr->clientInfo->id != senderID)
		{
			send(nodeptr->clientInfo->socketNum, tmp, sizeof(tmp), 0);
		}

		nodeptr = nodeptr->next;
	}

	return 0;
}

/* recovers data from client's socket and makes the proper updates based on the recieved message from the client side */
void updateClient(int clientSocket, int id)
{
	char name[MAX_LEN], msg[MAX_LEN];
	recv(clientSocket, name, sizeof(name), 0);

	setClientName(id, string(name));

	string joined_message = string(name).append(" has joined");
	broadcastMessage("#NULL", id);	
	broadcastNum(id, id);								
	broadcastMessage(joined_message, id);

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
			broadcastNum(id, id);
			broadcastMessage(left_message, id);
			synchronizedPrint(left_message, true);
			endConnection(id);

			return;
		}
		
		broadcastMessage(string(name), id);
		broadcastNum(id, id);
		broadcastMessage(string(msg), id);

		string full_terminal_text = string(name) + ": " + string(msg);
		synchronizedPrint(full_terminal_text, true);
	}	
}

/* synchronization of print messages */
void synchronizedPrint(string printMsg, bool endLineFlag=true)
{
	lock_guard<mutex> guard(print_mtx);

	cout << printMsg;

	if (endLineFlag) { cout << endl; }

}

/* ends the connection for the specified client id */
void endConnection(int id)
{
	Node* nodeptr = clients->getHead();

	while (nodeptr != NULL)
	{
		if (nodeptr->clientInfo->id == id)
		{
			lock_guard<mutex> guard(clients_mtx);
			nodeptr->clientInfo->th.detach();
			close(nodeptr->clientInfo->socketNum);
			clients->deleteUser(id);
			break;
		} 

		nodeptr = nodeptr->next;
	}
}




