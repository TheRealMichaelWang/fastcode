#include "errors.h"
#include "structure.h"
#include "collection.h"
#include "tokens.h"
#include "operators.h"
#include "builtins.h"
#include "io.h"

#include <fstream>
#include <iostream>
#include <stack>

namespace fastcode {
	inline void print_indent(int indent) {
		for (size_t i = 0; i < indent; i++)
			std::cout << '\t';
	}

	void print_value(value* val, bool primitive_mode);

	namespace parsing {
		void print_top_lvl_tok(token* token, int indent = 0) {
			print_indent(indent);
			switch (token->type)
			{
			case TOKEN_INCLUDE:
				((include_token*)token)->print();
				break;
			case TOKEN_SET:
				((set_token*)token)->print();
				break;
			case TOKEN_FUNCTION_CALL:
				((function_call_token*)token)->print();
				break;
			case TOKEN_RETURN:
				((return_token*)token)->print();
				break;
			case TOKEN_UNARY_OP:
				((unary_operator_token*)token)->print();
				break;
			case TOKEN_IF:
			case TOKEN_ELIF:
			case TOKEN_ELSE:
			case TOKEN_WHILE:
				((conditional_token*)token)->print(indent);
				break;
			case TOKEN_FOR:
				((for_token*)token)->print(indent);
				break;
			case TOKEN_BREAK:
				std::cout << "break";
				break;
			case TOKEN_STRUCT_PROTO:
				((structure_prototype*)token)->print();
				break;
			case TOKEN_FUNC_PROTO:
				((function_prototype*)token)->print();
				break;
			default:
				throw ERROR_UNEXPECTED_TOKEN;
			}
		}

		void print_value_tok(token* token) {
			switch (token->type)
			{
			case TOKEN_VALUE:
				((value_token*)token)->print();
				break;
			case TOKEN_VAR_ACCESS:
				((variable_access_token*)token)->print();
				break;
			case TOKEN_FUNCTION_CALL:
				((function_call_token*)token)->print();
				break;
			case TOKEN_BINARY_OP:
				((binary_operator_token*)token)->print();
				break;
			case TOKEN_UNARY_OP:
				((unary_operator_token*)token)->print();
				break;
			case TOKEN_GET_REFERENCE:
				((get_reference_token*)token)->print();
				break;
			case TOKEN_CREATE_ARRAY:
				((create_array_token*)token)->print();
				break;
			case TOKEN_CREATE_STRUCT:
				((create_struct_token*)token)->print();
				break;
			case TOKEN_SET:
				((set_token*)token)->print();
				break;
			default:
				throw ERROR_UNEXPECTED_TOKEN;
			}
		}

		void print_token_body(std::list<token*> tokens, int indent) {
			std::cout << " {" << std::endl;
			for (auto i = tokens.begin(); i != tokens.end(); ++i) {
				print_top_lvl_tok(*i, indent + 1);
				std::cout << std::endl;
			}
			print_indent(indent);
			std::cout << '}';
		}

		void value_token::print() {
			print_value(this->inner_value_ptr, false);
		}

		void identifier_token::print() {
			std::cout << this->id_str_ptr;
		}

		void variable_access_token::print() {
			for (auto i = modifiers.begin(); i != modifiers.end(); ++i) {
				if ((*i)->type == TOKEN_IDENTIFIER) {
					if (i != modifiers.begin())
						std::cout << '.';
					identifier_token* id_tok = (identifier_token*)*i;
					id_tok->print();
				}
				else if ((*i)->type == TOKEN_INDEX) {
					index_token* index_tok = (index_token*)*i;
					index_tok->print();
				}
				else
					throw ERROR_UNEXPECTED_TOKEN;
			}
		}

		void index_token::print() {
			std::cout << '[';
			print_value_tok(this->value);
			std::cout << ']';
		}

		void get_reference_token::print() {
			std::cout << "ref ";
			this->var_access->print();
		}

		void set_token::print() {
			this->destination->print();
			std::cout << " = ";
			print_value_tok(this->value);
		}

		void function_call_token::print() {
			this->identifier->print();
			std::cout << "(";
			for (auto i = this->arguments.begin(); i != this->arguments.end(); ++i) {
				if (i != this->arguments.begin())
					std::cout << ", ";
				print_value_tok(*i);
			}
			std::cout << ')';
		}

