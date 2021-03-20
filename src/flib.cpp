#include <iostream>
#include "error.h"
#include "flib.h"

value* readLine(var_context* parent_context)
{
	char* input_buffer = new char[250];
	std::cin.getline(input_buffer, 250);
	value* toret = new value(input_buffer);
	delete[] input_buffer;
	return toret;
}

void write(value_array* args)
{
	for (size_t i = 0; i < args->size; i++)
	{
		args->collection[i]->get_var_ptr()->print();
	}
}

void writeLine(value_array* args)
{
	write(args);
	std::cout << "\n";
}

value* objSize(value_array* args)
{
	if (args->size != 1)
	{
		throw ERROR_UNEXPECTED_ARGUMENT_LENGTH;
	}
	return new value(args->collection[0]->get_var_ptr()->length());
}

value* newArray(value_array* args, var_context* parent_context)
{
	if (args->size != 1)
	{
		throw ERROR_UNEXPECTED_ARGUMENT_LENGTH;
	}
	if (args->collection[0]->get_var_ptr()->type != VALUE_TYPE_DOUBLE)
	{
		throw ERROR_MUST_HAVE_DOUBLE_TYPE;
	}
	value_array* array = new value_array((int)*((double*)args->collection[0]->get_var_ptr()->ptr));
	for (size_t i = 0; i < array->size; i++)
	{
		array->collection[i] = new unique_refrence(new value(), nullptr, parent_context);
	}
	return new value(VALUE_TYPE_ARRAY, array);
}

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
	return brace_sum == bracket_sum == paren_sum == 0;
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