//
// Created by sam on 20/10/2020.
//

#include "Server.h"
#include <thread>
#include <iostream>
#include <arpa/inet.h>
#include "Blockchain.h"

void Server::listenAndObeyClients()
{
	while(true)
	{
		int server_fd, new_socket, valread;
		struct sockaddr_in address;
		int opt = 1;
		int addrlen = sizeof(address);
		char buffer[1024] = {0};
		std::string hello = "Hello from server";

		// Creating socket file descriptor
		if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
		{
			perror("socket failed");
			exit(EXIT_FAILURE);
		}

		// Forcefully attaching socket to the port 8080
		if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
					   &opt, sizeof(opt)))
		{
			perror("setsockopt");
			exit(EXIT_FAILURE);
		}
		address.sin_family = AF_INET;
		address.sin_addr.s_addr = INADDR_ANY;
		address.sin_port = htons(PORT);

		// Forcefully attaching socket to the port 8080
		if (bind(server_fd, (struct sockaddr*) &address,
				 sizeof(address)) < 0)
		{
			perror("bind failed");
			exit(EXIT_FAILURE);
		}
		if (listen(server_fd, 3) < 0)
		{
			perror("listen");
			exit(EXIT_FAILURE);
		}
		if ((new_socket = accept(server_fd, (struct sockaddr*) &address,
								 (socklen_t*) &addrlen)) < 0)
		{
			perror("accept");
			exit(EXIT_FAILURE);
		}
		valread = read(new_socket, buffer, 1024);
		std::string bufferStr(buffer);
		if(bufferStr=="PleaseSendYourCopyOfTheBlockchainThanks")
		{
			std::cout<<"Sending blockchain to client\n";
			std::vector<std::string>* chainToSend=chain.write("blockchain.txt");
			std::string chainStr="BLOCKCHAIN_INCOMING:";
			for(int i=0;i<chainToSend->size();i++)
			{
				chainStr+=chainToSend->at(i);
				chainStr+="\n";
			}
		}
		else if(bufferStr=="Hello! Are you awake?")
		{
			std::string awake = "Yeah, I'm awake";
			send(new_socket,awake.c_str(),awake.length(),0);
			std::cout << "Received ping from " << inet_ntoa(address.sin_addr) << std::endl;
		}
		else if(bufferStr=="Logging in...")
		{
			std::cout << "Client logged in\n";
			clientsConnected++;
		}
		else if(bufferStr=="Disconnecting...")
		{
			std::cout << "Client disconnected\n";
			clientsConnected--;
		}
		close(server_fd);
	}
}

void Server::start()
{
	std::thread tAcceptClients(&Server::listenAndObeyClients, this);
	// distribute blockchain to those who don't already have one
	// 
	std::string input;
	while(input!="s")
	{
		ReadAndWrite::getInputAsString(input);
		if(input=="list")
		{
			std::cout << "Clients Connected: " << clientsConnected << std::endl;
		}
	}
	tAcceptClients.detach();
}
