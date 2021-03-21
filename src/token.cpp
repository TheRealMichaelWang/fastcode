#include <iostream>
#include "error.h"
#include "token.h"

token::token(char type)
{
	this->type = type;
}

token::token()
{
	this->type = 255;
}

token_set::token_set()
{
	tail = nullptr;
	head = nullptr;
	size = 0;
}

token_set::~token_set()
{

}

void token_set::push(token* tok) {
	if (tail == nullptr && head == nullptr) {
		head = tok;
		tail = tok;
	}
	else {
		tail->next_tok = tok;
		tail = tail->next_tok;
	}
	tail->next_tok = nullptr;
	size++;
}

void generic_destroy_token(token* token)
{
	switch (token->type)
	{
	case TOK_VALUE: {
		value_token* tok = (value_token*)token;
		delete tok;
		break;
	}
	case TOK_IDENTIFIER: {
		identifier_token* tok = (identifier_token*)token;
		delete tok;
		break;
	}
	case TOK_SET_VARIABLE: {
		set_variable_token* tok = (set_variable_token*)token;
		delete tok;
		break;
	}
	case TOK_CALL_FUNCTION:	{
		function_call_token* tok = (function_call_token*)token;
		delete tok;
		break;
	}
	case TOK_FUNCTION_PROTO: {
		function_prototype* tok = (function_prototype*)token;
		delete tok;
		break;
	}
	case TOK_STRUCT_PROTO: {
		struct_prototype* tok = (struct_prototype*)token;
		delete tok;
		break;
	}
	case TOK_PROPERTY:	{
		property_token* tok = (property_token*)token;
		delete tok;
		break;
	}
	case TOK_INDEX: {
		indexer_token* tok = (indexer_token*)token;
		delete tok;
		break;
	}
	case TOK_UNIARY_OP: {
		uniary_operator_token* tok = (uniary_operator_token*)token;
		delete tok;
		break;
	}
	case TOK_BINARY_OP: {
		binary_operator_token* tok = (binary_operator_token*)token;
		delete tok;
		break;
	}
	case TOK_IF: {
		conditional_token* tok = (conditional_token*)token;
		delete tok;
		break;
	}
	case TOK_ELIF: {
		conditional_token* tok = (conditional_token*)token;
		delete tok;
		break;
	}
	case TOK_ELSE: {
		conditional_token* tok = (conditional_token*)token;
		delete tok;
		break;
	}
	case TOK_WHILE: {
		conditional_token* tok = (conditional_token*)token;
		delete tok;
		break;
	}
	case TOK_FOR: {
		for_token* tok = (for_token*)token;
		delete tok;
		break;
	}
	case TOK_NEW_STRUCT: {
		create_struct* tok = (create_struct*)token;
		delete tok;
		break;
	}
	case TOK_CREATE_ARRAY: {
		create_array* tok = (create_array*)token;
		delete tok;
		break;
	}
	case TOK_RETURN: {
		return_token* tok = (return_token*)token;
		delete tok;
		break;
	}
	case TOK_REFRENCE: {
		refrence_token* ref = (refrence_token*)token;
		delete ref;
		break;
	}
	case TOK_IMPORT: {
		import_token* tok = (import_token*)token;
		delete tok;
		break;
	}
	default: {
		delete token;
	}
	}
}

value_token::value_token(class value* value) : token(TOK_VALUE)
{
	this->value = value;
}

value_token::~value_token()
{
	delete value;
}

identifier_token::identifier_token(char* identifier) : token(TOK_IDENTIFIER)
{
	this->identifier = identifier;
	this->modifiers = nullptr;
}

identifier_token::identifier_token(char* identifier, token_set* modifiers) : token(TOK_IDENTIFIER)
{
	this->identifier = identifier;
	this->modifiers = modifiers;
}

identifier_token::~identifier_token()
{
	if (identifier != nullptr) {
		delete[] identifier;
	}
	if (modifiers != nullptr) {
		delete modifiers;
	}
}

