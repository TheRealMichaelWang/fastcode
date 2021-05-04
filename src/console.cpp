#include "errors.h"
#include "structure.h"
#include "collection.h"
#include "console.h"
#include <iostream>

void print_value(value* val);

void print_array(collection* collection) {
	bool is_str = true;
	for (size_t i = 0; i < collection->size; i++)
	{
		if (collection->get_value(i)->type != VALUE_TYPE_CHAR) {
			is_str = false;
			break;
		}
	}
	if (is_str) {
		for (size_t i = 0; i < collection->size; i++)
		{
			std::cout << *collection->get_value(i)->get_char();
		}
	}
	else {
		if (collection->size > 25) {
			std::cout << "<array>";
			return;
		}
		std::cout << '[';
		for (size_t i = 0; i < collection->size; i++)
		{
			print_value(collection->get_value(i));
			if (i != collection->size - 1)
				std::cout << ", ";
		}
		std::cout << ']';
	}
}

void print_value(value* val) {
	switch (val->type)
	{
	case VALUE_TYPE_NULL:
		std::cout << "null";
		break;
	case VALUE_TYPE_CHAR:
		std::cout << *val->get_char();
		break;
	case VALUE_TYPE_NUMERICAL:
		std::cout << *val->get_numerical();
		break;
	case VALUE_TYPE_COLLECTION:
		print_array((collection*)val->ptr);
		break;
	case VALUE_TYPE_STRUCT:
		std::cout << '<' << ((structure*)val->ptr)->get_identifier()->get_identifier() << '>';
		break;
	default:
		throw ERROR_INVALID_VALUE_TYPE;
	}
}

reference_apartment* print(std::vector<value*> arguments, garbage_collector* gc) {
	for (auto it = arguments.begin(); it != arguments.end(); ++it) {
		print_value(*it);
	}
	return gc->new_apartment(new value(VALUE_TYPE_NULL, nullptr));
}

reference_apartment* print_line(std::vector<value*> arguments, garbage_collector* gc) {
	reference_apartment* appt = print(arguments, gc);
	std::cout << std::endl;
	return appt;	
}

reference_apartment* get_input(std::vector<value*> arguments, garbage_collector* gc) {
	char* input = new char[250];
	std::cin.getline(input, 250);
	unsigned int size = strlen(input);
	collection* str = new collection(size, gc);
	for (size_t i = 0; i < size; i++)
	{
		str->set_value(i, new value(VALUE_TYPE_CHAR, new char(input[i])));
	}
	return str->get_parent();
}

void handle_syntax_err(int syntax_error, unsigned int pos, const char* source) {
	std::cout << "***Syntax Error***" << std::endl;
	std::cout << "Error Code: " << syntax_error << '\t' << "Lexer Index: " << pos << std::endl;
	for (int i = (int)pos - (int)10 > 0 ? pos - 10 : 0; i < strlen(source) && i < pos + 10; i++)
	{
		std::cout << source[i];
	}
	std::cout << std::endl;
}

void handle_runtime_err(int runtime_error, token* err_tok) {
	std::cout << std::endl << "***Runtime Error***" << std::endl;
	std::cout << "Error Code: " << runtime_error << '\t' << "Error Tok Type: " << (int)err_tok->type << std::endl;
}