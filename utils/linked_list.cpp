#include "linked_list.h"
#include <iostream>


LinkedList::LinkedList()
{
	head = NULL;
}

LinkedList::~LinkedList() {}

Node* LinkedList::getHead()
{
    return head;
}

void LinkedList::append(ClientInfo& info)
{
	Node* newNode = new Node(info);
	Node* nodeptr;

	if (head == NULL)
	{
		head = newNode;
	}

	else
	{
		nodeptr = head;

		while (nodeptr->next != NULL)
		{
			nodeptr = nodeptr->next;
		}

		nodeptr->next = newNode;	
	}
}

void LinkedList::deleteUser(int id)
{
	if (head == NULL) { return; }

	// if the node to delete is at the head
	if (head->equals(id))
	{
		if (head->next != NULL)
		{
			head->clientInfo = head->next->clientInfo;
			head->next = head->next->next;
		}

		else { head = NULL; }

		return;
	}

	Node* prev = head;

	// keep looping if not end of list or not the node to delete
	while (prev->next != NULL && !prev->next->equals(id))
	{
		prev = prev->next;
	}

	// username not found
	if (prev->next == NULL) { return; }

	// prev->next is the node to delete
	prev->next = prev->next->next;

}


bool LinkedList::usernameExists(int id)
{
	Node* nodeptr = head;

	while (nodeptr != NULL)
	{
		if (nodeptr->equals(id))
		{
			return true;
		}

		nodeptr = nodeptr->next;
	}

	return false;
}


void LinkedList::print()
{
    Node* nodeptr;

    nodeptr = head;

    while (nodeptr->next != NULL) 
    {
    	std::cout << nodeptr->clientInfo->username << " -> ";
    	nodeptr = nodeptr->next;
    }

    std::cout << nodeptr->clientInfo->username << std::endl;
}


int LinkedList::size()
{
	Node* nodeptr = head;
	int size = 0;

	while (nodeptr != NULL)
	{
		size++;
		nodeptr = nodeptr->next;
	}

	return size;
}
