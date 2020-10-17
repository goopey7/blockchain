#include <iostream>
#include "LLNode.h"

int main()
{
	LLNode<char> list;
	char a='a';
	char b='b';
	char c='c';
	list.add(&a);
	list.add(&b);
	list.add(&c);
	std::cout<<list.size()<<"\n";
	list.remove(0);
	for(int i=0;i<list.size();i++)
		std::cout<<*list.get(i);
	list.clear();
}
