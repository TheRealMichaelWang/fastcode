#include <iostream>
#include "error.h"
#include "string.h"

int block_checksum(char* line)
{
	int brace_sum = 0;
	int bracket_sum = 0;
	int paren_sum = 0;

	for (size_t i = 0; i < strlen(line); i++)
	{
		if (line[i] == '{') {
			brace_sum++;
		}
		else if (line[i] == '}') {
			brace_sum--;
		}
		else if (line[i] == '[') {
			bracket_sum++;
		}
		else if (line[i] == ']') {
			bracket_sum--;
		}
		else if (line[i] == '(') {
			paren_sum++;
		}
		else if (line[i] == ')') {
			paren_sum--;
		}
	}
	return brace_sum == 0 && bracket_sum == 0 && paren_sum == 0;
}

void str_append(char* to_append, const char* append_with)
{
	int offset = strlen(to_append);
	for (size_t i = 0; i < strlen(append_with); i++)
	{
		to_append[offset + i] = append_with[i];
	}
	to_append[offset + strlen(append_with)] = 0;
}

unsigned long file_path_hash(char* path) {
	unsigned long hash = 5381;
	for (int i = strlen(path) - 1; i >= 0; i--)
	{
		if (path[i] == '\\' || path[i] == '/') {
			break;
		}
		hash = ((hash << 5) + hash) + path[i];
	}
	return hash;
}

unsigned long dj2b(char* path) {
	unsigned long hash = 5381;
	for (int i = strlen(path) - 1; i >= 0; i--)
	{
		hash = ((hash << 5) + hash) + path[i];
	}
	return hash;
}