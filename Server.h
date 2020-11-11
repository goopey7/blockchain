//
// Created by sam on 20/10/2020.
//

#ifndef BLOCKCHAIN_SERVER_H
#define BLOCKCHAIN_SERVER_H
#pragma comment(lib, "Ws2_32.lib")
#include <string>
#include <thread>
#ifndef WIN64
#include <ws2tcpip.h>
#else
#include <unistd.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif
#include <stdio.h>
#include <stdlib.h>


#include "Blockchain.h"

#define PORT 9162
#define DELIM "***********%%%%%%%%%%%^^^^^^^^^^^^^^^MSG_END^^^^^^^^^^^^^^^%%%%%%%%%%%***********"

class Server
{
private:
	Blockchain* chain=new Blockchain;
	std::vector<std::thread*> threads;
	bool bVerbose=false;
public:
	Server();
	void listenAndObeyClient();
	void pingConnectedClients();
	void start();

	void displayHelpMenu();
};


#endif //BLOCKCHAIN_SERVER_H
