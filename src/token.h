#pragma once
#include "value.h"

#ifndef TOKEN_H
#define TOKEN_H

//syntax tokens
#define TOK_GLOBAL 0
#define TOK_IDENTIFIER 1
#define TOK_VALUE 2
#define TOK_FUNCTION 3
#define TOK_STRUCT 4
#define TOK_BREAK 5
#define TOK_RETURN 6
#define TOK_IMPORT 7
#define TOK_NEW 8
#define TOK_IN 9
#define TOK_IF 10
#define TOK_ELSE 11
#define TOK_ELIF 12
#define TOK_FOR 13
#define TOK_WHILE 14
#define TOK_COMMA 15
#define TOK_OPEN_PAREN 16
#define TOK_CLOSE_PAREN 17
#define TOK_OPEN_BRACKET 18
#define TOK_CLOSE_BRACKET 19
#define TOK_OPEN_BRACE 20
#define TOK_CLOSE_BRACE 21
#define TOK_PERIOD 22
#define TOK_PLUS 23
#define TOK_MINUS 24
#define TOK_SLASH 25
#define TOK_ASTERISK 26
#define TOK_MODULOUS 27
#define TOK_CARET 28
#define TOK_SET 29
#define TOK_EQUALS 30
#define TOK_NOT_EQUAL 31
#define TOK_LESS 32
#define TOK_MORE 33
#define TOK_LESS_EQUAL 34
#define TOK_MORE_EQUAL 35
#define TOK_QUICK_BLOCK 36
#define TOK_OR 37
#define TOK_AND 38
#define TOK_NOT 39
#define TOK_REFRENCE 40
#define TOK_INCRIMENT 41
#define TOK_DECRIMENT 42
#define TOK_EOF 43

//these are non-syntax instruction tokens //starts at 0x32
#define TOK_SET_VARIABLE 50
#define TOK_BINARY_OP 51
#define TOK_UNIARY_OP 52
#define TOK_CALL_FUNCTION 53
#define TOK_NEW_STRUCT 54
#define TOK_FUNCTION_PROTO 55
#define TOK_STRUCT_PROTO 56
#define TOK_PROPERTY 57
#define TOK_INDEX 58
#define TOK_CREATE_ARRAY 59

struct token
{
public:
	token* next_tok;
	char type;
	token(char type);
	token();
};

class token_set
{
public:
	unsigned int size;
	token* head;
	token* tail;
	token_set();
	~token_set();
	void push(token* tok);
};

struct value_token : public token
{
public:
	class value* value;
	value_token(class value* value);
	~value_token();
};

struct identifier_token : public token
{
public:
	token_set* modifiers;
	char* identifier = nullptr;
	identifier_token(char* identifier); 
	identifier_token(char* identifier, token_set* modifiers);
	~identifier_token();
	inline bool hasModifiers();
};

struct set_variable_token:public token
{
public:
	bool global;
	identifier_token* identifier;
	token* set_tok;
	set_variable_token(identifier_token* identifier, token* set_tok, bool global);
	~set_variable_token();
};

struct function_call_token :public token
{
public:
	identifier_token* identifier;
	token_set* arguments;
	function_call_token(identifier_token* identifier, token_set* arguments);
	~function_call_token();
};

struct function_prototype :public token
{
public:
	identifier_token* identifier;
	token_set* params;
	token_set* body;
	function_prototype(identifier_token* identifier, token_set* params, token_set* body);
	~function_prototype();
};

struct struct_prototype : public token
{
public:
	identifier_token* identifier;
	token_set* properties;
	struct_prototype(identifier_token* identifier, token_set* properties);
	~struct_prototype();
};

struct property_token : public token
{
public:
	char* property_identifier = nullptr;
	property_token(char* property_identifier);
	~property_token();
};

struct indexer_token:public token
{
public:
	token* index;
	indexer_token(token* index);
	~indexer_token();
};

struct uniary_operator_token:public token
{
public:
	char op_type;
	token* value;
	uniary_operator_token(char op_type, token* value);
	~uniary_operator_token();
};

struct binary_operator_token:public token
{
public:
	char op_type;
	token* left;
	token* right;
	binary_operator_token(char op_type, token* left, token* right);
	~binary_operator_token();
};

struct conditional_token :public token
{
public:
	token* condition;
	token_set* instructions;
	conditional_token* next_condition; //a condition to check should the current fail
	conditional_token(char tok_type, token* condition, token_set* instruction, conditional_token* next_condition);
	~conditional_token();
};

struct for_token:public token
{
public:
	token* to_iterate;
	identifier_token* iterator_identifier;
	token_set* instructions;
	for_token(token* to_iterate, identifier_token* iterator_identifier, token_set* instruction);
	~for_token();
};

struct create_struct:public token
{
public:
	identifier_token* identifier;
	create_struct(identifier_token* identifer);
	~create_struct();
};

struct return_token:public token
{
public:
	token* ret_tok;
	return_token();
	return_token(token* return_tok);
	~return_token();
};

struct create_array :public token
{
public:
	token_set* items;
	create_array(token_set* items);
	~create_array();
};

struct refrence_token :public token
{
public:
	token* value;
	refrence_token(token* value);
	~refrence_token();
};

struct import_token : public token
{
public:
	char* path;
	import_token(char* path);
	~import_token();
};

void generic_destroy_token(token* tok);
bool is_op_token(char tok_type);
char get_op_prior(char op); //returns the operator priority in PEMDAS
void print_token(token* token);

#endif TOKEN_H