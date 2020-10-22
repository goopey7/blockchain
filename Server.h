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
#include "Blockchain.h"

#define PORT 9162

class Server
{
private:
	int clientsConnected=0;
public:
	void listenAndObeyClients();
	void start();
};


#endif //BLOCKCHAIN_SERVER_H
