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

std::string Blockchain::mineBlock(Block* block)
{
	std::string hash="";
	uint64_t i=0;
	std::string zeros = "";
	for(int i=0;i<currentDifficulty;i++)zeros+="0";
	while(hash.substr(0,currentDifficulty)!=zeros)
	{
		block->setNonce(i);
		hash = block->generateHash();
		i++;
	}
	return hash;
}

