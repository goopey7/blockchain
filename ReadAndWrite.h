//
// Created by sam on 30/09/2020.
//

#ifndef AUTHENTICATIONPROGRAM_READANDWRITE_H
#define AUTHENTICATIONPROGRAM_READANDWRITE_H
#pragma once
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sys/stat.h>

#ifdef _WIN32 // we are compiling in windows
#include <Windows.h>
#define CLEAR_SCREEN system("cls");
#define WAIT_ONE_SECOND Sleep(1000);
#else // we are compiling on linux
#include <unistd.h>
#define CLEAR_SCREEN system("clear");
#define WAIT_ONE_SECOND sleep(1);
#endif

class ReadAndWrite
{
public:
	static void getInputAsString(std::string& input);
	static std::vector<std::string>* readFile(std::string inFileName);
	static void writeFile(std::vector<std::string>* outFileContents,std::string outFileName);
};

#endif //AUTHENTICATIONPROGRAM_READANDWRITE_H
