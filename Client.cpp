//
// Created by sam on 20/10/2020.
//


#include "Client.h"

Client::Client()
{
	pingMainServer();
}

void Client::pingMainServer()
{
		CLEAR_SCREEN
		std::cout << "Pinging official server..." << std::endl;
		int sock = 0, valread;
		struct sockaddr_in serv_addr;
		std::string pingServer = SERVER_PING_SEND_MESSAGE;
		char buffer[1024] = {0};
		if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		{
			printf("\n Socket creation error \n");
			return;
		}

		serv_addr.sin_family = AF_INET;
		serv_addr.sin_port = htons(PORT);

		// Convert IPv4 and IPv6 addresses from text to binary form
		if(inet_pton(AF_INET, OFFICIAL_IP, &serv_addr.sin_addr)<=0)
		{
			printf("\nInvalid address/ Address not supported \n");
			return;
		}
		else if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
		{
			//printf("\nConnection Failed \n");
			bOfficialServerIsOnline=false;
			return;
		}
		else
		{
			send(sock , pingServer.c_str() , pingServer.length() , 0 );
			valread = read( sock , buffer, 1024);
			std::string bufferStr = std::string(buffer);
			if(bufferStr==SERVER_PING_RECV_MESSAGE)
			{
				bOfficialServerIsOnline=true;
			}
		}
}

void Client::joinServer(std::string ip, int port)
{

}
