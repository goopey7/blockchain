//
// Created by sam on 08/11/2020.
//

#ifndef BLOCKCHAIN_ITEM_H
#define BLOCKCHAIN_ITEM_H


#include <string>

class Item
{
private:
	std::string itemName;
	int itemID;
	int damage;
	int durability;
	bool bConsumable;
};


#endif //BLOCKCHAIN_ITEM_H
