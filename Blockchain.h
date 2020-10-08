//
// Created by sam on 08/10/2020.
//

#ifndef BLOCKCHAIN_BLOCKCHAIN_H
#define BLOCKCHAIN_BLOCKCHAIN_H


#include <vector>
#include "Block.h"

class Blockchain
{
private:
	int difficulty;
	// TODO don't use vector, create own linked list class using recursion
	std::vector<Block*> chain;
public:
	bool validateChain();
};


#endif //BLOCKCHAIN_BLOCKCHAIN_H
