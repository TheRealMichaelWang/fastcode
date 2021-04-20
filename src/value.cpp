#include <cmath>
#include <iostream>
#include <string.h>
#include <math.h>
#include "error.h"
#include "token.h"
#include "value.h"
#include "string.h"

value::value()
{
	this->type = VALUE_TYPE_NULL;
	this->ptr = nullptr;
}

value::value(char type, void* ptr)
{
	this->type = type;
	this->ptr = ptr;
}

value::value(double d)
{
	this->type = VALUE_TYPE_DOUBLE;
	this->ptr = new double(d);
}

value::value(char c)
{
	this->type = VALUE_TYPE_CHAR;
	this->ptr = new char(c);
}

value::value(const char* str)
{
	this->type = VALUE_TYPE_ARRAY;
	value_array* array = new value_array(strlen(str));
	for (int i = 0; i < array->size; i++)
	{
		array->collection[i] = new unique_reference(new value(str[i]), nullptr, nullptr);
	}
	this->ptr = array;
}

value::value(value_array* array)
{
	this->type = VALUE_TYPE_ARRAY;
	this->ptr = array;
}

value::~value()
{
	if (this->type == VALUE_TYPE_ARRAY) {
		value_array* val_array = (value_array*)this->ptr;
		delete val_array;
	}
	else if (this->type == VALUE_TYPE_STRUCT) {
		structure* structure = (class structure*)this->ptr;
		delete structure;
	}
	else if (this->type == VALUE_TYPE_DOUBLE) {
		double* d = (double*)this->ptr;
		delete d;
	}
	else if (this->type == VALUE_TYPE_CHAR) {
		char* c = (char*)this->ptr;
		delete c;
	}
}

void value::print(int indent)
{
	for (size_t i = 0; i < indent; i++)
	{
		std::cout << "  ";
	}
	if (this->type == VALUE_TYPE_NULL)
	{
		std::cout << "NULL";
	}
	else if (this->type == VALUE_TYPE_DOUBLE)
	{
		std::cout << *((double*)this->ptr);
	}
	else if (this->type == VALUE_TYPE_CHAR)
	{
		std::cout << *((char*)this->ptr);
	}
	else if (this->type == VALUE_TYPE_ARRAY)
	{
		value_array* array = (value_array*)this->ptr;
		if (array->checktype(VALUE_TYPE_CHAR))
		{
			for (int i = 0; i < array->size; i++)
			{
				std::cout << *((char*)array->collection[i]->get_value_ptr()->ptr);
			}
		}
		else
		{
			std::cout << '[';
			for (int i = 0; i < array->size; i++)
			{
				array->collection[i]->get_value_ptr()->print();
				if (i < array->size - 1)
				{
					std::cout << ",";
				}
			}
			std::cout << ']';
		}
	}
	else if (this->type == VALUE_TYPE_STRUCT) {
		structure* st = (structure*)this->ptr;
		std::cout << "Type ID: " << st->identifier;
		var_node* current_node = st->properties->head;
		while (current_node != nullptr) {
			std::cout << std::endl;
			for (size_t j = 0; j < indent + 1; j++)
			{
				std::cout << "  ";
			}
			current_node->unique_ref->get_value_ptr()->print(indent + 2);
			current_node = current_node->next;
		}
	}
}

//TODO: Modify for implementation of custom iterate operators
unique_reference** value::iterate(size_t index)
{
	if (type == VALUE_TYPE_ARRAY)
	{
		value_array* array = (value_array*)ptr;
		return array->iterate(index);
	}
	throw ERROR_MUST_HAVE_ARRAY_TYPE;
}

double value::length()
{
	if (type == VALUE_TYPE_ARRAY)
	{
		value_array* array = (value_array*)ptr;
		return (double)array->size;
	}
	return 0.0;
}

value* value::clone()
{
	if (type == VALUE_TYPE_NULL){
		return new value();
	}
	else if (type == VALUE_TYPE_CHAR){
		return new value(*(char*)this->ptr);
	}
	else if (type == VALUE_TYPE_DOUBLE) {
		return new value(*(double*)this->ptr);
	}
	else if (type == VALUE_TYPE_ARRAY){
		return new value(VALUE_TYPE_ARRAY, ((value_array*)this->ptr)->clone());
	}
	else if (type == VALUE_TYPE_STRUCT) {
		return new value(VALUE_TYPE_STRUCT, ((structure*)this->ptr)->clone());
	}
	throw ERROR_MUST_HAVE_ARRAY_TYPE;
}

//TODO: Implement struct compare
double value::compare(value* value)
{
	if (value->type == this->type)
	{
		switch (type)
		{
		case VALUE_TYPE_CHAR:
			return ((double)*(char*)this->ptr - (double)*(char*)value->ptr);
		case VALUE_TYPE_DOUBLE:
			return *(double*)this->ptr - *(double*)value->ptr;
		case VALUE_TYPE_ARRAY: {
			return ((value_array*)value->ptr)->compare((value_array*)this->ptr);
		}
		case VALUE_TYPE_STRUCT: {
			return value->ptr == value->ptr;
		}
		case VALUE_TYPE_NULL: {
			return 0;
		}
		}
	}
	return 1;
}