		void return_token::print() {
			std::cout << "return ";
			print_value_tok(this->value);
		}

		void conditional_token::print(int indent) {
			switch (this->type)
			{
			case TOKEN_IF:
				std::cout << "if ";
				print_value_tok(this->condition);
				break;
			case TOKEN_ELIF:
				std::cout << "elif ";
				print_value_tok(this->condition);
				break;
			case TOKEN_ELSE:
				std::cout << "else";
				break;
			case TOKEN_WHILE:
				std::cout << "while ";
				print_value_tok(this->condition);
				break;
			default:
				throw ERROR_UNEXPECTED_TOKEN;
			}
			print_token_body(this->instructions, indent);
		}

		void for_token::print(int indent) {
			std::cout << "for ";
			this->identifier->print();
			std::cout << " in ";
			print_value_tok(this->collection);
			print_token_body(this->instructions, indent);
		}

		void create_array_token::print() {
			std::cout << '[';
			for (auto i = this->values.begin(); i != this->values.end(); ++i) {
				if (i != this->values.begin())
					std::cout << ", ";
				print_value_tok(*i);
			}
			std::cout << ']';
		}

		void create_struct_token::print() {
			std::cout << "new ";
			this->identifier->print();
		}

		void function_prototype::print() {
			std::cout << "proc ";
			this->identifier->print();
			std::cout << "(";
			for (auto i = this->argument_identifiers.begin(); i != this->argument_identifiers.end(); ++i) {
				if (i != this->argument_identifiers.begin())
					std::cout << ", ";
				(*i)->print();
			}
			std::cout << ')';
			print_token_body(this->tokens, 0);
		}

		void structure_prototype::print() {
			std::cout << "struct ";
			this->identifier->print();
			std::cout << " {";
			
			for (auto i = this->properties.begin(); i != this->properties.end(); ++i) {
				std::cout << std::endl << '\t';
				(*i)->print();
			}
			
			std::cout << std::endl << '}';
		}

		void include_token::print() {
			std::cout << "include " << this->file_path;
		}

		void binary_operator_token::print() {
			print_value_tok(this->left);
			switch (this->op)
			{
			case OP_AND:
				std::cout << " and ";
				break;
			case OP_OR:
				std::cout << " or ";
				break;
			case OP_EQUALS:
				std::cout << " == ";
				break;
			case OP_NOT_EQUAL:
				std::cout << " != ";
				break;
			case OP_LESS:
				std::cout << " < ";
				break;
			case OP_MORE:
				std::cout << " > ";
				break;
			case OP_LESS_EQUAL:
				std::cout << " <= ";
				break;
			case OP_MORE_EQUAL:
				std::cout << " >= ";
				break;
			case OP_ADD:
				std::cout << " + ";
				break;
			case OP_SUBTRACT:
				std::cout << " - ";
				break;
			case OP_MULTIPLY:
				std::cout << " * ";
				break;
			case OP_DIVIDE:
				std::cout << " / ";
				break;
			case OP_MODULOUS:
				std::cout << " % ";
				break;
			case OP_POWER:
				std::cout << " ^ ";
				break;
			default:
				throw ERROR_OP_NOT_IMPLEMENTED;
			}
			print_value_tok(this->right);
		}

		void unary_operator_token::print() {
			switch (this->op)
			{
			case OP_NEGATE:
				std::cout << '-';
				break;
			case OP_INVERT:
				std::cout << '!';
				break;
			}
			print_value_tok(this->value);
			switch (this->op) {
			case OP_INCRIMENT:
				std::cout << "++";
				break;
			case OP_DECRIMENT:
				std::cout << "--";
				break;
			}
		}
	}

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
		std::cout << "Error Code: " << runtime_error << '\t' << "Error Tok Type: " << (int)err_tok->type << std::endl << std::endl;;
		parsing::print_top_lvl_tok(err_tok);
		std::cout << std::endl;
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

	void xprint(runtime::structure* structure, unsigned int indent) {
		//print_indent(indent);
		std::cout << '<' << structure->get_identifier()->get_identifier() << '>';
		runtime::reference_apartment** children = structure->get_children();
		std::list<parsing::identifier_token*> props = structure->get_proto()->get_properties();
		auto it = props.begin();
		for (size_t i = 0; i < structure->get_size(); i++)
		{
			std::cout << std::endl;
			print_indent(indent + 1);
			(*it)->print();
			std::cout << ": ";
			++it;
			print_value(children[i]->value, false);
		}
	}

