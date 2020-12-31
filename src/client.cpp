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

using namespace std;

int clientSocket;
bool exitFlag = false;
thread tSend, tRecv;

void sendMessage(int socket);
void recieveMessage(int socket);
void exitGracefully(int signal);
void removeText(int count);

int main()
{
	/*
	AF_INET: IPv4 Protocol
	SOCK_STREAM: TCP
	0: Internet Protocol (IP) 
	*/
	clientSocket = socket(AF_INET, SOCK_STREAM, 0);

	if (clientSocket == -1)
	{
		perror("Socket Error: ");
		exit(EXIT_FAILURE);
	}

	struct sockaddr_in client;
	client.sin_family = AF_INET;
	client.sin_port = htons(10000); 
	client.sin_addr.s_addr = INADDR_ANY;
	bzero(&client.sin_zero,0);
	
	// check if error occurred when connecting the socket
	if( (connect(clientSocket,(struct sockaddr *)&client, sizeof(struct sockaddr_in))) ==-1 )
	{
		perror("Connect Error: ");
		exit(EXIT_FAILURE);
	}
	
	signal(SIGINT, exitGracefully);

	char name[MAX_LEN];

	cout << "\nEnter in a username: ";
	
	cin.getline(name, MAX_LEN);

	send(clientSocket, name, sizeof(name), 0);

	cout << "\n\t  ====== Welcome to the Chat Room! ======   " << endl;

	thread t_s(sendMessage, clientSocket);
	thread t_r(recieveMessage, clientSocket);

	tSend = move(t_s);
	tRecv = move(t_r);

	if (tSend.joinable()) { tSend.join(); }

	if (tRecv.joinable()) { tRecv.join(); }	

	return 0;
}

/* handle situation when user hits ctrl+^C to exit program */
void exitGracefully(int signal)
{
	char msg[MAX_LEN] = "#exit";

	send(clientSocket, msg, sizeof(msg), 0); // send to server

	exitFlag = true;

	tSend.detach();
	tRecv.detach();

	close(clientSocket);
	exit(signal);
}

/* deletes the text from the terminal */
void removeText(int count)
{
	char backSpace = 8;

	for (int i = 0; i < count; i++)
	{
		cout << backSpace;
	}
}

/* sends a message to the client's socket */
void sendMessage(int socket)
{
	while (1)
	{
		char msg[MAX_LEN];

		cout << "You: ";
		cin.getline(msg, MAX_LEN);

		send(socket, msg, sizeof(msg), 0);

		// check if client wants to leave chat room
		if (strncmp(msg, "#exit", sizeof("#exit")) == 0)
		{
			exitFlag = true;

			tRecv.detach();

			close(socket);

			return;
		}
	}
}

/* recieves the message from the client's socket */
void recieveMessage(int socket)
{
	while (1)
	{
		if (exitFlag) { return; }

		char name[MAX_LEN], msg[MAX_LEN];

		int recievedBytes = recv(socket, name, sizeof(name), 0);

		// no messages were sent to the socket 
		if (recievedBytes <= 0) { continue; }

		recv(socket, msg, sizeof(msg), 0);

		removeText(6);

		if (strncmp(name, "#NULL", sizeof("#NULL")) == 0)
		{
			cout << msg << endl;
		}

		else
		{
			string otherClientMsg = string(name) + ": " + string(msg);
			cout << otherClientMsg << endl;
		}

		cout << "You: ";

		fflush(stdout);
	}
}

