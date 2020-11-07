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
#include "Blockchain.h"
#define OFFICIAL_IP "192.168.103.187"
//#define OFFICIAL_IP "74.72.186.84"
//#define OFFICIAL_IP "_official._tcp.blockchain.samcollier.tech"
#define PORT 9162

#define SERVER_PING_SEND_MESSAGE "Hello! Are you awake?"
#define SERVER_PING_RECV_MESSAGE "Yeah, I'm awake"
#define DELIM "***********%%%%%%%%%%%^^^^^^^^^^^^^^^MSG_END^^^^^^^^^^^^^^^%%%%%%%%%%%***********"

#include <thread>

class Client
{
private:
	bool bOfficialServerIsOnline=false;
	Blockchain* chain= nullptr;
	Blockchain* serverChain;
	void waitFiveSeconds();
	void pingMainServer();
	std::string sendMessageToServer(std::string message,std::string ip,int port,bool bExpectResponse=false);
	void sendChain(std::string ip, int port);
	std::thread* tGrabChain;
	bool bShuttingDown=false;
public:
	Client();
	~Client();
	bool isOfficalServerOnline(){return bOfficialServerIsOnline;}
	void openMainMenu();
	void grabChain(std::string ip, int port);
};


#endif //BLOCKCHAIN_CLIENT_H
