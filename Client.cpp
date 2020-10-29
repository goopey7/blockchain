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

void Client::grabChain(std::string ip, int port)
{
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
		//TODO Send our chain to be distributed by the server
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
	clientMainMenu:
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
			sendMessageToServer("Logging in...",OFFICIAL_IP,PORT);
			goto menuAfterLoggingIn;
		}
		if(input.empty()) // Enter to refresh menu
		{
			pingMainServer();
		}
	}
	while(input!="e");
	if(input=="e")return;
	menuAfterLoggingIn:
	do
	{
		CLEAR_SCREEN
		input="";
		std::cout << "Main Server Status: " << (bOfficialServerIsOnline ? "Online" : "Offline") << std::endl;
		std::cout << "1: Update blockchain\n";
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
			grabChain(OFFICIAL_IP,PORT);
		}
		if(input.empty()) // Enter to refresh menu
		{
			pingMainServer();
		}
	}
	while(input!="e");
	goto clientMainMenu;
}
