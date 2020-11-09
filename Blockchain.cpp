//
// Created by sam on 08/10/2020.
//

#include "Blockchain.h"

bool Blockchain::validateChain()
{
	//TODO add a check for chronological order
	for(int i=1;i<chain->size();i++)
	{
		if(chain->at(i-1)->generateHash()!=chain->at(i)->getPrevHash())
		{
			int diffBetweenSizeAndIndex = chain->size()-i;
			for(int j=0;j<diffBetweenSizeAndIndex;j++)
				chain->remove(i);
			return false;
		}
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
	if(chain->getNextNode()==nullptr&&chain->getData()==nullptr)
	{
		blockToAdd->setPrevHash("0"); // this is the genesis block
		blockToAdd->setIndex(0);
	}
	else
	{
		blockToAdd->setPrevHash(chain->at(chain->size()-1)->generateHash());
		blockToAdd->setIndex(chain->size());
	}
	mineBlock(blockToAdd);
	while(chain==nullptr){}
	chain->add(blockToAdd);
}

void Blockchain::addBlockToChain(Block* block)
{
	chain->add(block);
}

void Blockchain::deleteBlockFromChain(uint64_t index)
{
	chain->remove(index);
	for(int i=index;i<chain->size();i++)
	{
		uint64_t newIndex = chain->at(i)->getIndex()-1;
		chain->at(i)->setIndex(newIndex);
		if(newIndex==0)
			chain->at(i)->setPrevHash("0");
		else chain->at(i)->setPrevHash(chain->at(newIndex-1)->generateHash());
		mineBlock(chain->at(i));
	}
}

//TODO make multi-threaded!
std::string Blockchain::mineBlock(Block* block)
{
	std::string hash="";
	uint64_t i=0;
	std::string zeros = "";
	for(int j=0;j<block->getDifficulty();j++)zeros+="0";
	while(hash.substr(0,block->getDifficulty())!=zeros)
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
			std::string data="\n";
			for(int currInd=i+3;currInd<i+10;currInd++)
			{
				data += blockchainFile->at(currInd+1);
				data+='\n';
			}
			data=data.substr(0,data.length()-1);
			i+=7;
			std::string prevHash;
			try
			{
				prevHash = blockchainFile->at(i+4).substr(std::string("PreviousHash:").length());
			}
			catch (...)
			{
				prevHash = blockchainFile->at(i+5).substr(blockchainFile->at(i+5).find(':')+1);
				i++;
			}
			if(prevHash.length()!=64&&prevHash!="0")
			{
				prevHash+=blockchainFile->at(i+5);
				i++;
			}
			std::string timeStamp = blockchainFile->at(i+5).substr(std::string("TimeStamp:").length());
			if(blockchainFile->at(i+6).empty())i++;
			uint64_t nonce;
			try
			{
				nonce = std::stoll(blockchainFile->at(i+6).substr(std::string("Nonce:").length()));
			}
			catch(...)
			{
				timeStamp+=blockchainFile->at(i+6);
				i++;
				nonce = std::stoll(blockchainFile->at(i+6).substr(std::string("Nonce:").length()));
			}
			int difficulty =
					std::stoi(blockchainFile->at(i+7).substr(std::string("Difficulty:").length()));
			chain->add(new Block(index,data,prevHash,timeStamp,nonce,difficulty));
		}
	}
}

std::vector<std::string>* Blockchain::write(std::string fileName)
{
	try
	{
		std::vector<std::string>* outVector=new std::vector<std::string>;
		for(int i=0;i<chain->size();i++)
		{
			Block* blockToWrite = chain->at(i);
			if(blockToWrite== nullptr)return nullptr;
			outVector->push_back("BLOCK");
			outVector->push_back("{");
			outVector->push_back("Index:"+std::to_string(blockToWrite->getIndex()));
			std::string s=blockToWrite->getData();
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
	catch (...)
	{
		return nullptr;
	}
}

Block* Blockchain::operator[](uint64_t index)
{
	return at(index);
}
