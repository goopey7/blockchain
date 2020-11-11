//
// Created by sam on 20/10/2020.
//

#ifndef BLOCKCHAIN_CLIENT_H
#define BLOCKCHAIN_CLIENT_H
#pragma comment(lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string>
#ifndef WIN64
#include <ws2tcpip.h>
#else
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif

#include "Blockchain.h"
#include "Inventory.h"

#define OFFICIAL_IP "192.168.103.187"

#define DEFAULT_BUFLEN 1024
#define DEFAULT_PORT "9162"
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
	bool bPlayOffline=false;
	Blockchain* chain= nullptr;
	Blockchain* serverChain;
	Inventory* inventory;
	void pingMainServer();
	std::string sendMessageToServer(std::string message, const char *ip, int port, bool bExpectResponse=false);
	void sendChain(std::string ip, int port);
	std::thread* tGrabChain;
	bool bShuttingDown=false;
	bool bPause=false;
	std::string ip;
public:
	Client();
	void openMainMenu();
	void grabChain(std::string ip, int port);
	void grabDifficulty(std::string ip,int port);
};


#endif //BLOCKCHAIN_CLIENT_H
