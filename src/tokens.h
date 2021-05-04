#pragma once

#ifndef TOKENS_H
#define TOKENS_H

#include <vector>
#include "errors.h"
#include "value.h"


//value and accessor tokens 0-4
#define TOKEN_VALUE 0
#define TOKEN_IDENTIFIER 1

#define TOKEN_VAR_ACCESS 2
#define TOKEN_INDEX 3

#define TOKEN_GET_REFERENCE 4

//operator tokens 5 - 59
#define TOKEN_BINARY_OP 5
#define TOKEN_UNIARY_OP 6

#define STD_OP_TOK_OFFSET 7 //offset for individual operator tokens/ids

//top-level instruction tokens 60-69
//instruction tokens 61-64
#define TOKEN_SET 61
#define TOKEN_FUNCTION_CALL 62
#define TOKEN_RETURN 63
#define TOKEN_BREAK 64

//control structure tokens 65-69
#define TOKEN_IF 65
#define TOKEN_ELIF 66
#define TOKEN_ELSE 67
#define TOKEN_WHILE 68

//hardcoded create tokens
#define TOKEN_CREATE_ARRAY 70 
#define TOKEN_CREATE_STRUCT 71

#define TOKEN_STRUCT_PROTO 72
#define TOKEN_FUNC_PROTO 73

#define TOKEN_INCLUDE 74

#define MAX_TOKEN_LIMIT 75

struct token {
	unsigned char type;
	token(unsigned char type);
};

struct value_token :token {
public:
	value_token(class value* value);
	~value_token();

	inline value* get_value() {
		return this->inner_value_ptr->clone();
	}
private:
	value* inner_value_ptr;
};

struct identifier_token : token {
public:
	inline const char* get_identifier() {
		return (const char*)this->id_str_ptr;
	}
	unsigned long id_hash;
	identifier_token(char* identifier);
	identifier_token(char* identifier, unsigned long id_hash);
	~identifier_token();
private:
	char* id_str_ptr;
};

struct variable_access_token : token {
	std::vector<token*> modifiers;
	variable_access_token(std::vector<token*> modifiers);
	~variable_access_token();

	inline identifier_token* get_identifier() {
		return (identifier_token*)modifiers[0];
	}
};

struct index_token : token {
	token* value;
	index_token(token* value);
	~index_token();
};

struct get_reference_token :token {
	variable_access_token* var_access;
	get_reference_token(variable_access_token* var_access);
	~get_reference_token();
};

struct set_token :token {
	bool create_static;
	variable_access_token* destination;
	token* value;
	set_token(variable_access_token* destination, token* value, bool create_static);
	~set_token();
};

struct function_call_token :token {
	identifier_token* identifier;
	std::vector<token*> arguments;
	function_call_token(identifier_token* identifier, std::vector<token*> arguments);
	~function_call_token();
};

struct return_token :token {
	token* value;
	return_token(token* value);
	~return_token();
};

struct conditional_token :token {
	token* condition;
	std::vector<token*> instructions;
	conditional_token(unsigned char type, token* condition, std::vector<token*> instructions, conditional_token* next);
	~conditional_token();
	conditional_token* next;
	conditional_token* get_next_conditional(bool condition_val);
};

struct create_array_token :token {
	std::vector<token*> values;
	create_array_token(std::vector<token*> values);
	~create_array_token();
};

struct create_struct_token :token {
	identifier_token* prototype_identifier;
	create_struct_token(identifier_token* prototype_identifier);
	~create_struct_token();
};

struct function_prototype :token {
	identifier_token* identifier;
	std::vector<identifier_token*> argument_identifiers;
	std::vector<token*> tokens;
	function_prototype(identifier_token* identifier, std::vector<identifier_token*> argument_identifiers, std::vector<token*> tokens);
	~function_prototype();
};

struct include_token :token {
public:

	include_token(char* file_path);
	~include_token();

	inline const char* get_file_path() {
		return this->file_path;
	}
private:
	char* file_path;
};

inline bool is_top_level_tok(token* token) {
	return (token->type > 60 && token->type < 70) || token->type == TOKEN_UNIARY_OP || token->type == TOKEN_INCLUDE;
}

void destroy_top_lvl_tok(token* token);

void destroy_value_tok(token* val_tok);

inline bool is_value_tok(token* value) {
	return value->type == TOKEN_VALUE || value->type == TOKEN_VAR_ACCESS || value->type == TOKEN_FUNCTION_CALL || value->type == TOKEN_UNIARY_OP || value->type == TOKEN_BINARY_OP || value->type == TOKEN_GET_REFERENCE || value->type == TOKEN_CREATE_ARRAY || value->type == TOKEN_CREATE_STRUCT || value->type == TOKEN_SET || value->type == TOKEN_RETURN;
}

#endif // !TOKENS_H