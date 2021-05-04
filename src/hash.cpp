#include "hash.h"
#include <iostream>

unsigned long insecure_hash(const char* str){
	unsigned long hash = 5381;
	for (int i = std::strlen(str) - 1; i >= 0; i--)
	{
		hash = ((hash << 5) + hash) + str[i];
	}
	return hash;
}