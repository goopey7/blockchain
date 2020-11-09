//
// Created by sam on 08/11/2020.
//

#ifndef BLOCKCHAIN_INVENTORY_H
#define BLOCKCHAIN_INVENTORY_H
// found this code over here:
// http://www.lostintransaction.com/blog/2014/03/14/computing-a-bitcoin-address-part-1-private-to-public-key/
// I made it a little more C++ friendly
#include <stdlib.h>

#include <stdio.h>
#include <openssl/ec.h>
#include <openssl/obj_mac.h> // for NID_secp256k1
#include "ReadAndWrite.h"
#include "LLNode.h"
#include "Item.h"
#include "Blockchain.h"

class Inventory
{
private:
	std::string publicKey;
	LLNode<Item>* items=new LLNode<Item>;
	Blockchain* chain;
	// calculates and returns the public key associated with the given private key
	// - input private key and output public key are in hexadecimal
	// - output is null-terminated string
	// form = POINT_CONVERSION_[UNCOMPRESSED|COMPRESSED|HYBRID]
	unsigned char* priv2pub(const unsigned char* priv_hex,
							point_conversion_form_t form);

	// calculates and returns the public key associated with the given private key
	// - input private key is in hexadecimal
	// - output public key is in raw bytes
	// form = POINT_CONVERSION_[UNCOMPRESSED|COMPRESSED|HYBRID]
	unsigned char* priv2pub_bytes(const unsigned char* priv_hex,
								  point_conversion_form_t form,
								  unsigned char* ret);
	std::string generatePrivateKey();
	std::string generatePublicKey(std::string privateKey);
public:
	// use this constructor to access a wallet
	Inventory(std::string privateKey,Blockchain* _chain);
	// use this constructor to generate a new wallet
	Inventory(Blockchain* _chain);
	std::string getPublicKey(){return publicKey;}
	void updateInventory(Blockchain* _chain);
	Item* at(int index){return items->at(index);}
	int size();
};


#endif //BLOCKCHAIN_INVENTORY_H
