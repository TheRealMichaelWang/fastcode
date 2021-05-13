#ifndef BUILTINS_H
#define BUILTINS_H

#include <list>
#include "errors.h"
#include "value.h"
#include "references.h"
#include "collection.h"
#include "garbage.h"

typedef reference_apartment* (*built_in_function)(std::list<value*> arguments, garbage_collector* gc);

inline void match_arg_len(std::list<value*> arguments, unsigned int expected_size) {
	if (arguments.size() != expected_size)
		throw ERROR_UNEXPECTED_ARGUMENT_SIZE;
}

inline void match_arg_type(value* value, char expected_type) {
	if (value->type != expected_type)
		throw ERROR_INVALID_VALUE_TYPE;
}

inline char* to_c_str(value* value) {
	match_arg_type(value, VALUE_TYPE_COLLECTION);
	collection* collection = (class collection*)value->ptr;
	char* c = new char[collection->size + 1];
	for (size_t i = 0; i < collection->size; i++)
	{
		match_arg_type(collection->get_value(i), VALUE_TYPE_CHAR);
		c[i] = *collection->get_value(i)->get_char();
	}
	c[collection->size] = 0;
	return c;
}

inline collection* from_c_str(const char* str, garbage_collector* gc) {
	collection* str_col = new collection(strlen(str), gc);
	for (size_t i = 0; i < str_col->size; i++)
	{
		str_col->set_value(i, new value(VALUE_TYPE_CHAR, new char(str[i])));
	}
	return str_col;
}

#endif // !BUILTINS_H
