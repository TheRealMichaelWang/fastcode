#include <iostream>
#include <fstream>
#include "value.h"
#include "builtins.h"
#include "error.h"
#include "string.h"

value* readLine(value_array* args)
{
	char* input_buffer = new char[250];
	std::cin.getline(input_buffer, 250);
	value* toret = new value(input_buffer);
	delete[] input_buffer;
	return toret;
}

value* write(value_array* args){
	for (size_t i = 0; i < args->size; i++)
	{
		args->collection[i]->get_value_ptr()->print();
	}
	return new value();
}

value* writeLine(value_array* args) {
	delete write(args);
	std::cout << std::endl;
	return new value();
}

value* objSize(value_array* args)
{
	if (args->size != 1)
	{
		throw ERROR_UNEXPECTED_ARGUMENT_LENGTH;
	}
	return new value(args->collection[0]->get_value_ptr()->length());
}

value* newArray(value_array* args)
{
	if (args->size != 1)
	{
		throw ERROR_UNEXPECTED_ARGUMENT_LENGTH;
	}
	if (args->collection[0]->get_value_ptr()->type != VALUE_TYPE_DOUBLE)
	{
		throw ERROR_MUST_HAVE_DOUBLE_TYPE;
	}
	value_array* array = new value_array((int)*((double*)args->collection[0]->get_value_ptr()->ptr));
	for (size_t i = 0; i < array->size; i++)
	{
		array->collection[i] = new unique_reference(new value(), nullptr, nullptr);
	}
	return new value(VALUE_TYPE_ARRAY, array);
}

value* cloneValue(value_array* args) {
	return args->collection[0]->get_value_ptr()->clone();
}

value* getObjType(value_array* args) {
	return new value(args->collection[0]->get_value_ptr()->type);
}

value* file_read_text(value_array* args) {
	if (args->collection[0]->get_value_ptr()->type != VALUE_TYPE_ARRAY) {
		throw ERROR_CANNOT_ITERATE_TYPE;
	}
	value_array* char_array = (value_array*)args->collection[0]->get_value_ptr()->ptr;
	if (!char_array->checktype(VALUE_TYPE_CHAR)) {
		throw ERROR_UNEXPECTED_TOK;
	}
	char* path = new char[char_array->size+1];
	for (size_t i = 0; i < char_array->size; i++)
	{
		path[i] = *(char*)char_array->collection[i]->get_value_ptr()->ptr;
	}
	path[char_array->size] = '\0';
	std::ifstream infile;
	infile.open(path, std::ifstream::binary);
	delete[] path;
	if (infile.is_open()) {
		infile.seekg(0, std::ios::end);
		int buffer_length = infile.tellg();
		infile.seekg(0, std::ios::beg);
		char* buffer = new char[buffer_length + 1];
		infile.read(buffer, buffer_length);
		infile.close();
		buffer[buffer_length] = '\0';
		value* toret = new value(buffer);
		delete[] buffer;
		return toret;
	}
	else {
		return new value(); //return null on error
	}
}

value* file_write_text(value_array* args) {
	if (args->collection[0]->get_value_ptr()->type != VALUE_TYPE_ARRAY || args->collection[1]->get_value_ptr()->type != VALUE_TYPE_ARRAY) {
		throw ERROR_CANNOT_ITERATE_TYPE;
	}
	value_array* path_char_array = (value_array*)args->collection[0]->get_value_ptr()->ptr;
	value_array* buffer_char_array = (value_array*)args->collection[1]->get_value_ptr()->ptr;
	if (!path_char_array->checktype(VALUE_TYPE_CHAR) || !buffer_char_array->checktype(VALUE_TYPE_CHAR)) {
		throw ERROR_UNEXPECTED_TOK;
	}
	char* path = new char[path_char_array->size + 1];
	for (size_t i = 0; i < path_char_array->size; i++)
	{
		path[i] = *(char*)path_char_array->collection[i]->get_value_ptr()->ptr;
	}
	path[path_char_array->size] = '\0';
	char* buffer = new char[buffer_char_array->size];
	for (size_t i = 0; i < buffer_char_array->size; i++)
	{
		buffer[i] = *(char*)buffer_char_array->collection[i]->get_value_ptr()->ptr;
	}
	std::ofstream infile;
	infile.open(path, std::ofstream::binary);
	if (infile.is_open()) {
		infile.write(buffer, buffer_char_array->size);
		infile.close();
		delete[] path;
		delete[] buffer;
		return new value(1.0);
	}
	return new value(0.0);
}