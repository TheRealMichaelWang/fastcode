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
typedef class unique_refrence;

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
	void print();
	unique_refrence* iterate(size_t index);
	double length();
	value* clone();
	value* shallowClone();
	int compare(value* value);
	//bool has_val_ptr(value* val_ptr);
};

class unique_refrence {
public:
	var_context* parent_context;
	unique_refrence* parent_refrence;
	value* value_ptr;
	unique_refrence(value* value_ptr, unique_refrence* parent_refrence, var_context* parent_context);
	~unique_refrence();
	bool is_root_refrence();
	void set_var_ptr(value* new_ptr);
	value* get_var_ptr();
	void change_refrence(unique_refrence* new_ref);
	bool context_check(var_context* delete_context, bool allow_correct);
	void replaceNullContext(var_context* new_context);
};

class value_array
{
public:
	int size;
	unique_refrence** collection;
	value_array(int size);
	value_array(int size, unique_refrence** collection);
	//value_array(int size, value* fillValue);
	~value_array();
	bool checktype(char type);
	unique_refrence* iterate(size_t index);
	value_array* clone();
	value_array* shallowClone();
	int compare(value_array* array);
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
	int compare(structure* tocomp);
	structure* clone();
	structure* shallowClone(bool take_ownership = true);
};

class variable
{
public:
	char* identifier;
	unique_refrence* unique_ref;
	variable(char* identifier, class value* value, var_context* parent_context);
	variable(char* identifier, unique_refrence* unique_ref);
	variable();
	~variable();
};

class var_context
{
private:
	int allocated_size;
	void extend(int n);
public:
	var_context* parent_context;
	variable** collection;
	int size;
	var_context(var_context* parent_context);
	~var_context();
	unique_refrence* declare(char* identifier, unique_refrence* value);
	void remove(char* identifier);
	unique_refrence* searchForVal(char* identifier);
	bool has_val(char* identifier);
	//bool has_val_ptr(value* val_ptr);
};

value* applyUniaryOp(char type, unique_refrence* value);
value* applyBinaryOp(char type, unique_refrence* a, unique_refrence* b);

#endif // !VALUE_H
