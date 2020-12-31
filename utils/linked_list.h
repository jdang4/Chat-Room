#ifndef LINKEDLIST_H
#define LINKEDLIST_H

#include "node.h"
#include <string>

class Node;

class LinkedList 
{

private:
	Node* head;

public:
	LinkedList();
	~LinkedList();

	void append(struct ClientInfo& info);
	void print();
	bool usernameExists(int id);
	void deleteUser(int id);
	Node* getHead();
	int size();
};

#endif