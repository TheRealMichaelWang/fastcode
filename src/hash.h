#pragma once

#ifndef HASH_H
#define HASH_H

//don't use to hash sensitive data
unsigned long insecure_hash(const char* str);

//also an insecure hash
inline int combine_hash(int hash_a, int hash_b) {
	return (hash_a << 5) + hash_b;
}

#endif // !DJ2B_H