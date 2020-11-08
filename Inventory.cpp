//
// Created by sam on 08/11/2020.
//

#include <chrono>
#include <random>
#include "sha256.h"
#include "Inventory.h"

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
	unsigned char* pub_hex = priv2pub(reinterpret_cast<const unsigned char*>(privateKey.c_str()), POINT_CONVERSION_UNCOMPRESSED );
	std::string pubKey = reinterpret_cast<char*>(pub_hex);
	free( pub_hex );
	std::cout << "Public key: " << pubKey << "\n";
	return pubKey;
}

Inventory::Inventory(std::string privateKey,Blockchain* _chain)
{
	publicKey = generatePublicKey(privateKey);
	chain=_chain;
	//TODO read the blockchain to determine our inventory
	//TODO figure out how we are going to represent items in the blockchain
}

Inventory::Inventory(Blockchain* _chain)
{
	chain=_chain;
	publicKey=generatePublicKey(generatePrivateKey());
}
