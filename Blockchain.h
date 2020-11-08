//
// Created by sam on 08/10/2020.
//

#ifndef BLOCKCHAIN_BLOCKCHAIN_H
#define BLOCKCHAIN_BLOCKCHAIN_H
#pragma once
#include "LLNode.h"
#include "Block.h"
#include "ReadAndWrite.h"

class Blockchain
{
private:
	short currentDifficulty=2;
	LLNode<Block>* chain=new LLNode<Block>;
	std::string mineBlock(Block* block);
public:
	~Blockchain(){chain->clear();} //Destructor. Frees memory
	// Overload [] operator to access elements of the list
	Block* operator[](uint64_t);
	bool validateChain();
	void addBlockToChain(std::string data);
	void addBlockToChain(Block* block);
	Block* at(uint64_t index){return chain->at(index);}
	uint64_t size(){return chain->size();}
	uint64_t length(){return chain->size();}
	void clear(){chain->clear();}
	void read(std::string fileName);
	std::vector<std::string>* write(std::string fileName);
	void setDifficulty(short _difficulty){currentDifficulty=_difficulty;}
	short getDifficulty(){return currentDifficulty;}

	void deleteBlockFromChain(uint64_t index);
};

#endif //BLOCKCHAIN_BLOCKCHAIN_H
