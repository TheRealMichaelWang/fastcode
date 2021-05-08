#include <string>
#include "collection.h"
#include "builtins.h"

reference_apartment* get_type(std::list<value*> arguments, garbage_collector* gc) {
	match_arg_len(arguments, 1);
	return gc->new_apartment(new value(VALUE_TYPE_CHAR, new char(arguments.front()->type)));
}

reference_apartment* to_string(std::list<value*> arguments, garbage_collector* gc) {
	match_arg_len(arguments, 1);
	match_arg_type(arguments.front(), VALUE_TYPE_NUMERICAL);
	long double num = *arguments.front()->get_numerical();
	
	std::string str = std::to_string(num);
	
	collection* strcol = new collection(str.length(), gc);
	for (size_t i = 0; i < str.length(); i++)
	{
		strcol->set_value(i, new value(VALUE_TYPE_CHAR, new char(str[i])));
	}
	return strcol->get_parent();
}

reference_apartment* to_numerical(std::list<value*> arguments, garbage_collector* gc) {
	match_arg_len(arguments, 1);
	match_arg_type(arguments.front(), VALUE_TYPE_COLLECTION);

	collection* strcol = (collection*)arguments.front()->ptr;
	char* str = new char[strcol->size + 1]; 
	for (size_t i = 0; i < strcol->size; i++)
	{
		value* val = strcol->get_value(i);
		match_arg_type(val, VALUE_TYPE_CHAR);
		str[i] = *val->get_char();
	}
	str[strcol->size] = 0;

	long double num = std::strtold(str, NULL);
	delete[] str;

	return gc->new_apartment(new value(VALUE_TYPE_NUMERICAL, new long double(num)));
}