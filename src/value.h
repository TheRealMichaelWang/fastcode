#pragma once

#include "token.h"

#ifndef VALUE_H
#define VALUE_H

#define VALUE_TYPE_NULL 0
#define VALUE_TYPE_DOUBLE 1
#define VALUE_TYPE_CHAR 2
#define VALUE_TYPE_ARRAY 3
#define VALUE_TYPE_STRUCT 4
#define VALUE_TYPE_DELETED 5

typedef class value_array;
typedef class var_context;
typedef class unique_reference;

class value
{
public:
	char type;
	void* ptr;
	value(char type, void* ptr);
	value(double d);
	value(char c);
	value(const char* str);
	value(value_array* array);
	value();
	~value();
	void print(int indent = 0);
	unique_reference* iterate(size_t index);
	double length();
	value* clone();
	double compare(value* value);
};

class unique_reference {
public:
	var_context* parent_context;
	unique_reference* parent_refrence;
	value* value_ptr;
	unique_reference(value* value_ptr, unique_reference* parent_refrence, var_context* parent_context);
	~unique_reference();
	bool is_root_refrence();
	void set_var_ptr(value* new_ptr, bool alter_parent = true);
	value* get_value_ptr();
	void change_refrence(unique_reference* new_ref);
	bool context_check(var_context* delete_context);
	void replaceNullContext(var_context* new_context);
private:
	unique_reference* refrence_correct(unique_reference* new_parent_refrence);
};

class value_array
{
public:
	int size;
	unique_reference** collection;
	value_array(int size);
	value_array(int size, unique_reference** collection);
	//value_array(int size, value* fillValue);
	~value_array();
	bool checktype(char type);
	unique_reference* iterate(size_t index);
	value_array* clone();
	double compare(value_array* array);
	//bool has_val_ptr(value* val_ptr);
};

class structure
{
public:
	char* identifier;
	var_context* properties;
	structure(class struct_prototype* prototype, var_context* parent_context);
	structure(char* identifier, var_context* parent_context);
	~structure();
	structure* clone();
};

class var_node {
public:
	unique_reference* unique_ref;
	var_node* next;
	unsigned long hash_id;
	var_node(unsigned long hash_id, unique_reference* unique_ref);
	~var_node();
};

class var_context {
public:
	var_context* parent_context;
	var_node* head;
	var_context(var_context* parent_context);
	~var_context();
	unique_reference* declare(char* identifier, unique_reference* value);
	unique_reference* declare(unsigned long hash, unique_reference* value);
	unique_reference* push_refrence(unique_reference* refrence);
	void remove(char* identifier);
	unique_reference* searchForVal(char* identifier);
	bool has_val(char* identifier);
	bool _disposing;
};

value* applyUniaryOp(char type, unique_reference* value);
value* applyBinaryOp(char type, unique_reference* a, unique_reference* b);

#endif // !VALUE_H