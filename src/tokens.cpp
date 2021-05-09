#include "errors.h"
#include "hash.h"
#include "tokens.h"
#include "operators.h"

inline bool is_control_tok(unsigned char type) {
	return type >= 65 && type < 70;
}

inline bool is_control_tok(token* control_structure) {
	return is_control_tok(control_structure->type);
}

void destroy_value_tok(token* val_tok) {
	switch (val_tok->type)
	{
	case TOKEN_VALUE:
		delete (value_token*)val_tok;
		break;
	case TOKEN_VAR_ACCESS:
		delete (variable_access_token*)val_tok;
		break;
	case TOKEN_FUNCTION_CALL:
		delete (function_call_token*)val_tok;
		break;
	case TOKEN_BINARY_OP:
		delete (binary_operator_token*)val_tok;
		break;
	case TOKEN_UNIARY_OP:
		delete (uniary_operator_token*)val_tok;
		break;
	case TOKEN_GET_REFERENCE:
		delete (get_reference_token*)val_tok;
		break;
	case TOKEN_CREATE_ARRAY:
		delete (create_array_token*)val_tok;
		break;
	case TOKEN_CREATE_STRUCT:
		delete (create_struct_token*)val_tok;
		break;
	case TOKEN_SET:
		delete (set_token*)val_tok;
		break;
	default:
		throw ERROR_UNEXPECTED_TOKEN;
	}
}

void destroy_top_lvl_tok(token* token) {
	switch (token->type)
	{
	case TOKEN_INCLUDE:
		delete (include_token*)token;
		break;
	case TOKEN_SET:
		delete (set_token*)token;
		break;
	case TOKEN_FUNCTION_CALL:
		delete (function_call_token*)token;
		break;
	case TOKEN_RETURN:
		delete (return_token*)token;
		break;
	case TOKEN_UNIARY_OP:
		delete (uniary_operator_token*)token;
		break;
	case TOKEN_IF:
	case TOKEN_ELIF:
	case TOKEN_ELSE:
	case TOKEN_WHILE:
		delete (conditional_token*)token;
		break;
	case TOKEN_BREAK:
		delete token;
		break;
	default:
		throw ERROR_UNEXPECTED_TOKEN;
	}
}

token::token(unsigned char type) {
	this->type = type;
}

value_token::value_token(class value* value) : token(TOKEN_VALUE) {
	this->inner_value_ptr = value;
}

value_token::~value_token() {
	delete inner_value_ptr;
}

identifier_token::identifier_token(const char* identifier) : identifier_token((char*)identifier, insecure_hash(identifier), false) {

}

identifier_token::identifier_token(char* identifier, unsigned long id_hash, bool delete_id) : token(TOKEN_IDENTIFIER) {
	this->id_str_ptr = identifier;
	this->id_hash = id_hash;
	this->delete_id = delete_id;
}

identifier_token::~identifier_token() {
	if(delete_id)
		delete[] id_str_ptr;
}

variable_access_token::variable_access_token(std::list<token*> modifiers) : token(TOKEN_VAR_ACCESS) {
	this->modifiers = modifiers;
	if (this->modifiers.size() < 1)
		throw ERROR_INVALID_ACCESSOR_MODIFIERS;
	if (this->modifiers.front()->type != TOKEN_IDENTIFIER)
		throw ERROR_INVALID_ACCESSOR_MODIFIERS;
	for (auto i = this->modifiers.begin(); i != this->modifiers.end(); ++i) {
		if ((*i)->type != TOKEN_IDENTIFIER && (*i)->type != TOKEN_INDEX)
			throw ERROR_UNEXPECTED_TOKEN;
	}
	
}

variable_access_token::~variable_access_token() {
	for (auto i = this->modifiers.begin(); i != this->modifiers.end(); ++i)
	{
		token* to_delete = *i;
		if (to_delete->type == TOKEN_IDENTIFIER)
			delete (identifier_token*)to_delete;
		else if (to_delete->type == TOKEN_INDEX)
			delete (index_token*)to_delete;
		else
			throw ERROR_UNEXPECTED_TOKEN;
	}
}

index_token::index_token(token* value) : token(TOKEN_INDEX) {
	if (!is_value_tok(value))
		throw ERROR_UNEXPECTED_TOKEN;
	this->value = value;
}

index_token::~index_token() {
	destroy_value_tok(this->value);
}

