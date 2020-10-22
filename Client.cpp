//
// Created by sam on 20/10/2020.
//


#include "Client.h"

Client::Client()
{
	pingMainServer();
}

Client::~Client()
{
	sendMessageToServer("Disconnecting...",OFFICIAL_IP,PORT);
}

void Client::pingMainServer()
{
	CLEAR_SCREEN
	std::cout << "Pinging official server..." << std::endl;
	std::string response = sendMessageToServer(SERVER_PING_SEND_MESSAGE,OFFICIAL_IP,PORT,true);
	if(response==SERVER_PING_RECV_MESSAGE)
		bOfficialServerIsOnline=true;
}

std::string Client::sendMessageToServer(std::string message,std::string ip,int port,bool bExpectResponse)
{
	int sock = 0, valread;
	struct sockaddr_in serv_addr;
	char buffer[1024] = {0};
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		printf("\n Socket creation error \n");
		return "";
	}

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(port);

	// Convert IPv4 and IPv6 addresses from text to binary form
	if(inet_pton(AF_INET, ip.c_str(), &serv_addr.sin_addr)<=0)
	{
		printf("\nInvalid address/ Address not supported \n");
		return "";
	}
	else if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
	{
		//printf("\nConnection Failed \n");
		bOfficialServerIsOnline = false;
		return "";
	}
	else if(bExpectResponse)
	{
		send(sock , message.c_str() , message.length() , 0 );
		valread = read( sock , buffer, 1024);
		std::string bufferStr = std::string(buffer);
		close(sock);
		return bufferStr;
	}
	else send(sock , message.c_str() , message.length() , 0 );
	return "";
}

void Client::joinServer(std::string ip, int port)
{
	sendMessageToServer("Logging in...",ip,port);
}

void Client::openMenu()
{
	bool bBadInput=false;
	std::string input;
	do
	{
		CLEAR_SCREEN
		input="";
		std::cout << "Main Server Status: " << (bOfficialServerIsOnline ? "Online" : "Offline") << std::endl;
		std::cout << "1: Join main server\n";
		std::cout << "Enter: Refresh\n";
		ReadAndWrite::getInputAsString(input);
		try
		{
			int isNumInput = std::stoi(input);
			bBadInput=false;
		}
		catch (...)
		{
			bBadInput=true;
		}
		if(input=="1")
		{
			joinServer(OFFICIAL_IP,PORT);
		}
		if(input.empty()) // Enter to refresh menu
		{
			pingMainServer();
		}
	}
	while(input!="e");
}
