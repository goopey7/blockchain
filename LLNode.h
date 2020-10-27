/*

 MIT License

Copyright (c) 2020 Sam Collier

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

 */

#ifndef BLOCKCHAIN_LLNODE_H
#define BLOCKCHAIN_LLNODE_H

#include <cstdint>
#include <cstdlib>

template <class T>
class LLNode
{
public:
	LLNode();
	explicit LLNode(T* _data);
	LLNode<T>* getNextNode();
	T* getData();

	// get element at index
	T* at(uint64_t index);

	// we don't actually need it since in c++ we can use at to get the reference and then assign something to it.
	T* set(uint64_t index, T* _data); // set specified element to specified data

	uint64_t size();
	void add(T* _data); // add to the end of the list
	void add(uint64_t index, T* _data); // insert at index
	T* remove(uint64_t index); // remove an element
	void clear(); // empty list, and clear memory. Should be called when done with the list.
private:
	LLNode<T>* next;
	T* data;
};

template<class T>
LLNode<T>::LLNode()
{
	data= nullptr;
	next= nullptr;
}

template<class T>
LLNode<T>::LLNode(T* _data)
{
	data=_data;
	next=nullptr;
}

template<class T>
LLNode<T>* LLNode<T>::getNextNode()
{
	return next;
}

template<class T>
T* LLNode<T>::getData()
{
	return data;
}

template<class T>
T* LLNode<T>::at(uint64_t index)
{
	if(index==0)return data;
	return next->at(index - 1);
}

template<class T>
T* LLNode<T>::set(uint64_t index, T* _data)
{
	// we have reached the element we want to modify
	if(index==0)
	{
		T* returnData=data;
		data=_data;
		return returnData;
	}
	else return next->set(index-1,_data);
}

template<class T>
uint64_t LLNode<T>::size()
{
	if(next==nullptr)return 1;
	return next->size()+1;
}

template<class T>
void LLNode<T>::add(T* _data)
{
	// we have just created the list for the first time
	if(next==nullptr&&data==nullptr&&size()==1)
		data=_data;
	else if(next==nullptr) // if we reach the end of the list
		next = new LLNode<T>(_data);
	else next->add(_data);
}

// clears the list and frees memory
template<class T>
void LLNode<T>::clear()
{
	if(this== nullptr)return;
	if(next!=nullptr)next->clear();
	if(next==nullptr)
	{
		delete this;
	}
}

template<class T>
void LLNode<T>::add(uint64_t index, T* _data)
{
	// reach node before node specified
	if(index==1)
	{
		LLNode<T> newNode = new LLNode<T>(_data);
		LLNode<T> myNext = next;
		next=newNode;
		next->getNextNode()=myNext;
	}
	// we don't want to remove the first index, so instead we create a new node and copy stuff over to it.
	else if(index==0)
	{
		LLNode<T> newNode = new LLNode<T>(data);
		data=_data;
		LLNode<T> myNext = next;
		next=newNode;
		newNode.getNextNode()=myNext;
	}
	else next->add(index-1,_data);
}

template<class T>
T* LLNode<T>::remove(uint64_t index)
{
	// we should stop at the node right before the one we want, so we can delete it and then connect this node to the
	// one after our deleted node
	if(index==1)
	{
		LLNode<T>* myNext = next->getNextNode();
		T* dat = next->getData();
		delete next;
		next=myNext;
		return dat;
	}
	// if we want to delete the first element, instead we'll just copy the next element to it and then link to the node
	// two intervals away
	else if(index==0)
	{
		T* dat=data;
		data=next->getData();
		LLNode<T>* nextNode=next->getNextNode();
		delete next;
		next=nextNode;
		return dat;
	}
	else return remove(index-1);
}

#endif //BLOCKCHAIN_LLNODE_H
