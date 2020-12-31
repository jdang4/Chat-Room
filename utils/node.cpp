#include "node.h"


// default constructor
Node::Node() {}

Node::Node(ClientInfo& info)
{
	clientInfo = &info;
}

Node::~Node() {}


bool Node::equals(int id)
{
	return this->clientInfo->id == id;
}
