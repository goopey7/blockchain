#include <iostream>
#include "Server.h"
#include "Client.h"


int main()
{
	std::string input;
	do
	{
		CLEAR_SCREEN
		std::cout << "1: Server\n";
		std::cout << "2: Client\n";
		std::cout << "E: Exit\n";
		ReadAndWrite::getInputAsString(input);
	}
	while (input!="E"&&input!="e"&&input!="1"&&input!="2");
	CLEAR_SCREEN
	if(input=="1")
	{
		Server server;
		server.start();
	}
	else if(input=="2")
	{
		Client client;
		client.openMainMenu();
	}
	return 0;
}
