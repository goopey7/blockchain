/*
// Server side C/C++ program to demonstrate Socket programming

#include "Server.h"
#include "Client.h"

int main()
{
	Server* s=new Server;
	s->start();
	return 0;
}
/*#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>

#define PORT 8080
*/

#include <iostream>
#include "Server.h"
#include "Client.h"

int main()
{
	std::cout << "1: Server\n";
	std::cout << "2: Client\n";
	std::string input;
	ReadAndWrite::getInputAsString(input);
	CLEAR_SCREEN
	if(input=="1")
	{
		// Server stuff...
		Server server;
		server.start();
	}
	else if(input=="2")
	{
		// Client stuff...
		Client client;
		client.openMainMenu();
	}
	/*
	Blockchain chain;
	chain.read("blockchain.txt");

	chain.addBlockToChain("How are we?");
	chain.addBlockToChain("Hello there");
	chain.addBlockToChain("hmmmmmmm");
	chain.validateChain();
	for(int i=0;i<chain.size();i++)
	{
		std::cout << "*******************************************************\n";
		std::cout << chain.at(i)->getIndex() << std::endl;
		std::cout << chain.at(i)->getData() << std::endl;
		std::cout << chain.at(i)->getNonce() << std::endl;
		std::cout << chain.at(i)->generateHash() << std::endl;
		std::cout << "*******************************************************\n";
	}
	chain.write("blockchain.txt");
	*/
	return 0;
}
