#include <thread>
#include <string>

struct ClientInfo
{
	int id;
	std::string username;
	int socketNum;
	std::thread th;
};