	void print_array(runtime::collection* collection, bool primitive_mode) {
		bool is_str = true;
		for (size_t i = 0; i < collection->size; i++)
		{
			if (collection->get_value(i)->type != VALUE_TYPE_CHAR) {
				is_str = false;
				break;
			}
		}
		if (is_str) {
			if (primitive_mode)
				std::cout << '\"';
			for (size_t i = 0; i < collection->size; i++)
			{
				std::cout << *collection->get_value(i)->get_char();
			}
			if (primitive_mode)
				std::cout << '\"';
		}
		else {
			if (collection->size > 25) {
				std::cout << "<array>";
				return;
			}
			std::cout << '[';
			for (size_t i = 0; i < collection->size; i++)
			{
				print_value(collection->get_value(i), false);
				if (i != collection->size - 1)
					std::cout << ", ";
			}
			std::cout << ']';
		}
	}

	void print_value(value* val, bool primitive_mode) {
		switch (val->type)
		{
		case VALUE_TYPE_NULL:
			std::cout << "null";
			break;
		case VALUE_TYPE_CHAR:
			if (primitive_mode)
				std::cout << *val->get_char();
			else
				std::cout << '\'' << *val->get_char() << '\'';
			break;
		case VALUE_TYPE_NUMERICAL:
			std::cout << *val->get_numerical();
			break;
		case VALUE_TYPE_COLLECTION:
			print_array((runtime::collection*)val->ptr, primitive_mode);
			break;
		case VALUE_TYPE_STRUCT:
			if(primitive_mode)
				xprint((runtime::structure*)val->ptr, 0);
			else
				std::cout << '<' << ((runtime::structure*)val->ptr)->get_identifier()->get_identifier() << '>';
			break;
		case VALUE_TYPE_HANDLE:
			std::cout << "<handle " << (long)val->ptr << ">";
			break;
		default:
			throw ERROR_INVALID_VALUE_TYPE;
		}
	}

	namespace builtins {
		runtime::reference_apartment* print(std::vector<value*> arguments, runtime::garbage_collector* gc) {
			for (auto it = arguments.begin(); it != arguments.end(); ++it) {
				print_value(*it, true);
			}
			return gc->new_apartment(new value(VALUE_TYPE_NULL, nullptr));
		}

		runtime::reference_apartment* print_line(std::vector<value*> arguments, runtime::garbage_collector* gc) {
			runtime::reference_apartment* appt = print(arguments, gc);
			std::cout << std::endl;
			return appt;
		}

		runtime::reference_apartment* get_input(std::vector<value*> arguments, runtime::garbage_collector* gc) {
			char* input = new char[250];
			std::cin.getline(input, 250);
			runtime::collection* str = from_c_str(input, gc);
			return str->get_parent_ref();
		}

		runtime::reference_apartment* file_read_text(std::vector<value*> args, runtime::garbage_collector* gc) {
			match_arg_len(args, 1);
			match_arg_type(args[0], VALUE_TYPE_COLLECTION);

			char* file_path = to_c_str(args[0]);

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

		runtime::reference_apartment* file_write_text(std::vector<value*> args, runtime::garbage_collector* gc) {
			match_arg_len(args, 2);
			match_arg_type(args[0], VALUE_TYPE_COLLECTION);
			match_arg_type(args[1], VALUE_TYPE_COLLECTION);

			char* file_path = to_c_str(args[0]);

			std::ofstream infile(file_path, std::ofstream::binary);

			if (!infile.is_open())
				return gc->new_apartment(new value(VALUE_TYPE_NUMERICAL, new long double(0)));
			delete[] file_path;

			char* buffer = to_c_str(args[1]);
			infile.write(buffer, strlen(buffer));
			infile.close();
			delete[] buffer;

			return gc->new_apartment(new value(VALUE_TYPE_NUMERICAL, new long double(1)));
		}

		runtime::reference_apartment* system_call(std::vector<value*> args, runtime::garbage_collector* gc) {
			match_arg_len(args, 1);
			match_arg_type(args[0], VALUE_TYPE_COLLECTION);

			char* command = to_c_str(args[0]);
			system(command);
			delete[] command;

			return gc->new_apartment(new value(VALUE_TYPE_NULL, nullptr));
		}
	}
}