bool identifier_token::hasModifiers()
{
	return (this->modifiers != nullptr);
}

set_variable_token::set_variable_token(identifier_token* identifier, token* set_tok, bool global) : token(TOK_SET_VARIABLE)
{
	this->identifier = identifier;
	this->set_tok = set_tok;
	this->global = global;
}

set_variable_token::~set_variable_token()
{
	delete identifier;
	generic_destroy_token(set_tok);
}

function_call_token::function_call_token(identifier_token* identifier, token_set* arguments) : token(TOK_CALL_FUNCTION)
{
	this->identifier = identifier;
	this->arguments = arguments;
}

function_call_token::~function_call_token()
{
	delete identifier;
	delete arguments;
}

function_prototype::function_prototype(identifier_token* identifier, token_set* params, token_set* body) : token(TOK_FUNCTION_PROTO)
{
	this->identifier = identifier;
	this->params = params;
	this->body = body;
}

function_prototype::~function_prototype()
{
	delete identifier;
	delete params;
	delete body;
}

struct_prototype::struct_prototype(identifier_token* identifier, token_set* properties):token(TOK_STRUCT_PROTO)
{
	this->identifier = identifier;
	this->properties = properties;
}

struct_prototype::~struct_prototype()
{
	delete identifier;
	delete properties;
}

property_token::property_token(char* property_identifier):token(TOK_PROPERTY)
{
	this->property_identifier = property_identifier;
}

property_token::~property_token()
{
	delete[] property_identifier;
}

indexer_token::indexer_token(token* index):token(TOK_INDEX)
{
	this->index = index;
}

indexer_token::~indexer_token()
{
	generic_destroy_token(index);
}

uniary_operator_token::uniary_operator_token(char op_type, token* value) : token(TOK_UNIARY_OP)
{
	this->op_type = op_type;
	this->value = value;
}

uniary_operator_token::~uniary_operator_token()
{
	generic_destroy_token(value);
}

binary_operator_token::binary_operator_token(char op_type, token* left, token* right) : token(TOK_BINARY_OP)
{
	this->op_type = op_type;
	this->left = left;
	this->right = right;
}

binary_operator_token::~binary_operator_token()
{
	generic_destroy_token(left);
	generic_destroy_token(right);
}

conditional_token::conditional_token(char tok_type, token* condition, token_set* instructions, conditional_token* next_condition) : token(tok_type)
{
	this->condition = condition;
	this->instructions = instructions;
	this->next_condition = next_condition;
}

conditional_token::~conditional_token()
{
	if (condition != nullptr) {
		generic_destroy_token(condition);
	}
	if (next_condition != nullptr) {
		generic_destroy_token(next_condition);
	}
	delete instructions;
}

for_token::for_token(token* to_iterate, identifier_token* iterator_identifier, token_set* instructions) : token(TOK_FOR)
{
	this->to_iterate = to_iterate;
	this->iterator_identifier = iterator_identifier;
	this->instructions = instructions;
}

for_token::~for_token()
{
	generic_destroy_token(to_iterate);
	delete iterator_identifier;
	delete instructions;
}

create_struct::create_struct(identifier_token* identifier) : token(TOK_NEW_STRUCT)
{
	this->identifier = identifier;
}

create_struct::~create_struct()
{
	delete this->identifier;
}

return_token::return_token() : token(TOK_RETURN)
{
	this->ret_tok = nullptr;
}

return_token::return_token(token* return_tok) : token(TOK_RETURN)
{
	this->ret_tok = return_tok;
}

return_token::~return_token()
{
	if (this->ret_tok != nullptr)
	{
		generic_destroy_token(this->ret_tok);
	}
}

create_array::create_array(token_set* items) : token(TOK_CREATE_ARRAY)
{
	this->items = items;
}

create_array::~create_array()
{
	delete this->items;
}

refrence_token::refrence_token(token* value) : token(TOK_REFRENCE){
	this->value = value;
}

