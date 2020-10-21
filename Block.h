//
// Created by sam on 08/10/2020.
//

#ifndef BLOCKCHAIN_BLOCK_H
#define BLOCKCHAIN_BLOCK_H


#include <string>
#include "sha256.h"
class Block
{
private:
	uint64_t index;
	std::string data;
	std::string prevHash;
	std::string timeStamp;
	uint64_t nonce;
	short difficulty;
	std::string generateTimeStamp();
public:
	// this constructor is used when creating a new block
	Block(std::string _data);
	// this constructor is used when reading from file
	Block(uint64_t _index, std::string _data, std::string _prevHash, std::string _timeStamp,
	   uint64_t _nonce,
	   short _difficulty);
	uint64_t getIndex(){return index;}
	std::string getData(){return data;}
	std::string getPrevHash(){return prevHash;};
	std::string getTimeStamp(){return timeStamp;};
	uint64_t getNonce(){return nonce;}
	short getDifficulty(){return difficulty;}
	void setIndex(uint64_t _index){index=_index;}
	void setPrevHash(std::string _prevHash){prevHash=_prevHash;}
	void setNonce(uint64_t _nonce){nonce=_nonce;}
	void setDifficulty(short _difficulty){difficulty=_difficulty;}
	std::string generateHash();
};


#endif //BLOCKCHAIN_BLOCK_H
