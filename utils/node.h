#ifndef NODE_H
#define NODE_H

#include <string>
#include "client_info.h"

class Node
{

public:
	Node();
	Node(struct ClientInfo& info);
	~Node();

	bool equals(int id);

	// attributes
	Node* next;
	ClientInfo* clientInfo;
};

#endif