get_reference_token::get_reference_token(variable_access_token* var_access) :token(TOKEN_GET_REFERENCE) {
	this->var_access = var_access;
}

get_reference_token::~get_reference_token() {
	delete var_access;
}

set_token::set_token(variable_access_token* destination, token* value, bool create_static) : token(TOKEN_SET) {
	this->destination = destination;
	this->create_static = create_static;
	if (is_value_tok(value))
		this->value = value;
	else
		throw ERROR_UNEXPECTED_TOKEN;
}

set_token::~set_token() {
	delete destination;
	destroy_value_tok(this->value);
}

function_call_token::function_call_token(identifier_token* identifier, std::list<token*> arguments) : token(TOKEN_FUNCTION_CALL){
	this->identifier = identifier;
	this->arguments = arguments;
	for (auto i = this->arguments.begin(); i != this->arguments.end(); ++i)
		if (!is_value_tok(*i))
			throw ERROR_UNEXPECTED_TOKEN;
}

function_call_token::~function_call_token() {
	delete identifier;
	for (auto i = this->arguments.begin(); i != this->arguments.end(); ++i)
		destroy_value_tok(*i);
}

return_token::return_token(token* value) : token(TOKEN_RETURN) {
	if (!is_value_tok(value))
		throw ERROR_UNEXPECTED_TOKEN;
	this->value = value;
}

return_token::~return_token() {
	destroy_value_tok(this->value);
}

conditional_token::conditional_token(unsigned char type, token* condition, std::list<token*> instructions, conditional_token* next): token(type) {
	if (!is_control_tok(type) || (type != TOKEN_ELSE && !is_value_tok(condition)))
		throw ERROR_UNEXPECTED_TOKEN;
	this->condition = condition;
	for (auto i = instructions.begin(); i != instructions.end(); ++i)
		if (!is_top_level_tok(*i))
			throw ERROR_UNEXPECTED_TOKEN;
	this->instructions = instructions;
	if (next != nullptr && type == TOKEN_ELSE)
		throw ERROR_UNEXPECTED_TOKEN;
	this->next = next;
}

conditional_token::~conditional_token() {
	if(this->condition != nullptr)
		destroy_value_tok(this->condition);
	if (this->next != nullptr)
		delete this->next;
	for (auto i = this->instructions.begin(); i != this->instructions.end(); ++i)
		destroy_top_lvl_tok(*i);
}

conditional_token* conditional_token::get_next_conditional(bool condition_success) {
	if (this->condition == nullptr) 
		return nullptr;
	switch (this->type)
	{
	case TOKEN_IF:
	case TOKEN_ELIF:
		return condition_success ? nullptr : this->next;
	case TOKEN_WHILE:
		return condition_success ? this : nullptr;
	}
	throw ERROR_UNRECOGNIZED_TOKEN;
}

create_array_token::create_array_token(std::list<token*> values) : token(TOKEN_CREATE_ARRAY){
	for (auto i = values.begin(); i != values.end(); ++i)
		if (!is_value_tok(*i))
			throw ERROR_UNEXPECTED_TOKEN;
	this->values = values;
}

create_array_token::~create_array_token() {
	for (auto i = this->values.begin(); i != this->values.end(); ++i)
		destroy_value_tok(*i);
}

create_struct_token::create_struct_token(identifier_token* prototype_identifier) : token(TOKEN_CREATE_STRUCT){
	this->prototype_identifier = prototype_identifier;
}

create_struct_token::~create_struct_token() {
	delete this->prototype_identifier;
}

function_prototype::function_prototype(identifier_token* identifier, std::list<identifier_token*> argument_identifiers, std::list<token*> tokens) : token(TOKEN_FUNC_PROTO) {
	this->identifier = identifier;
	this->argument_identifiers = argument_identifiers;
	this->tokens = tokens;
	for (auto i = this->tokens.begin(); i != this->tokens.end(); ++i)
		if (!is_top_level_tok(*i))
			throw ERROR_UNEXPECTED_TOKEN;
	
}

function_prototype::~function_prototype() {
	delete identifier;
	for (auto i = this->argument_identifiers.begin(); i != this->argument_identifiers.end(); ++i)
		delete* i;
	for (auto i = this->tokens.begin(); i != this->tokens.end(); ++i)
		destroy_top_lvl_tok(*i);
}

include_token::include_token(char* file_path) : token(TOKEN_INCLUDE) {
	this->file_path = file_path;
}

include_token::~include_token() {
	delete this->file_path;
}