unique_reference::unique_reference(value* value_ptr, unique_reference* parent_refrence, var_context* parent_context)
{
	this->value_ptr = value_ptr;
	if (parent_refrence != nullptr) {
		change_refrence(parent_refrence);
	}
	else {
		this->parent_refrence = nullptr;
	}
	this->parent_context = parent_context;
}

unique_reference::~unique_reference()
{
	if (this->is_root_reference())
	{
		delete value_ptr;
	}
}

void unique_reference::set_var_ptr(value* new_ptr, bool alter_parent)
{
	if (this->parent_refrence != nullptr) {
		//reference_correct();
		this->value_ptr = new_ptr;
		if (alter_parent) {
			delete parent_refrence->value_ptr;
			this->parent_refrence->value_ptr = new_ptr;
		}
	}
	else {
		delete this->value_ptr;
		this->value_ptr = new_ptr;
	}
}

inline void unique_reference::change_refrence(unique_reference* new_ref) {
	this->parent_refrence = new_ref;
	reference_correct();
}

inline bool unique_reference::is_root_reference()
{
	return (parent_refrence == nullptr);
}

value* unique_reference::get_value_ptr() {
	if (this->parent_refrence == nullptr) {
		return value_ptr;
	}
	//this->parent_refrence = reference_correct(this->parent_refrence);
	return parent_refrence->value_ptr;
}

bool unique_reference::context_check(var_context* del_context, var_context* replace_context) {
	if (this->parent_context == del_context) {
		this->parent_context = replace_context;
	}
	if (!is_root_reference()) {
		//this->parent_refrence = reference_correct(this->parent_refrence);
		if (parent_refrence->parent_context == del_context) {
			parent_refrence->parent_context = replace_context;
			replace_context->push_refrence(parent_refrence);
		}
	}
	value* val_ptr = get_value_ptr();
	if (val_ptr->type == VALUE_TYPE_ARRAY) {
		value_array* array = (value_array*)val_ptr->ptr;
		for (size_t i = 0; i < array->size; i++)
		{
			array->collection[i]->context_check(del_context, replace_context);
		}
	}
	else if (val_ptr->type == VALUE_TYPE_STRUCT) {
		structure* structure = (class structure*)val_ptr->ptr;
		if (structure->properties->parent_context == del_context) {
			structure->properties->parent_context = replace_context;
		}
		var_node* current_node = structure->properties->head;
		while (current_node != nullptr)
		{
			current_node->unique_ref->context_check(del_context, replace_context);
			current_node = current_node->next;
		}
	}
	return true;
}

void unique_reference::replaceNullContext(var_context* new_context) {
	if (this->parent_context == nullptr) {
		this->parent_context = new_context;
	}
	value* val_ptr = get_value_ptr();
	if (val_ptr->type == VALUE_TYPE_ARRAY) {
		value_array* array = (value_array*)val_ptr->ptr;
		for (size_t i = 0; i < array->size; i++)
		{
			array->collection[i]->replaceNullContext(new_context);	
		}
	}
	else if (val_ptr->type == VALUE_TYPE_STRUCT) {
		structure* structure = (class structure*)val_ptr->ptr;
		if (structure->properties->parent_context == nullptr) {
			structure->properties->parent_context = new_context;
		}
		var_node* current_node = structure->properties->head;
		while (current_node != nullptr) {
			current_node->unique_ref->replaceNullContext(new_context);
			current_node = current_node->next;
		}
	}
}

void unique_reference::reference_correct() {
	unique_reference* current = this;
	while (!current->is_root_reference()) {
		current = current->parent_refrence;
	}
	if (this != current) {
		parent_refrence = current;
	}
}

value_array::value_array(int size)
{
	this->size = size;
	this->collection = new unique_reference*[size];
}

value_array::value_array(int size, unique_reference** collection)
{
	this->size = size;
	this->collection = collection;
}

value_array::~value_array()
{
	for (size_t i = 0; i < size; i++)
	{
		delete collection[i];
	}
	delete[] collection;
}

bool value_array::checktype(char type)
{
	for (int i = 0; i < this->size; i++)
	{
		if (this->collection[i]->get_value_ptr()->type != type)
		{
			return false;
		}
	}
	return true;
}
unique_reference** value_array::iterate(size_t index)
{
	if (index > this->size)
	{
		throw ERROR_INDEX_OUT_OF_RANGE;
	}
	return &collection[index];
}

value_array* value_array::clone()
{
	value_array* new_array = new value_array(this->size);
	for (size_t i = 0; i < this->size; i++)
	{
		new_array->collection[i] = new unique_reference(collection[i]->get_value_ptr()->clone(), nullptr, nullptr);
	}
	return new_array;
}

