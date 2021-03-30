#include <iostream>
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
		array->collection[i] = new unique_refrence(new value(), nullptr, nullptr);
	}
	return new value(VALUE_TYPE_ARRAY, array);
}

value* cloneValue(value_array* args) {
	return args->collection[0]->get_value_ptr()->clone();
}