#include "utils/node.h"
#include "utils/linked_list.h"
#include <iostream>
#include <string>

using namespace std;


int main()
{
	LinkedList* l1 = new LinkedList();

	ClientInfo c1 = { 1, "1", 0 };	
	ClientInfo c2 = { 2, "2", 0 };
	ClientInfo c3 = { 3, "3", 0 };
	ClientInfo c4 = { 4, "4", 0 };
	ClientInfo c5 = { 5, "5", 0 };
    
    l1->append(c1);
    l1->append(c2);
    l1->append(c3);
    l1->append(c4);
    l1->append(c5);

    l1->print();

    cout << l1->size() << endl;

    string result = l1->usernameExists(3) ? "true" : "false";

    cout << result << endl;

    l1->deleteUser(5);

    l1->print();

    cout << l1->size() << endl;


}