double value_array::compare(value_array* array)
{
	if (array->size != this->size)
	{
		return (double)this->size - (double)array->size;
	}
	for (size_t i = 0; i < this->size; i++)
	{
		int comp = array->collection[i]->get_value_ptr()->compare(collection[i]->get_value_ptr());
		if (comp != 0)
		{
			return comp;
		}
	}
	return 0;
}

structure::structure(class struct_prototype* prototype)
{
	this->identifier = prototype->identifier->identifier;
	this->properties = new var_context(nullptr);
	struct token* current_tok = prototype->properties->head;
	while (current_tok != nullptr) {
		identifier_token* prop = (identifier_token*)current_tok;
		properties->declare(prop->identifier, new unique_reference(new value(), nullptr, nullptr));
		current_tok = current_tok->next_tok;
	}
}

structure::structure(char* identifier, var_context* parent_context) {
	this->identifier = identifier;
	this->properties = new var_context(parent_context);
}

structure::~structure()
{
	delete properties;
}

structure* structure::clone()
{
	structure* structure = new class structure(identifier, this->properties->parent_context == nullptr ? this->properties : this->properties->parent_context);
	/*for (size_t i = 0; i < this->properties->size; i++)
	{
		structure->properties->declare(this->properties->collection[i]->identifier, new unique_reference(this->properties->collection[i]->unique_ref->get_value_ptr()->clone(), nullptr, nullptr));
	}*/
	var_node* current_node = this->properties->head;
	while (current_node != nullptr) {
		structure->properties->declare(current_node->hash_id, new unique_reference(current_node->unique_ref->get_value_ptr()->clone(), nullptr, nullptr));
		current_node = current_node->next;
	}
	return structure;
}

var_node::var_node(unsigned long hash_id, unique_reference* unique_ref) {
	this->hash_id = hash_id;
	this->unique_ref = unique_ref;
	this->next = nullptr;
}

var_node::~var_node() {
	//delete this->unique_ref;
}

var_context::var_context(var_context* parent_context)
{
	this->parent_context = parent_context;
	this->head = nullptr;
}

var_context::~var_context()
{
	var_node* current_node = head;
	while (current_node != nullptr) {
		if (parent_context == nullptr) {
			if (current_node->unique_ref->parent_context == this) {
				delete current_node->unique_ref;
			}
		}
		else if (current_node->unique_ref->parent_context == parent_context) {
			delete current_node->unique_ref;
		}
		var_node* to_delete = current_node;
		current_node = current_node->next;
		delete to_delete;
	}
}

inline unique_reference** var_context::declare(char* identifier, unique_reference* value)
{
	return declare(dj2b(identifier), value);
}

unique_reference** var_context::declare(unsigned long hash_id, unique_reference* value) {
	if (head == nullptr) {
		head = new var_node(hash_id, value);
		definition_map[hash_id % STD_HASH_MAP_LIMIT] = head;
		return &head->unique_ref;
	}
	var_node* old_head = head;
	head = new var_node(hash_id, value);
	definition_map[hash_id % STD_HASH_MAP_LIMIT] = head;
	head->next = old_head; //there are no requirements to preseve the original order
	return &head->unique_ref;
}

inline unique_reference** var_context::push_refrence(unique_reference* refrence) {
	return declare(-1, refrence); //i don't give a fuck whether it's an unsigned int, the hash would be way to high for a normal string to generate either way.
}

void var_context::remove(char* identifier, bool delete_ref)
{
	var_node* parent = nullptr;
	var_node* current_node = head;
	unsigned long id_hash = dj2b(identifier);
	while (current_node != nullptr)
	{
		if (current_node->hash_id == id_hash) {
			if (delete_ref) { 
				delete current_node->unique_ref; 
			}
			if (parent == nullptr) {
				head = head->next;
			}
			else {
				parent->next = current_node->next;
			}
			definition_map[current_node->hash_id % STD_HASH_MAP_LIMIT] = NULL;
			delete current_node;
			return;
		}
		parent = current_node;
		current_node = current_node->next;
	}
	//TODO: Add error handling
	throw ERROR_VAR_NOT_FOUND;
}

unique_reference** var_context::searchForVal(char* identifier)
{
	/*var_node* current_node = head;
	unsigned long id_hash = dj2b(identifier);
	while (current_node != nullptr) {
		if (current_node->hash_id == id_hash) {
			return &current_node->unique_ref;
		}
		current_node = current_node->next;
	}
	throw ERROR_VAR_NOT_FOUND;*/
	unsigned long id = dj2b(identifier) % STD_HASH_MAP_LIMIT;
	if (definition_map[id] != NULL) {
		return &definition_map[id]->unique_ref;
	}
	throw ERROR_VAR_NOT_FOUND;
}

bool var_context::has_val(char* identifier)
{
	/*var_node* current_node = head;
	unsigned long id_hash = dj2b(identifier);
	while (current_node != nullptr) {
		if (current_node->hash_id == id_hash) {
			return true;
		}
		current_node = current_node->next;
	}
	return false;*/
	return definition_map[dj2b(identifier) % STD_HASH_MAP_LIMIT] != NULL;
}