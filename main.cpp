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
#include "LLNode.h"
#include "Block.h"

#define PORT 8080
*/
#include "Blockchain.h"
#include <iostream>
int main()
{
	//TODO
	Blockchain chain;
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

}
