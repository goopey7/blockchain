//
// Created by sam on 08/10/2020.
//

#ifndef BLOCKCHAIN_DLLNODE_H
#define BLOCKCHAIN_DLLNODE_H

template <class Object>
class DLLNode
{
private:
	DLLNode* next;
	DLLNode* prev;
	Object obj;
};


#endif //BLOCKCHAIN_DLLNODE_H
