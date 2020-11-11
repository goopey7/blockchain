//
// Created by sam on 20/10/2020.
//

// TODO if we get peer mining implemented, then they also should get rewarded.
// TODO verify all messages sent using SHA256

#include "Client.h"

Client::Client()
{
	pingMainServer();
	chain=new Blockchain;
	chain->read("ClientBlockchain.txt");
	chain->validateChain();
}

void pressEnterToCont()
{
	std::string input="1";
	do
	{
		std::cout << "Press enter to continue";
		ReadAndWrite::getInputAsString(input);
	}
	while(!input.empty());
}

void Client::grabDifficulty(std::string ip,int port)
{
	std::string serverChainStr;
	while(serverChainStr.find("DIFFICULTY:")==std::string::npos)
	{
		serverChainStr = sendMessageToServer("SEND_DIFFICULTY", ip, port, true);
		// we have to wait until the server is ready to receive another message we don't want to overwhelm the server
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

void Client::pingMainServer()
{
	CLEAR_SCREEN
	//std::cout << "Pinging official server..." << std::endl;
	std::string response = sendMessageToServer(SERVER_PING_SEND_MESSAGE,OFFICIAL_IP,PORT,true);
	if(response==SERVER_PING_RECV_MESSAGE)
		bOfficialServerIsOnline=true;
}

std::string Client::sendMessageToServer(std::string message,std::string ip,int port,bool bExpectResponse)
{
	message+=picosha2::hash256_hex_string(message);
	message+=DELIM;
	// https://www.geeksforgeeks.org/socket-programming-cc/
	int sock = 0, valread;
	struct sockaddr_in serverAddress;
	char buffer[1024] = {0};
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		bOfficialServerIsOnline=false;
		printf("\n Socket creation error \n");
		return "";
	}

	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(port);

	// Convert IPv4 and IPv6 addresses from text to binary form
	if(inet_pton(AF_INET, ip.c_str(), &serverAddress.sin_addr) <= 0)
	{
		printf("\nInvalid address/ Address not supported \n");
		return "";
	}
	else if (connect(sock, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0)
	{
		//printf("\nConnection Failed \n");
		if(ip==OFFICIAL_IP)bOfficialServerIsOnline = false;
		return "";
	}
	send(sock , message.c_str() , message.length() , 0 );
	if(bExpectResponse)
	{
		std::string bufferStr;
		// keep reading from the socket until we run into our delimiter
		while(bufferStr.find(DELIM)==std::string::npos)
		{
			valread = read(sock,buffer,1024);
			if(valread>0) // read success
				bufferStr+=std::string(buffer,valread);
			else return "ERROR READING SOCKET";
		}
		close(sock);
		bufferStr=bufferStr.substr(0,bufferStr.find(DELIM));
		std::string hashToCheck = bufferStr.substr(bufferStr.length()-64,bufferStr.length());
		bufferStr=bufferStr.substr(0,bufferStr.length()-64);
		if(hashToCheck!=picosha2::hash256_hex_string(bufferStr))
			return "";
		return bufferStr;
	}
	return "";
}

void Client::grabChain(std::string ip, int port)
{
	uint64_t numPings=0;
	while(!bShuttingDown)
	{
		if(bPause){} // don't do anything if we are about to add a block
		else if(numPings%2==0) // flip between grabbing difficulty and grabbing the blockchain every 500ms
		{
			std::string serverChainStr;
			// keep asking for a blockchain until we get one
			while(serverChainStr!="EMPTY_CHAIN"&&serverChainStr.find("BLOCKCHAIN_INCOMING:")==std::string::npos)
			{
				serverChainStr = sendMessageToServer("SEND_CHAIN",ip, port,true);
				// we have to wait until the server is ready to receive another message
				// we don't want to overwhelm the server
				// https://stackoverflow.com/questions/4184468/sleep-for-milliseconds
				std::this_thread::sleep_for(std::chrono::milliseconds(100));
			}
			if(serverChainStr=="EMPTY_CHAIN") // if the server's blockchain is empty
			{
				// if we can send our chain
				if(chain!=nullptr&&chain->size()>0&&chain->validateChain())
				{
					sendChain(ip,port);
				}
			}
			// if we have received a blockchain
			else if(serverChainStr.find("BLOCKCHAIN_INCOMING:") != std::string::npos)
			{
				// get rid of prefix
				serverChainStr = serverChainStr.substr(std::string("BLOCKCHAIN_INCOMING:").length());

				//read string into a vector that our blockchain can read
				std::vector<std::string> readChain;
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
				if(serverChain==nullptr)serverChain->clear();
				serverChain = new Blockchain;
				serverChain->read(&readChain);
				uint64_t serverSize=serverChain->size();
				uint64_t chainSize;
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
			grabDifficulty(ip,port);
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
		numPings++;
	}
}


void Client::openMainMenu()
{
	std::string input;
	clientMainMenu:
	do
	{
		CLEAR_SCREEN
		input="";
		std::cout << "Main Server Status: " << (bOfficialServerIsOnline ? "Online" : "Offline") << std::endl;
		std::cout << "1: Join main server\n";
		std::cout << "2: Connect to custom server\n";
		std::cout << "3: Play offline\n";
		std::cout << "Enter: Refresh\n";
		ReadAndWrite::getInputAsString(input);
		if(input=="1"&&bOfficialServerIsOnline)
		{
			ip=OFFICIAL_IP;
			sendMessageToServer("Logging in...",OFFICIAL_IP,PORT);
			// grab chain every second
			bShuttingDown=false;
			tGrabChain=new std::thread(&Client::grabChain,this,OFFICIAL_IP,PORT);
			goto menuAfterLoggingIn;
		}
		else if(input=="2")
		{
			std::cout << "Enter the IP address: ";
			ReadAndWrite::getInputAsString(ip);
			std::string response = sendMessageToServer(SERVER_PING_SEND_MESSAGE,ip,PORT,true);
			if(response==SERVER_PING_RECV_MESSAGE)
			{
				sendMessageToServer("Logging in...",ip,PORT);
				bShuttingDown=false;
				tGrabChain=new std::thread(&Client::grabChain,this,ip,PORT);
				goto menuAfterLoggingIn;
			}
			else
			{
				std::cout << "Unable to connect to server\n";
				pressEnterToCont();
			}
		}
		else if(input=="3")
		{
			bPlayOffline=true;
			goto menuAfterLoggingIn;
		}
		else
		{
			pingMainServer();
		}
	}
	while(input!="e");
	if(input=="e")return;
	menuAfterLoggingIn:
	do
	{
		if(ip==OFFICIAL_IP&&!bOfficialServerIsOnline)break;
		CLEAR_SCREEN
		input="";
		std::cout << "Main Server Status: " << (bOfficialServerIsOnline ? "Online" : "Offline") << std::endl;
		std::cout << "Current Difficulty: " << chain->getDifficulty() << std::endl;
		std::cout << "1: Explore Chain\n";
		std::cout << "2: Delete Block\n";
		std::cout << "3: Access inventory\n";
		std::cout << "4: Create new inventory\n";
		std::cout << "Enter: Refresh\n";
		ReadAndWrite::getInputAsString(input);
		if(input=="1")
		{
			bPause=true;
			std::cout << "Enter the index of the block you'd like to view: ";
			ReadAndWrite::getInputAsString(input);
			try
			{
				uint64_t index = std::stoll(input);
				Block* blockToView = chain->at(index);
				std::cout << "Index: " << blockToView->getIndex() << std::endl;
				std::cout << "Data: " << blockToView->getData() << std::endl;
				std::cout << "Previous Hash: " << blockToView->getPrevHash() << std::endl;
				std::cout << "Time Stamp: " << blockToView->getTimeStamp() << std::endl;
				std::cout << "Nonce: " << blockToView->getNonce() << std::endl;
				std::cout << "Difficulty: " << blockToView->getDifficulty() << std::endl;
				pressEnterToCont();
			}
			catch (...){}
			bPause=false;
		}
		else if(input=="2")
		{
			bPause=true;
			do
			{
				std::cout << "Are you sure you want to delete a block from the blockchain? (Y/N)";
				ReadAndWrite::getInputAsString(input);
			}
			while(input!="y"&&input!="Y"&&input!="n"&&input!="N");
			if(input=="y"||input=="Y")
			{
				std::cout << "Enter index of block to delete: ";
				ReadAndWrite::getInputAsString(input);
				try
				{
					uint64_t indexToRemove = std::stoll(input);
					chain->deleteBlockFromChain(indexToRemove);
					chain->write("ClientBlockchain.txt");
				}
				catch (...){}
			}
			bPause=false;
		}
		else if(input=="3") //access an inventory
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
		else if(input=="4") // generate an inventory
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
				if(ip==OFFICIAL_IP&&!bOfficialServerIsOnline)break;
				CLEAR_SCREEN
				inventory->updateInventory(chain);
				std::cout << "Main Server Status: " << (bOfficialServerIsOnline ? "Online" : "Offline") << std::endl;
				std::cout << "Current Difficulty: " << chain->getDifficulty() << std::endl;
				std::cout << "Inventory Address: " << inventory->getPublicKey() << std::endl;
				std::cout << "Items: ";
				for(int i=0;i<inventory->size();i++)
				{
					std::cout << i+1 << ") " << inventory->at(i)->getItemName() << " ";
				}
				std::cout << std::endl;
				std::cout << "1: Attribute an item from the game\n";
				std::cout << "2: Make a transaction\n";
				std::cout << "3: Change the name of your item\n";
				ReadAndWrite::getInputAsString(input);
				if(input=="1") // attribute item from game
				{
					bPause=true;
					std::cout << "Enter item ID (not necessarily a number, just how the game will identify the item): ";
					std::string itemID;
					ReadAndWrite::getInputAsString(itemID);
					std::cout << "Enter a personalized name for this item: ";
					std::string itemName;
					ReadAndWrite::getInputAsString(itemName);
					if(input.find('>')==std::string::npos)
					{
						// refer to the txt file if you want a clearer picture of what's going on here
						std::string data = "\n\tITEM\n\t{\n\tID:"+itemID+"\n\tNAME:"+itemName+"\n\t}\n\tFROM GAME\n\tTO "+
										   inventory->getPublicKey();
						chain->addBlockToChain(data);
						chain->write("ClientBlockchain.txt");
						inventory->updateInventory(chain);
					}
					bPause=false;
				}
				else if(input=="2") // make a transaction
				{
					bPause=true;
					std::cout << "Select the item from your inventory using it's corresponding number listed above: ";
					ReadAndWrite::getInputAsString(input);
					try
					{
						int indexOfChoice = std::stoi(input);
						indexOfChoice--;
						if(indexOfChoice<inventory->size()&&indexOfChoice>=0)
						{
							std::cout << "Enter the address of the recipient: ";
							ReadAndWrite::getInputAsString(input);
							chain->addBlockToChain(inventory->at(indexOfChoice)->toString()+"\n\tFROM "+
												   inventory->getPublicKey()+"\n\tTO "+input);
							chain->write("ClientBlockchain.txt");
							inventory->updateInventory(chain);
						}
					}
					catch(...){}
					bPause=false;
				}
				else if(input=="3") // change the name of an existing item
				{
					bPause=true;
					std::cout << "Select the item from your inventory using it's corresponding number listed above: ";
					ReadAndWrite::getInputAsString(input);
					try
					{
						int indexOfChoice = std::stoi(input);
						indexOfChoice--;
						if(indexOfChoice<inventory->size()&&indexOfChoice>=0)
						{
							std::string itemID = inventory->at(indexOfChoice)->getItemID();
							std::string currentItemName=inventory->at(indexOfChoice)->getItemName();
							std::cout << "Enter a new name for "<<itemID<<": ";
							ReadAndWrite::getInputAsString(input);
							if(input.find('>')==std::string::npos)
							{
								chain->addBlockToChain("\n\tITEM\n\t{\n\tID:"+itemID+"\n\tNAME:"+currentItemName
													   +" --> "+input+"\n\t}\n\tFROM "+inventory->getPublicKey()+"\n\tTO "+
													   inventory->getPublicKey());
								chain->write("ClientBlockchain.txt");
							}
							else
							{
								std::cout << "Mr. Polizano please stop trying to break this program."
					 					<<" I worked so hard on it\n";
								pressEnterToCont();
							}
							inventory->updateInventory(chain);
						}
					}
					catch(...){}
					bPause=false;
				}
			}
			while(input!="e"&&input!="E");
			delete inventory;
			goto menuAfterLoggingIn;
		}
	}
	while(input!="e");
	if(!bPlayOffline)
	{
		bShuttingDown=true;
		tGrabChain->detach();
		sendMessageToServer("Disconnecting...",ip,PORT);
	}
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
	std::string response;
	while(response!="REQ_RECVD")
	{
		response = sendMessageToServer(chainStr,ip,port,true);
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
}
