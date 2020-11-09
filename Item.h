//
// Created by sam on 08/11/2020.
//

#ifndef BLOCKCHAIN_ITEM_H
#define BLOCKCHAIN_ITEM_H


#include <string>

class Item //this class should be inherited in the game engine for any player items created
{
private:
	std::string itemName; // player can personalize their items by assigning a name to them.
	std::string itemID; // identifier the game uses to get the specific item
public:
	Item(std::string _itemID,std::string _itemName);
	std::string getItemID(){return itemID;}
	std::string getItemName(){return itemName;}
	void setItemName(std::string _name){itemName=_name;}
	std::string toString();
};


#endif //BLOCKCHAIN_ITEM_H
