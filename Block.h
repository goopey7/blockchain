//
// Created by sam on 08/10/2020.
//

#ifndef BLOCKCHAIN_BLOCK_H
#define BLOCKCHAIN_BLOCK_H


#include <string>

class Block
{
private:
	long index;
	std::string data;
	std::string prevHash;
	std::string timeStamp;
	long nonce;
public:
	long getIndex();
	std::string getData();
	std::string getPrevHash();
	std::string getTimeStamp();
	long getNonce();
	void setNonce(long nonceToSet);
};


#endif //BLOCKCHAIN_BLOCK_H
