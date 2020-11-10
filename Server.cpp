//
// Created by sam on 20/10/2020.
//

#include "Server.h"
#include <iostream>
#include <arpa/inet.h>
#include "Blockchain.h"

void Server::listenAndObeyClient()
{
	while(true)
	{
		int server_fd, new_socket, valread;
		struct sockaddr_in address;
		std::string bufferStr="";
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
		while(bufferStr.find(DELIM)==std::string::npos)
		{
			valread = read(new_socket, buffer, 1024);
			bufferStr+=buffer;
		}
		close(server_fd);
		bufferStr=bufferStr.substr(0,
							 bufferStr.find(DELIM));
		if (bufferStr=="SEND_DIFFICULTY")
		{
			std::string diffStr="DIFFICULTY:"+std::to_string(chain->getDifficulty())+DELIM;
			send(new_socket,diffStr.c_str(),diffStr.length(),0);
			std::cout << "Difficulty sent to " << inet_ntoa(address.sin_addr) << std::endl;
		}
		else if(bufferStr=="SEND_CHAIN")
		{
			std::vector<std::string>* chainToSend=chain->write("ServerBlockchain.txt");
			if(chainToSend== nullptr)
			{
				std::string empty=std::string("EMPTY_CHAIN"+std::string(DELIM));
				send(new_socket,empty.c_str(),empty.length(),0);
				break;
			}
			std::string chainStr="BLOCKCHAIN_INCOMING:";
			for(int i=0;i<chainToSend->size();i++)
			{
				chainStr+=chainToSend->at(i);
				chainStr+='\n';
			}
			chainStr+=DELIM;
			/*std::vector<std::string> buffersToSend;
			std::string buffer;
			for(int i=0;i<chainStr.length();i++)
			{
				if((i+1)%1000000==0&&i!=0)
				{
					buffersToSend.push_back(buffer);
					buffer="";
				}
				buffer+=chainStr[i];
			}
			for(int i=0;i<buffersToSend.size();i++)
			{
				send(new_socket,buffersToSend[i].c_str(),buffersToSend[i].length(),0);
				//std::this_thread::sleep_for(std::chrono::milliseconds(500));
			}*/
			send(new_socket,chainStr.c_str(),chainStr.length(),0);
			std::cout << "Blockchain sent to " << inet_ntoa(address.sin_addr) << std::endl;
			delete chainToSend;
		}
		// If we are receiving a client's blockchain
		else if(bufferStr.find("BLOCKCHAIN_UPDATE_REQ:")!=std::string::npos)
		{
			std::cout << "BLOCKCHAIN_UPDATE_REQ from " << inet_ntoa(address.sin_addr) << std::endl;
			std::string recvd="REQ_RECVD"+std::string(DELIM);
			send(new_socket,recvd.c_str(),recvd.length(),0);
			bufferStr=bufferStr.substr(0,bufferStr.find_last_of('}')+1);
			std::vector<std::string> clientChain;
			std::string clientChainStr = bufferStr.substr(std::string("BLOCKCHAIN_UPDATE_REQ:").length());
			std::string lineToAdd;
			int i=0;
			while(i<clientChainStr.length())
			{
				if(clientChainStr.c_str()[i]=='\n')
				{
					clientChain.push_back(lineToAdd);
					lineToAdd="";
				}
				else
					lineToAdd+=clientChainStr.c_str()[i];
				i++;
			}
			if(clientChain.at(clientChain.size()-1)!="}")
				clientChain.push_back("}");
			ReadAndWrite::writeFile(&clientChain,"Server'sCopyOfAClient.txt");
			Blockchain* incomingClientChain=new Blockchain;
			incomingClientChain->read("Server'sCopyOfAClient.txt");

			//TODO This won't work when peers are validating a block.
			// How do we handle when the server's block is deemed invalid?
			bool b = incomingClientChain->validateChain();
			bool c = incomingClientChain->length()>chain->length();
			if(incomingClientChain->length()>chain->length() && incomingClientChain->validateChain())
			{
				delete chain;
				chain=incomingClientChain;
			}
		}
		else if(bufferStr=="Hello! Are you awake?")
		{
			std::string awake = "Yeah, I'm awake"+std::string(DELIM);
			send(new_socket,awake.c_str(),awake.length(),0);
			std::cout << "Received ping from " << inet_ntoa(address.sin_addr) << std::endl;
		}
		else if(bufferStr=="Logging in...")
		{
			std::cout << inet_ntoa(address.sin_addr)<<" logged in\n";
			//numClientsConnected++;
			//connectedClients.push_back(inet_ntoa(address.sin_addr));
			//std::thread* t=new std::thread(&Server::listenAndObeyClient,this);
			//threads.push_back(t);
		}
		else
		{
			std::string unknown = "ERROR:UnknownCommand"+std::string(DELIM);
			send(new_socket,unknown.c_str(),unknown.length(),0);
		}
		close(server_fd);
		close(new_socket);
	}
}

void Server::start()
{
	std::thread tAcceptClients(&Server::listenAndObeyClient, this);
	// distribute blockchain to those who don't already have one
	// 
	std::string input;
	while(input!="s"&&input!="shutdown")
	{
		ReadAndWrite::getInputAsString(input);
		if(input=="h"||input=="help"||input=="?")
			displayHelpMenu();
		else if(input.substr(0,14)=="difficulty set")
		{
			short cmdDifficulty = std::stoi(input.substr(15));
			chain->setDifficulty(cmdDifficulty);

		}
		else if(input!="s"&&input!="shutdown")
		{
			std::cout << "Unknown Command: Use '?' to view the help screen\n";
		}
	}
	tAcceptClients.detach();
	for(int i=0;i<threads.size();i++)
		threads[i]->detach();
}

Server::Server()
{
	chain->read("ServerBlockchain.txt");
	if(!chain->validateChain())
		chain->write("ServerBlockchain.txt");
}

void Server::displayHelpMenu()
{
	std::cout << "**************************************** Help ****************************************\n";
	std::cout << "command                                       explanation\n";
	std::cout << "help, h, or ?                                 displays this help screen\n";
	std::cout << "shutdown or s                                 shutdown server\n";
	std::cout << "difficulty set NUMBER                         set the difficulty to specified value\n";
	std::cout << "**************************************************************************************\n";
}
