//
// Created by sam on 08/10/2020.
//

#include <iostream>
#include <ctime>
#include <iomanip>
#include "Block.h"

Block::Block(std::string _data)
{
	data=_data;
	timeStamp=generateTimeStamp();
}

Block::Block(uint64_t _index, std::string _data, std::string _prevHash, std::string _timeStamp, uint64_t _nonce,
			 short _difficulty)
{
	index=_index;
	data=_data;
	prevHash=_prevHash;
	timeStamp=_timeStamp;
	nonce=_nonce;
	difficulty=_difficulty;
}

std::string Block::generateHash()
{
	return picosha2::hash256_hex_string(std::to_string(index)+prevHash+data+
										std::to_string(difficulty)+timeStamp+std::to_string(nonce));
}

// referred to stack overflow for this one https://stackoverflow.com/questions/16357999/current-date-and-time-as-string
std::string Block::generateTimeStamp()
{
	long t = std::time(nullptr);
	tm localTime = *std::localtime(reinterpret_cast<const time_t *>(&t));
	std::ostringstream oss;
	oss << std::put_time(&localTime, "%d-%m-%Y %H:%M:%S");
	std::string returnStr = oss.str();
	return returnStr;
}
