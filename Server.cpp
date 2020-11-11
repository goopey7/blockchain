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
		// https://www.geeksforgeeks.org/socket-programming-cc/
		int serverFileDescriptor, newSocket, valread;
		struct sockaddr_in address;
		std::string bufferStr="";
		int opt = 1;
		int addrlen = sizeof(address);
		char buffer[1024] = {0};
		std::string hello = "Hello from server";

		// Creating socket file descriptor
		if ((serverFileDescriptor = socket(AF_INET, SOCK_STREAM, 0)) == 0)
		{
			perror("socket failed");
			exit(EXIT_FAILURE);
		}

		// Forcefully attaching socket to the port
		if (setsockopt(serverFileDescriptor, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
					   &opt, sizeof(opt)))
		{
			perror("setsockopt");
			exit(EXIT_FAILURE);
		}
		address.sin_family = AF_INET;
		address.sin_addr.s_addr = INADDR_ANY;
		address.sin_port = htons(PORT);

		// Forcefully attaching socket to the port 8080
		if (bind(serverFileDescriptor, (struct sockaddr*) &address,
				 sizeof(address)) < 0)
		{
			perror("bind failed");
			exit(EXIT_FAILURE);
		}
		if (listen(serverFileDescriptor, 3) < 0)
		{
			perror("listenForPing");
			exit(EXIT_FAILURE);
		}
		if ((newSocket = accept(serverFileDescriptor, (struct sockaddr*) &address,
								(socklen_t*) &addrlen)) < 0)
		{
			perror("accept");
			exit(EXIT_FAILURE);
		}
		// keep reading from the buffer until we have reached the delimiter
		while(bufferStr.find(DELIM)==std::string::npos)
		{
			valread = read(newSocket, buffer, 1024);
			if(valread < 0)break; // this means there was an error reading the socket
			bufferStr+=std::string(buffer, valread);
		}
		if(valread<0)bufferStr="";
		bufferStr=bufferStr.substr(0,
							 bufferStr.find(DELIM));
		std::string hashToCheck = bufferStr.substr(bufferStr.length()-64,bufferStr.length());
		bufferStr=bufferStr.substr(0,bufferStr.length()-64);
		if(hashToCheck!=picosha2::hash256_hex_string(bufferStr))
			bufferStr="";
		// send the difficulty
		if (bufferStr=="SEND_DIFFICULTY")
		{
			std::string diffStr="DIFFICULTY:"+std::to_string(chain->getDifficulty());
			diffStr+=picosha2::hash256_hex_string(diffStr);
			diffStr+=DELIM;
			send(newSocket, diffStr.c_str(), diffStr.length(), 0);
			if(bVerbose)std::cout << "Difficulty sent to " << inet_ntoa(address.sin_addr) << std::endl;
		}
		else if(bufferStr=="SEND_CHAIN") // send the blockchain
		{
			std::vector<std::string>* chainToSend=chain->write("ServerBlockchain.txt");
			if(chainToSend== nullptr) // if we don't have a blockchain, ask the client to send their blockchain.
			{
				std::string empty="EMPTY_CHAIN";
				empty+=picosha2::hash256_hex_string(empty);
				empty+=std::string(DELIM);
				send(newSocket, empty.c_str(), empty.length(), 0);
				break;
			}
			// we need to convert our stored blockchain to a string
			std::string chainStr="BLOCKCHAIN_INCOMING:";
			for(int i=0;i<chainToSend->size();i++)
			{
				chainStr+=chainToSend->at(i);
				chainStr+='\n';
			}
			chainStr+=picosha2::hash256_hex_string(chainStr);
			chainStr+=DELIM;
			send(newSocket, chainStr.c_str(), chainStr.length(), 0);
			if(bVerbose)std::cout << "Blockchain sent to " << inet_ntoa(address.sin_addr) << std::endl;
			delete chainToSend;
		}
		// If we are receiving a client's blockchain
		else if(bufferStr.find("BLOCKCHAIN_UPDATE_REQ:")!=std::string::npos)
		{
			if(bVerbose)std::cout << "BLOCKCHAIN_UPDATE_REQ from " << inet_ntoa(address.sin_addr) << std::endl;
			std::string recvd="REQ_RECVD";
			recvd+=picosha2::hash256_hex_string(recvd);
			recvd+=std::string(DELIM);
			send(newSocket, recvd.c_str(), recvd.length(), 0);
			bufferStr=bufferStr.substr(0,bufferStr.find_last_of('}')+1);
			std::vector<std::string> clientChain;
			std::string clientChainStr = bufferStr.substr(std::string("BLOCKCHAIN_UPDATE_REQ:").length());

			// convert string into a vector which can be read into a blockchain
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

			Blockchain* incomingClientChain=new Blockchain;
			incomingClientChain->read(&clientChain);

			//TODO This won't work when peers are validating a block.
			// How do we handle when the server's block is deemed invalid?
			if(incomingClientChain->length()>chain->length() && incomingClientChain->validateChain())
			{
				delete chain;
				chain=incomingClientChain;
			}
			else delete incomingClientChain;
		}
		else if(bufferStr=="Hello! Are you awake?")
		{
			std::string awake = "Yeah, I'm awake";
			awake+=picosha2::hash256_hex_string(awake);
			awake+=std::string(DELIM);
			send(newSocket, awake.c_str(), awake.length(), 0);
			if(bVerbose)std::cout << "Received ping from " << inet_ntoa(address.sin_addr) << std::endl;
		}
		else if(bufferStr=="Logging in...") // create a thread for every logged in client
		{
			// ideally we will have a thread for every client that is connected
			std::cout << inet_ntoa(address.sin_addr)<<" logged in\n";
			std::thread* t=new std::thread(&Server::listenAndObeyClient,this);
			threads.push_back(t);
		}
		else if(bufferStr=="Disconnecting...")
		{
			// remove thread on client disconnect
			std::cout << inet_ntoa(address.sin_addr) << " disconnected\n";
			threads[threads.size()-1]->detach();
			delete threads[threads.size()-1];
			threads.pop_back();
		}
		else
		{
			std::string unknown = "ERROR:UnknownCommand";
			unknown+=picosha2::hash256_hex_string(unknown);
			unknown+=std::string(DELIM);
			send(newSocket, unknown.c_str(), unknown.length(), 0);
		}
		close(serverFileDescriptor);
		close(newSocket);
	}
}

