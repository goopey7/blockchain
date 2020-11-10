//
// Created by sam on 08/11/2020.
//
#ifdef WINDOWS
#include <direct.h>
#define GetCurrentDir _getcwd
#else
#include <unistd.h>
#define GetCurrentDir getcwd
#endif
#include <chrono>
#include <random>
#include "sha256.h"
#include "Inventory.h"

//https://www.tutorialspoint.com/find-out-the-current-working-directory-in-c-cplusplus
std::string getCurrentDir()
{
	char buff[FILENAME_MAX]; //create string buffer to hold path
	GetCurrentDir(buff, FILENAME_MAX);
	std::string cwd(buff);
	return cwd;
}

inline uint64_t getNanosSinceEpoch()
{
	// we use system clock because we want unix epoch, not time since program has been opened
	std::chrono::time_point<std::chrono::system_clock> ts = std::chrono::system_clock::now();
	return std::chrono::duration_cast<std::chrono::nanoseconds>(ts.time_since_epoch()).count();
}

void pressEnterToContinue()
{
	std::string input="1";
	do
	{
		std::cout << "Press enter to continue";
		ReadAndWrite::getInputAsString(input);
	}
	while(!input.empty());
}

// found this code over here:
// http://www.lostintransaction.com/blog/2014/03/14/computing-a-bitcoin-address-part-1-private-to-public-key/
// I made it a little more C++ friendly
unsigned char* Inventory::priv2pub(const unsigned char* priv_hex, point_conversion_form_t form)
{
	// create group
	EC_GROUP* ecgrp = EC_GROUP_new_by_curve_name(NID_secp256k1);

	// convert priv key from hexadecimal to BIGNUM
	BIGNUM* priv_bn = BN_new();
	BN_hex2bn(&priv_bn, reinterpret_cast<const char*>(priv_hex));

	// compute pub key from priv key and group
	EC_POINT* pub = EC_POINT_new(ecgrp);
	EC_POINT_mul(ecgrp, pub, priv_bn, NULL, NULL, NULL);

	// convert pub_key from elliptic curve coordinate to hexadecimal
	unsigned char* ret = reinterpret_cast<unsigned char*>(EC_POINT_point2hex(ecgrp, pub, form, NULL));

	EC_GROUP_free(ecgrp);
	BN_free(priv_bn);
	EC_POINT_free(pub);

	return ret;
}

unsigned char*
Inventory::priv2pub_bytes(const unsigned char* priv_hex, point_conversion_form_t form, unsigned char* ret)
{
	// create group
	EC_GROUP* ecgrp = EC_GROUP_new_by_curve_name(NID_secp256k1);

	// convert priv key from hexadecimal to BIGNUM
	BIGNUM* priv_bn = BN_new();
	BN_hex2bn(&priv_bn, reinterpret_cast<const char*>(priv_hex));

	// compute pub key from priv key and group
	EC_POINT* pub = EC_POINT_new(ecgrp);
	EC_POINT_mul(ecgrp, pub, priv_bn, NULL, NULL, NULL);

	// convert pub_key from elliptic curve coordinate to bytes
	//  (first call gets the appropriate length to use)
	size_t len = EC_POINT_point2oct(ecgrp, pub, form, NULL, 0, NULL);
	EC_POINT_point2oct(ecgrp, pub, form, ret, len, NULL);

	EC_GROUP_free(ecgrp);
	BN_free(priv_bn);
	EC_POINT_free(pub);

	return ret;
}