refrence_token::~refrence_token()
{
	generic_destroy_token(this->value);
}

import_token::import_token(char* path) : token(TOK_IMPORT) {
	this->path = path;
}

import_token::~import_token() {
	delete this->path;
}

bool is_op_token(char tok_type)
{
	return tok_type == TOK_PLUS || tok_type == TOK_MINUS || tok_type == TOK_MODULOUS || tok_type == TOK_CARET || tok_type == TOK_ASTERISK || tok_type == TOK_SLASH || tok_type == TOK_INCRIMENT || tok_type == TOK_DECRIMENT || tok_type == TOK_OR || tok_type == TOK_AND || tok_type == TOK_EQUALS || tok_type == TOK_MORE_EQUAL || tok_type == TOK_LESS_EQUAL || tok_type == TOK_NOT_EQUAL || tok_type == TOK_MORE || tok_type == TOK_LESS;
}

char get_op_prior(char op)
{
	if (op == TOK_OR || op == TOK_AND) {
		return 0;
	}
	else if (op == TOK_EQUALS || op == TOK_NOT_EQUAL || op == TOK_LESS || op == TOK_MORE || op == TOK_MORE_EQUAL || op == TOK_LESS_EQUAL){
		return 1;
	}
	else if (op == TOK_PLUS || op == TOK_MINUS) {
		return 2;
	}
	else if (op == TOK_ASTERISK || op == TOK_MODULOUS || op == TOK_SLASH) {
		return 3;
	}
	else if (op == TOK_CARET) {
		return 4;
	}
	throw ERROR_UNEXPECTED_TOK;
}

void print_op(char op) {
	switch (op)
	{
	case TOK_OR: std::cout << "or"; break;
	case TOK_AND: std::cout << "and"; break;
	case TOK_EQUALS: std::cout << "=="; break;
	case TOK_NOT_EQUAL: std::cout << "!="; break;
	case TOK_LESS: std::cout << '<'; break;
	case TOK_MORE: std::cout << '>'; break;
	case TOK_MORE_EQUAL: std::cout << ">="; break;
	case TOK_LESS_EQUAL: std::cout << "<="; break;
	case TOK_PLUS: std::cout << '+'; break;
	case TOK_MINUS: std::cout << '-'; break;
	case TOK_ASTERISK: std::cout << '*'; break;
	case TOK_SLASH: std::cout << '/'; break;
	case TOK_MODULOUS: std::cout << '%'; break;
	case TOK_CARET: std::cout << '^'; break;
	case TOK_INCRIMENT: std::cout << "++"; break;
	case TOK_DECRIMENT: std::cout << "--"; break;
	case TOK_NOT: std::cout << "!"; break;
	}
}

void print_body(token_set* body) {
	std::cout << '{' << std::endl;
	token* current_tok = body->head;
	while (current_tok !=nullptr)
	{
		std::cout << '\t';
		print_token(current_tok);
		current_tok = current_tok->next_tok;
	}
	std::cout << '}';
}

