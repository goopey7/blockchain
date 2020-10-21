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
	Blockchain chain;
	chain.addBlockToChain("Hello there");
	chain.addBlockToChain("How are we?");
	chain.validateChain();
	std::cout << chain.at(0)->getData() << std::endl;
	std::cout << chain.at(0)->getNonce() << std::endl;
	std::cout << chain.at(0)->generateHash() << std::endl;
	std::cout << chain.at(1)->getData() << std::endl;
	std::cout << chain.at(1)->getNonce() << std::endl;
	std::cout << chain.at(1)->generateHash() << std::endl;
}
