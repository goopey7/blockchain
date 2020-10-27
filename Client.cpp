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
	pingThread->detach();
	disconnect();
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
	chain.read("ClientBlockchain.txt");

	std::string serverChainStr;
	while(serverChainStr!="EMPTY_CHAIN"&&serverChainStr.find("BLOCKCHAIN_INCOMING:")==std::string::npos)
	{
		serverChainStr = sendMessageToServer("SEND_CHAIN",ip, port,true);
		// 100ms is too fast for me. 250ms works, so just in case I'll make it 500ms
		// we have to wait until the server is ready to receive another message otherwise our request for the blockchain will be dropped.
		std::this_thread::sleep_for(std::chrono::milliseconds(100)); // https://stackoverflow.com/questions/4184468/sleep-for-milliseconds
	}

	if(serverChainStr=="EMPTY_CHAIN")
	{
		//TODO if ClientChain length is greater than one, send our chain to the server
		if(chain.size()>0)
		{
			std::vector<std::string>* chainToSend=chain.write("ClientBlockchain.txt");
			std::string chainStr="BLOCKCHAIN_UPDATE_REQ:";
			for(int i=0;i<chainToSend->size();i++)
			{
				chainStr+=chainToSend->at(i);
			}
			sendMessageToServer(chainStr,ip,port);
			std::cout << "Blockchain sent\n";
		}
		//TODO Server should PING Clients to make sure they are still connected
	}
	else if(serverChainStr.find("BLOCKCHAIN_INCOMING:") != std::string::npos)
	{
		std::vector<std::string> readChain;
		serverChainStr = serverChainStr.substr(std::string("BLOCKCHAIN_INCOMING:").length());
		std::string lineToAdd;
		int i=0;
		while(i<serverChainStr.length())
		{
			if(serverChainStr.c_str()[i]=='\n')
			{
				readChain.push_back(lineToAdd);
				lineToAdd="";
			}
			else
				lineToAdd+=serverChainStr.c_str()[i];
			i++;
		}
		ReadAndWrite::writeFile(&readChain,"Client'sCopyOfServerBlockchain.txt");
		serverChain.read("Client'sCopyOfServerBlockchain.txt");
	}
	if(serverChain.size()>=chain.size())
	{
		chain=serverChain;
		chain.write("ClientBlockchain.txt");
	}
	else
	{
		//TODO send our chain to be verified by other clients
	}
}

void Client::openMainMenu()
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
			std::string ip = OFFICIAL_IP;
			pingThread = new std::thread(&Client::listenForPing, this, &ip);
		}
		if(input.empty()) // Enter to refresh menu
		{
			pingMainServer();
		}
	}
	while(input!="e");
}

void Client::disconnect()
{
	sendMessageToServer("Disconnecting...",OFFICIAL_IP,PORT);
}

void Client::listenForPing(std::string* ip)
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
	if(bufferStr=="are you still there?")
		sendMessageToServer("Yep, still here",*ip,PORT);
}
