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
	int difficulty;
	LLNode<Block> chain;
public:
	bool validateChain();
};


#endif //BLOCKCHAIN_BLOCKCHAIN_H
