#include "errors.h"
#include "structure.h"
#include "collection.h"
#include "tokens.h"
#include "builtins.h"
#include "io.h"

#include <fstream>
#include <iostream>
#include <stack>

namespace fastcode {
	void handle_syntax_err(int syntax_error, unsigned int pos, const char* source) {
		std::cout << std::endl << "***Syntax Error: " << get_err_info(syntax_error) << "***" << std::endl;
		std::cout << "Error Code: " << syntax_error << '\t' << "Lexer Index: " << pos << std::endl;
		for (int i = (int)pos - (int)17 > 0 ? pos - 17 : 0; i < strlen(source) && i < pos + 17; i++) {
			std::cout << source[i];
		}
		std::cout << "" << std::endl;
	}

	void handle_runtime_err(int runtime_error, parsing::token* err_tok) {
		std::cout << std::endl << "***Runtime Error: " << get_err_info(runtime_error) << "***" << std::endl;
		std::cout << "Error Code: " << runtime_error << '\t' << "Error Tok Type: " << (int)err_tok->type << std::endl;
	}

	void print_call_stack(std::stack<parsing::function_prototype*> call_stack) {
		std::cout << std::endl << "Stack Trace:";
		if (call_stack.empty()) {
			std::cout << " Empty";
			return;
		}
		while (!call_stack.empty())
		{
			std::cout << std::endl << "\tin " << call_stack.top()->identifier->get_identifier();
			call_stack.pop();
		}
	}

	void print_value(value* val);

	inline void print_indent(unsigned int indent) {
		for (size_t i = 0; i < indent; i++)
			std::cout << '\t';
	}

	void xprint(runtime::structure* structure, unsigned int indent) {
		print_indent(indent);
		std::cout << '<' << structure->get_identifier()->get_identifier() << '>';
		runtime::reference_apartment** children = structure->get_children();
		for (size_t i = 0; i < structure->get_size(); i++)
		{
			std::cout << std::endl;
			if (children[i]->value->type != VALUE_TYPE_STRUCT) {
				print_indent(indent + 1);
				print_value(children[i]->value);
			}
			else {
				xprint((runtime::structure*)children[i]->value->ptr, indent + 1);
			}
		}
	}

	void print_array(runtime::collection* collection) {
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
			print_array((runtime::collection*)val->ptr);
			break;
		case VALUE_TYPE_STRUCT:
			//std::cout << '<' << ((structure*)val->ptr)->get_identifier()->get_identifier() << '>';
			xprint((runtime::structure*)val->ptr, 0);
			break;
		case VALUE_TYPE_HANDLE:
			std::cout << "<handle " << (long)val->ptr << ">";
			break;
		default:
			throw ERROR_INVALID_VALUE_TYPE;
		}
	}

	namespace builtins {
		runtime::reference_apartment* print(std::list<value*> arguments, runtime::garbage_collector* gc) {
			for (auto it = arguments.begin(); it != arguments.end(); ++it) {
				print_value(*it);
			}
			return gc->new_apartment(new value(VALUE_TYPE_NULL, nullptr));
		}

		runtime::reference_apartment* print_line(std::list<value*> arguments, runtime::garbage_collector* gc) {
			runtime::reference_apartment* appt = print(arguments, gc);
			std::cout << std::endl;
			return appt;
		}

		runtime::reference_apartment* get_input(std::list<value*> arguments, runtime::garbage_collector* gc) {
			char* input = new char[250];
			std::cin.getline(input, 250);
			runtime::collection* str = from_c_str(input, gc);
			return str->get_parent_ref();
		}

		runtime::reference_apartment* file_read_text(std::list<value*> args, runtime::garbage_collector* gc) {
			match_arg_len(args, 1);
			match_arg_type(args.front(), VALUE_TYPE_COLLECTION);

			char* file_path = to_c_str(args.front());

			std::ifstream infile(file_path, std::ifstream::binary);

			if (!infile.is_open())
				return gc->new_apartment(new value(VALUE_TYPE_NUMERICAL, new long double(0)));
			delete[] file_path;

			infile.seekg(0, std::ios::end);
			int buffer_length = infile.tellg();
			infile.seekg(0, std::ios::beg);
			char* buffer = new char[buffer_length + 1];
			infile.read(buffer, buffer_length);
			infile.close();
			buffer[buffer_length] = 0;

			runtime::collection* strcol = from_c_str(buffer, gc);
			delete[] buffer;
			return strcol->get_parent_ref();
		}

		runtime::reference_apartment* file_write_text(std::list<value*> args, runtime::garbage_collector* gc) {
			match_arg_len(args, 2);
			match_arg_type(args.front(), VALUE_TYPE_COLLECTION);
			match_arg_type(args.back(), VALUE_TYPE_COLLECTION);

			char* file_path = to_c_str(args.front());

			std::ofstream infile(file_path, std::ofstream::binary);

			if (!infile.is_open())
				return gc->new_apartment(new value(VALUE_TYPE_NUMERICAL, new long double(0)));
			delete[] file_path;

			char* buffer = to_c_str(args.back());
			infile.write(buffer, strlen(buffer));
			infile.close();
			delete[] buffer;

			return gc->new_apartment(new value(VALUE_TYPE_NUMERICAL, new long double(1)));
		}
	}
}