std::string Inventory::generatePrivateKey()
{
	// generating a "random" number
	std::random_device rd;
	std::mt19937 mt(rd());
	std::uniform_int_distribution<int> dist(0,INT_MAX);
	uint64_t randInt=dist(mt);
	// adding the nanoseconds since epoch
	randInt+=getNanosSinceEpoch();
	// adding the real user ID of the calling process
	randInt+=getuid();
	std::string privKey = std::to_string(randInt);
	privKey+=getCurrentDir(); // grab the current working directory and add that to the end of our random number
	// hash it using SHA256
	privKey=picosha2::hash256_hex_string(privKey);
	for(int i=0;i<77;i++)std::cout<<"*";
	std::cout<<std::endl;
	std::cout << "PRIVATE KEY: " << privKey << std::endl;
	for(int i=0;i<77;i++)std::cout<<"*";
	std::cout << "\nOnce you hit ENTER you will never see this key again! It is important that you save this key"
			  " somewhere!\n";
	pressEnterToContinue();
	return privKey;
}

std::string Inventory::generatePublicKey(std::string privateKey)
{
	unsigned char* pub_hex = priv2pub(reinterpret_cast<const unsigned char*>(privateKey.c_str()),
								   POINT_CONVERSION_UNCOMPRESSED );
	std::string pubKey = reinterpret_cast<char*>(pub_hex);
	free( pub_hex );
	return pubKey;
}

Inventory::Inventory(std::string privateKey,Blockchain* _chain)
{
	publicKey = generatePublicKey(privateKey);
	chain=_chain;
}

Inventory::Inventory(Blockchain* _chain)
{
	chain=_chain;
	publicKey=generatePublicKey(generatePrivateKey());
	items->set(0, nullptr);
}

void Inventory::updateInventory(Blockchain* _chain)
{
	chain=_chain; // make sure we have the latest chain
	items->clear(); // we read the entire blockchain, so start with a fresh empty inventory
	items=new LLNode<Item>;
	for(int i=0;i<chain->size();i++)
	{
		std::string data=chain->at(i)->getData();
		// we need to parse the transaction stored in the data field of the block
		if(data.find(publicKey)!=std::string::npos) // if we find a block involving our public key
		{
			// my head works better when it's in a vector
			std::vector<std::string> itemParseHelper;
			std::string line;
			for(int j=1;j<data.length();j++)
			{
				if(data[j]!='\t')
				{
					if(data[j]!='\n')
						line+=data[j];
					else
					{
						itemParseHelper.push_back(line);
						line="";
					}
				}
			}
			// if we are making a transaction with ourselves (changing the name of an item), no need to add or remove
			if(data.find("TO "+publicKey)==std::string::npos||data.find("FROM "+publicKey)==std::string::npos)
			{
				// if we are receiving the item
				if(data.find("TO "+publicKey)!=std::string::npos)
				{
					// add it to the inventory
					Item* newItem = new Item(itemParseHelper[2].substr(3),
											 itemParseHelper[3].substr(5));
					items->add(newItem);
				}
				// if we are loosing an item
				else if(data.find("FROM "+publicKey)!=std::string::npos)
				{
					// remove it from the inventory
					Item itemToRemove(itemParseHelper[2].substr(3),itemParseHelper[3].substr(5));
					for(int j=0;j<items->size();j++)
					{
						if(items->at(j)->getItemID()==itemToRemove.getItemID()&&
						   items->at(j)->getItemName()==itemToRemove.getItemName())
						{
							items->remove(j);
							break;
						}
					}
				}
			}
			else // we are changing the item's name
			{
				std::string oldName = itemParseHelper[3].substr(5,itemParseHelper[3].find("-->")-6);
				std::string newName = itemParseHelper[3].substr(itemParseHelper[3].find('>')+2);
				std::string itemID = itemParseHelper[2].substr(3);
				for(int i=0;i<items->size();i++)
				{
					if(items->at(i)->getItemID()==itemID&&items->at(i)->getItemName()==oldName)
					{
						items->at(i)->setItemName(newName);
						break;
					}
				}
			}
		}
	}
}

int Inventory::size()
{
	if(items->at(0)==nullptr&&items->getNextNode()== nullptr&&items->getData()== nullptr)
	{
		return 0;
	}
	else return items->size();
}
