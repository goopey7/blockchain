//
// Created by sam on 08/10/2020.
//

#ifndef BLOCKCHAIN_BLOCKCHAIN_H
#define BLOCKCHAIN_BLOCKCHAIN_H


#include "LLNode.h"
#include "Block.h"

class Blockchain
{
private:
	short currentDifficulty=1;
	LLNode<Block> chain;
	std::string mineBlock(Block* block);
public:
	~Blockchain(){chain.clear();} //Destructor. Frees memory
	bool validateChain();
	void addBlockToChain(std::string data);
	Block* at(uint64_t index){return chain.at(index);}
	uint64_t size(){return chain.size();}

	void read(std::string fileName);
	void write(std::string fileName);
};

#endif //BLOCKCHAIN_BLOCKCHAIN_H
