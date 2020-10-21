//
// Created by sam on 30/09/2020.
//

#ifndef AUTHENTICATIONPROGRAM_READANDWRITE_H
#define AUTHENTICATIONPROGRAM_READANDWRITE_H

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sys/stat.h>

class ReadAndWrite
{
public:
	static void getInputAsString(std::string& input);
	static std::vector<std::string>* readFile(std::string inFileName);
	static void writeFile(std::vector<std::string>* outFileContents,std::string outFileName);
};

void ReadAndWrite::getInputAsString(std::string& input)
{
	std::getline(std::cin, input);
}

std::vector<std::string>* ReadAndWrite::readFile(std::string inFileName)
{
	// read into our database line by line
	auto* lines=new std::vector<std::string>;
	std::ifstream inFile(inFileName);
	std::string line;
	while(std::getline(inFile,line))
		lines->push_back(line);
	return lines;
}

void ReadAndWrite::writeFile(std::vector<std::string>* outFileContents, std::string outFileName)
{
	std::ofstream outFile;
	outFile.open(outFileName);
	std::string outString;
	for(int i=0;i<outFileContents->size();i++)
	{
		outString+=outFileContents->at(i);
		if(outFileContents->size()>1&&i<outFileContents->size()-1)outString+="\n";
	}
	outFile << outString;
	outFile.close();
}

#endif //AUTHENTICATIONPROGRAM_READANDWRITE_H
