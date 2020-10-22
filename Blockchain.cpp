//
// Created by sam on 08/10/2020.
//

#include "Blockchain.h"

bool Blockchain::validateChain()
{
	//TODO add a check for chronological order
	for(int i=1;i<chain.size();i++)
	{
		if(chain.at(i-1)->generateHash()!=chain.at(i)->getPrevHash())
			return false;
	}
	return true;
}

void Blockchain::addBlockToChain(std::string data)
{
	Block* blockToAdd = new Block(data);

	blockToAdd->setDifficulty(currentDifficulty);

	// due to the nature of my linked list, it will always have at least one element.
	// so to check if we need to make a genesis block we need to check to see if the first element contains
	// a nullptr. Our chain variable is the first element in the list, so I can call getData() directly from it.
	if(chain.size()==1&&chain.getData()==nullptr)
	{
		blockToAdd->setPrevHash("0"); // this is the genesis block
		blockToAdd->setIndex(0);
	}
	else
	{
		blockToAdd->setPrevHash(chain.at(chain.size()-1)->generateHash());
		blockToAdd->setIndex(chain.size());
	}
	mineBlock(blockToAdd);
	chain.add(blockToAdd);
}

//TODO make multi-threaded!
std::string Blockchain::mineBlock(Block* block)
{
	std::string hash="";
	uint64_t i=0;
	std::string zeros = "";
	for(int j=0;j<currentDifficulty;j++)zeros+="0";
	while(hash.substr(0,currentDifficulty)!=zeros)
	{
		block->setNonce(i);
		hash = block->generateHash();
		i++;
	}
	return hash;
}

void Blockchain::read(std::string fileName)
{
	std::vector<std::string>* blockchainFile = ReadAndWrite::readFile(fileName);
	for(int i=0;i<blockchainFile->size();i+=9)
	{
		if(blockchainFile->at(i)=="BLOCK")
		{
			uint64_t index = std::stoll(blockchainFile->at(i+2).substr(std::string("Index:").length()));
			std::string data = blockchainFile->at(i+3).substr(std::string("Data:").length());
			std::string prevHash = blockchainFile->at(i+4).substr(std::string("PreviousHash:").length());
			std::string timeStamp = blockchainFile->at(i+5).substr(std::string("TimeStamp:").length());
			uint64_t nonce = std::stoll(blockchainFile->at(i+6).substr(std::string("Nonce:").length()));
			short difficulty =
					std::stoi(blockchainFile->at(i+7).substr(std::string("Difficulty:").length()));
			chain.add(new Block(index,data,prevHash,timeStamp,nonce,difficulty));
		}
	}
}

std::vector<std::string>* Blockchain::write(std::string fileName)
{
	std::vector<std::string>* outVector=new std::vector<std::string>;
	for(int i=0;i<chain.size();i++)
	{
		Block* blockToWrite = chain.at(i);
		outVector->push_back("BLOCK");
		outVector->push_back("{");
		outVector->push_back("Index:"+std::to_string(blockToWrite->getIndex()));
		outVector->push_back("Data:"+blockToWrite->getData());
		outVector->push_back("PreviousHash:"+blockToWrite->getPrevHash());
		outVector->push_back("TimeStamp:"+blockToWrite->getTimeStamp());
		outVector->push_back("Nonce:"+std::to_string(blockToWrite->getNonce()));
		outVector->push_back("Difficulty:"+std::to_string(blockToWrite->getDifficulty()));
		outVector->push_back("}");
	}
	ReadAndWrite::writeFile(outVector,fileName);
	return outVector;
}
