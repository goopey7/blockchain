//
// Created by sam on 20/10/2020.
//

#ifndef BLOCKCHAIN_SERVER_H
#define BLOCKCHAIN_SERVER_H

#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string>
#include <thread>
#include "Blockchain.h"

#define PORT 9162

class Server
{
private:
	Blockchain chain;
	std::vector<std::thread*> threads;
	void acceptClients();
public:
	Server();
	void listenAndObeyClient();
	void pingConnectedClients();
	void start();
};


#endif //BLOCKCHAIN_SERVER_H