void Server::start()
{
	std::thread tAcceptClients(&Server::listenAndObeyClient, this);
	std::string input;
	// this is where we capture commands input by the server operator
	while(input!="s"&&input!="shutdown"&&input!="exit")
	{
		std::cout << "\n> ";
		ReadAndWrite::getInputAsString(input);
		if(input=="h"||input=="help"||input=="?")
			displayHelpMenu();
		else if(input.substr(0,14)=="difficulty set")
		{
			try
			{
				short cmdDifficulty = std::stoi(input.substr(15));
				// I know sha256 is 64 bytes, but I don't really want difficulty to exceed 60
				if(cmdDifficulty<11&&cmdDifficulty>0)
					chain->setDifficulty(cmdDifficulty);
				else std::cout << "\nPlease use a number between 0 and 11 exclusive\n";
			}
			catch(...)
			{
				std::cout << "\nUsage Invalid!\nUsage: difficulty set <<<NUMBER>>>\n";
			}
		}
		else if (input=="verbose"||input=="v")
		{
			bVerbose=!bVerbose; //toggle verbosity
		}
		else if(input=="clear"||input=="cls")
		{
			CLEAR_SCREEN
		}
		else if(input!="s"&&input!="shutdown"&&input!="exit")
		{
			std::cout << "\nUnknown Command: Use '?' to view the help screen\n";
		}
	}
	// cleanup
	tAcceptClients.detach();
	for(int i=0;i<threads.size();i++)
	{
		threads[i]->detach();
		delete threads[i];
	}
}

Server::Server()
{
	//read in our chain
	chain->read("ServerBlockchain.txt");
	chain->validateChain(); // if the chain isn't valid, it becomes valid by deleting the invalid parts
}

void Server::displayHelpMenu()
{
	std::cout << "**************************************** Help ****************************************\n";
	std::cout << "command                                       explanation\n";
	std::cout << "help, h, or ?                                 displays this help screen\n";
	std::cout << "shutdown, exit, or s                          shutdown server\n";
	std::cout << "difficulty set NUMBER                         set the difficulty to specified value\n";
	std::cout << "verbose or v                                  toggle output verbosity\n";
	std::cout << "clear or cls                                  clear screen\n";
	std::cout << "**************************************************************************************\n";
}
