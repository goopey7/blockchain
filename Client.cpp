//
// Created by sam on 20/10/2020.
//

//TODO Implement an inventory (which should work similar to a crypto wallet)
//TODO the blockchain should store transactions not items. Some of those transactions would be rewarded through the game
// if we get peer mining implemented, then they also get rewarded.
//TODO make a generic item class
//TODO when an item's property changes, the owner should make a transaction to themselves, so that the change is updated
// on the blockchain.

#include "Client.h"

Client::Client()
{
	pingMainServer();
	chain=new Blockchain;
	chain->read("ClientBlockchain.txt");
	if(!chain->validateChain())
		chain->write("ClientBlockchain.txt");
}

void Client::grabDifficulty()
{
	std::string serverChainStr;
	while(serverChainStr.find("DIFFICULTY:")==std::string::npos)
	{
		serverChainStr = sendMessageToServer("SEND_DIFFICULTY", OFFICIAL_IP, PORT, true);
		// we have to wait until the server is ready to receive another message otherwise
		// our request for the blockchain will be dropped.
		// https://stackoverflow.com/questions/4184468/sleep-for-milliseconds
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
	if(serverChainStr.find("DIFFICULTY:")!=std::string::npos)
	{
		short diffToSet=std::stoi(serverChainStr.substr(
				std::string("DIFFICULTY:").length()));
		chain->setDifficulty(diffToSet);
	}
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
	message+=DELIM;
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
		//if for whatever reason we don't hear back from the server, we don't want both the server and the client
		//to be stuck listening
		std::string bufferStr;
		while(bufferStr.find(DELIM)==std::string::npos)
		{
			valread = read(sock,buffer,1024);
			//std::this_thread::sleep_for(std::chrono::milliseconds(5000));
			bufferStr += std::string(buffer);
		}
		close(sock);
		return bufferStr.substr(0,bufferStr.find(DELIM));
	}
	else send(sock , message.c_str() , message.length() , 0 );
	return "";
}

void Client::grabChain(std::string ip, int port)
{
	uint64_t numPings=0;
	while(!bShuttingDown)
	{
		if(numPings%2==0)
		{
			std::string serverChainStr;
			while(serverChainStr!="EMPTY_CHAIN"&&serverChainStr.find("BLOCKCHAIN_INCOMING:")==std::string::npos)
			{
				serverChainStr = sendMessageToServer("SEND_CHAIN",ip, port,true);
				// we have to wait until the server is ready to receive another message otherwise
				// our request for the blockchain will be dropped.
				// https://stackoverflow.com/questions/4184468/sleep-for-milliseconds
				std::this_thread::sleep_for(std::chrono::milliseconds(100));
			}
			if(serverChainStr=="EMPTY_CHAIN")
			{
				if(chain!=nullptr&&chain->size()>0)
				{
					sendChain(ip,port);
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
				if(serverChain==nullptr)serverChain->clear();
				serverChain = new Blockchain;
				serverChain->read("Client'sCopyOfServerBlockchain.txt");
				uint64_t  serverSize=serverChain->size();
				uint64_t  chainSize;
				if(chain!= nullptr)
				{
					serverChain->setDifficulty(chain->getDifficulty());
					chainSize = chain->size();
				}
				else chainSize=0;
				if(serverSize>=chainSize&&serverChain->validateChain())
				{
					if(chain!=nullptr)
					{
						delete chain;
					}
					chain = serverChain;
					chain->write("ClientBlockchain.txt");
				}
				else
				{
					delete serverChain;
					//TODO send our chain to be verified by other clients
					sendChain(ip,port);
				}
			}

		}
		else
		{
			grabDifficulty();
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
		numPings++;
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
	// grab chain every second
	bShuttingDown=false;
	tGrabChain=new std::thread(&Client::grabChain,this,OFFICIAL_IP,PORT);
	do
	{
		CLEAR_SCREEN
		input="";
		std::cout << "Main Server Status: " << (bOfficialServerIsOnline ? "Online" : "Offline") << std::endl;
		std::cout << "Current Difficulty: " << chain->getDifficulty() << std::endl;
		std::cout << "1: Explore Chain\n";
		std::cout << "2: Create Block\n";
		std::cout << "3: Delete Block\n";
		std::cout << "4: Access wallet\n";
		std::cout << "5: Generate wallet\n";
		std::cout << "Enter: Refresh\n";
		ReadAndWrite::getInputAsString(input);
		if(input=="2")
		{
			std::cout << "Enter data: ";
			std::string input;
			ReadAndWrite::getInputAsString(input);
			chain->addBlockToChain(input);
			sendChain(OFFICIAL_IP,PORT);
		}
		else if(input=="3")
		{
			std::cout << "Enter index of block to delete: ";
			ReadAndWrite::getInputAsString(input);
			uint64_t indexToRemove = std::stoll(input);
			chain->deleteBlockFromChain(indexToRemove);
			chain->write("ClientBlockchain.txt");
		}
		else if(input=="4") //access an inventory
		{
			input="";
			do
			{
				std::cout << "Enter 'e' to exit\n";
				std::cout << "Enter your private key: ";
				ReadAndWrite::getInputAsString(input);
			}
			while(input.length()!=64&&input!="e"&&input!="E");
			if(input!="e"&&input!="E")
			{
				inventory=new Inventory(input,chain);
				goto inventoryMenu;
			}
			else input="";
		}
		else if(input=="5")
		{
			inventory=new Inventory(chain);
			goto inventoryMenu;
		}
		else if(input.empty()) // Enter to refresh menu
		{
			pingMainServer();
		}
		if(false) // only way in is through goto
		{
			inventoryMenu:
			input="";
			do
			{
				CLEAR_SCREEN
				std::cout << "Main Server Status: " << (bOfficialServerIsOnline ? "Online" : "Offline") << std::endl;
				std::cout << "Current Difficulty: " << chain->getDifficulty() << std::endl;
				std::cout << "Inventory Address: " << inventory->getPublicKey() << std::endl;
				ReadAndWrite::getInputAsString(input);
			}
			while(input!="e"&&input!="E");
			delete inventory;
			goto menuAfterLoggingIn;
		}
	}
	while(input!="e");
	bShuttingDown=true;
	tGrabChain->detach();
	goto clientMainMenu;
}

void Client::sendChain(std::string ip, int port)
{
	std::vector<std::string>* chainToSend=chain->write("ClientBlockchain.txt");
	std::string chainStr="BLOCKCHAIN_UPDATE_REQ:";
	for(int i=0;i<chainToSend->size();i++)
	{
		chainStr+=chainToSend->at(i);
		chainStr+='\n';
	}
	chainStr+=DELIM;
	std::string response;
	while(response!="REQ_RECVD")
	{
		response = sendMessageToServer(chainStr,ip,port,true);
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}

}