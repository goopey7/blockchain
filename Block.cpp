//
// Created by sam on 08/10/2020.
//

#include "Block.h"

long Block::getIndex()
{
	return index;
}

std::string Block::getData()
{
	return data;
}

std::string Block::getPrevHash()
{
	return prevHash;
}

std::string Block::getTimeStamp()
{
	return timeStamp;
}

long Block::getNonce()
{
	return nonce;
}

void Block::setNonce(long nonceToSet)
{
	nonce=nonceToSet;
}
