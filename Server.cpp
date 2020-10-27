//
// Created by sam on 20/10/2020.
//

#include "Server.h"
#include <iostream>
#include <arpa/inet.h>
#include "Blockchain.h"

void Server::acceptClients()
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
			perror("listenForPing");
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
		if(bufferStr=="Logging in...")
		{
			std::cout << inet_ntoa(address.sin_addr)<<" logged in\n";
			numClientsConnected++;
			connectedClients.push_back(inet_ntoa(address.sin_addr));
			std::thread* t=new std::thread(&Server::listenAndObeyClient,this);
			threads.push_back(t);
		}
		else if(bufferStr=="Hello! Are you awake?")
		{
			std::string awake = "Yeah, I'm awake";
			send(new_socket,awake.c_str(),awake.length(),0);
			std::cout << "Received ping from " << inet_ntoa(address.sin_addr) << std::endl;
		}
		else
		{
			send(new_socket,"ERROR:UnknownCommand",std::string("ERROR:UnknownCommand").length(),0);
		}
		close(server_fd);
	}
}

void Server::listenAndObeyClient()
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

		// Forcefully attaching socket to the port
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
			perror("listenForPing");
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
		if(bufferStr=="SEND_CHAIN")
		{
			std::cout<<"Sending blockchain to " << inet_ntoa(address.sin_addr) << std::endl;
			std::vector<std::string>* chainToSend=chain.write("ServerBlockchain.txt");
			if(chainToSend== nullptr)
			{
				send(new_socket,"EMPTY_CHAIN",11,0);
				break;
			}
			std::string chainStr="BLOCKCHAIN_INCOMING:";
			for(int i=0;i<chainToSend->size();i++)
			{
				chainStr+=chainToSend->at(i);
				chainStr+='\n';
			}
			send(new_socket,chainStr.c_str(),chainStr.length(),0);
			std::cout << "Blockchain sent\n";
		}
		else if(bufferStr.find("BLOCKCHAIN_UPDATE_REQ:")!=std::string::npos)
		{

		}
		else if(bufferStr=="Hello! Are you awake?")
		{
			std::string awake = "Yeah, I'm awake";
			send(new_socket,awake.c_str(),awake.length(),0);
			std::cout << "Received ping from " << inet_ntoa(address.sin_addr) << std::endl;
		}
		else if(bufferStr=="Disconnecting...")
		{
			std::cout << inet_ntoa(address.sin_addr) <<" disconnected\n";
			numClientsConnected--;
		}
		close(server_fd);
	}
}

void Server::start()
{
	std::thread tAcceptClients(&Server::acceptClients, this);
	std::thread t(&Server::pingConnectedClients,this);
	// distribute blockchain to those who don't already have one
	// 
	std::string input;
	while(input!="s")
	{
		ReadAndWrite::getInputAsString(input);
		if(input=="list")
		{
			std::cout << "Clients Connected: " << numClientsConnected << std::endl;
			for(int i=0;i<connectedClients.size();i++)
			{
				std::cout << i+1 << ": " << connectedClients[i] << std::endl;
			}
		}
	}
	t.detach();
	tAcceptClients.detach();
	for(int i=0;i<threads.size();i++)
		threads[i]->detach();
}

Server::Server()
{
	chain.read("ServerBlockchain.txt");
}

void waitForResponse(int sock,std::string* buffStr)
{
	char buffer[1024]={0};
	int valread = read( sock , buffer, 1024);
	*buffStr=std::string(buffer);
}

void Server::pingConnectedClients()
{
	while(true)
	{
		for(int i=0;i<connectedClients.size();i++)
		{
			std::string ip = connectedClients[i];
			int port = PORT;
			std::string ping("are you still there?");
			int sock = 0;
			struct sockaddr_in serv_addr;
			char buffer[1024] = {0};
			if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
			{
				printf("\n Socket creation error \n");
				return;
			}

			serv_addr.sin_family = AF_INET;
			serv_addr.sin_port = htons(port);

			// Convert IPv4 and IPv6 addresses from text to binary form
			if (inet_pton(AF_INET, ip.c_str(), &serv_addr.sin_addr) <= 0)
			{
				printf("\nInvalid address/ Address not supported \n");
				return;
			}
			else if (connect(sock, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) < 0)
			{
				return;
			}
			std::cout << "Sent Ping\n";
			send(sock, ping.c_str(), ping.length(), 0);
			std::string buffStr;
			std::thread t(&waitForResponse, sock, &buffStr);
			std::this_thread::sleep_for(std::chrono::milliseconds(2500));
			t.detach();
			if (buffStr != "Yep, still here")
			{
				std::cout << "Disconnecting " << connectedClients[i] << std::endl;
				connectedClients.erase(connectedClients.begin()+i);
				numClientsConnected--;
			}
			close(sock);
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(250)); // https://stackoverflow.com/questions/4184468/sleep-for-milliseconds
	}
}
