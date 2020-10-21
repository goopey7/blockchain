//
// Created by sam on 20/10/2020.
//

#ifndef BLOCKCHAIN_CLIENT_H
#define BLOCKCHAIN_CLIENT_H

#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string>
#define OFFICIAL_IP "blockchain.samcollier.tech"
#define PORT 8080

class Client
{
public:
	int start();
};


#endif //BLOCKCHAIN_CLIENT_H