void print_token(token* token) {
	switch (token->type)
	{
	case TOK_VALUE: {
		value_token* tok = (value_token*)token;
		tok->value->print();
		break;
	}
	case TOK_IDENTIFIER: {
		identifier_token* tok = (identifier_token*)token;
		std::cout << tok->identifier;
		if (tok->hasModifiers()) {
			struct token* current_tok = tok->modifiers->head;
			while (current_tok != nullptr)
			{
				print_token(current_tok->next_tok);
			}
		}
		break;
	}
	case TOK_SET_VARIABLE: {
		set_variable_token* tok = (set_variable_token*)token;
		print_token(tok->identifier);
		std::cout << " = ";
		print_token(tok->set_tok);
		break;
	}
	case TOK_CALL_FUNCTION: {
		function_call_token* tok = (function_call_token*)token;
		print_token(tok->identifier);
		std::cout << '(';
		struct token* current_tok = tok->arguments->head;
		while (current_tok != nullptr)
		{
			if (current_tok != tok->arguments->head) {
				std::cout << ", ";
			}
			print_token(current_tok);
			current_tok = current_tok->next_tok;
		}
		std::cout << ") ";
		break;
	}
	case TOK_FUNCTION_PROTO: {
		function_prototype* tok = (function_prototype*)token;
		print_token(tok->identifier);
		std::cout << '(';
		struct token* current_tok = tok->params->head;
		while (current_tok != nullptr)
		{
			if (current_tok != tok->params->head) {
				std::cout << ", ";
			}
			print_token(current_tok);
			current_tok = current_tok->next_tok;
		}
		std::cout << ") ";
		print_body(tok->body);
		break;
	}
	case TOK_STRUCT_PROTO: {
		struct_prototype* tok = (struct_prototype*)token;
		print_token(tok->identifier);
		print_body(tok->properties);
		break;
	}
	case TOK_PROPERTY: {
		property_token* tok = (property_token*)token;
		std::cout << '.' << tok->property_identifier;
		break;
	}
	case TOK_INDEX: {
		indexer_token* tok = (indexer_token*)token;
		std::cout << '[';
		print_token(tok->index);
		std::cout << ']';
		break;
	}
	case TOK_UNIARY_OP: {
		uniary_operator_token* tok = (uniary_operator_token*)token;
		print_op(tok->op_type);
		print_token(tok->value);
		break;
	}
	case TOK_BINARY_OP: {
		binary_operator_token* tok = (binary_operator_token*)token;
		std::cout << '(';
		print_token(tok->left);
		std::cout << ' ';
		print_op(tok->op_type);
		std::cout << ' ';
		print_token(tok->right);
		std::cout << ')';
		break;
	}
	case TOK_IF: {
		conditional_token* tok = (conditional_token*)token;
		std::cout << "IF ";
		print_token(tok->condition);
		std::cout << ' ';
		print_body(tok->instructions);
		if (tok->next_condition != nullptr) {
			std::cout << std::endl;
			print_token(tok->next_condition);
		}
		break;
	}
	case TOK_ELIF: {
		conditional_token* tok = (conditional_token*)token;
		std::cout << "ELIF ";
		print_token(tok->condition);
		std::cout << ' ';
		print_body(tok->instructions);
		if (tok->next_condition != nullptr) {
			std::cout << std::endl;
			print_token(tok->next_condition);
		}
		break;
	}
	case TOK_ELSE: {
		conditional_token* tok = (conditional_token*)token;
		std::cout << "ELSE ";
		print_body(tok->instructions);
		break;
	}
	case TOK_WHILE: {
		conditional_token* tok = (conditional_token*)token;
		std::cout << "WHILE ";
		print_token(tok->condition);
		std::cout << ' ';
		print_body(tok->instructions);
		break;
	}
	case TOK_FOR: {
		for_token* tok = (for_token*)token;
		std::cout << "FOR ";
		print_token(tok->iterator_identifier);
		std::cout << " IN ";
		print_token(tok->to_iterate);
		std::cout << ' ';
		print_body(tok->instructions);
		break;
	}
	case TOK_NEW_STRUCT: {
		create_struct* tok = (create_struct*)token;
		std::cout << "NEW ";
		print_token(tok->identifier);
		break;
	}
	case TOK_CREATE_ARRAY: {
		create_array* tok = (create_array*)token;
		std::cout << '[';
		struct token* current_tok = tok->items->head;
		while (current_tok != nullptr)
		{
			if (current_tok != tok->items->head) {
				std::cout << ", ";
			}
			print_token(current_tok);
			current_tok = current_tok->next_tok;
		}
		std::cout << ']';
		break;
	}
	case TOK_REFRENCE: {
		refrence_token* tok = (refrence_token*)token;
		std::cout << "REF ";
		print_token(tok->value);
		break;
	}
	case TOK_IMPORT: {
		import_token* tok = (import_token*)token;
		std::cout << "IMPORT \"" << tok->path << '\"';
		break;
	}
	}
}