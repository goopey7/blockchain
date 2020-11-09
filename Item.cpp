//
// Created by sam on 08/11/2020.
//

#include "Item.h"

std::string Item::toString()
{
	return "\n\tITEM\n\t{\n\tID:"+itemID+"\n\tNAME:"+itemName+"\n\t}";
}

Item::Item(std::string _itemID, std::string _itemName)
{
	itemID=_itemID;
	itemName=_itemName;
}
