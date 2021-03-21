#include <cmath>
#include <iostream>
#include <string.h>
#include <math.h>
#include "error.h"
#include "token.h"
#include "value.h"

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
		array->collection[i] = new unique_refrence(new value(str[i]), nullptr, nullptr);
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
				std::cout << *((char*)array->collection[i]->get_var_ptr()->ptr);
			}
		}
		else
		{
			std::cout << '[';
			for (int i = 0; i < array->size; i++)
			{
				array->collection[i]->get_var_ptr()->print();
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
		for (size_t i = 0; i < st->properties->size; i++)
		{
			std::cout << std::endl;
			for (size_t j = 0; j < indent + 1; j++)
			{
				std::cout << "  ";
			}
			std::cout << st->properties->collection[i]->identifier << ":" << std::endl;
			st->properties->collection[i]->unique_ref->get_var_ptr()->print(indent + 2);
		}
	}
}

unique_refrence* value::iterate(size_t index)
{
	if (type == VALUE_TYPE_ARRAY)
	{
		value_array* array = (value_array*)ptr;
		return array->iterate(index);
	}
	throw ERROR_CANNOT_ITERATE_TYPE;
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
	throw ERROR_CANNOT_ITERATE_TYPE;
}

value* value::shallowClone() {
	if (type == VALUE_TYPE_NULL) {
		return new value();
	}
	else if (type == VALUE_TYPE_CHAR) {
		return new value(*(char*)this->ptr);
	}
	else if (type == VALUE_TYPE_DOUBLE) {
		return new value(*(double*)this->ptr);
	}
	else if (type == VALUE_TYPE_ARRAY) {
		return new value(VALUE_TYPE_ARRAY, ((value_array*)this->ptr)->shallowClone());
	}
	else if (type == VALUE_TYPE_STRUCT) {
		return new value(VALUE_TYPE_STRUCT, ((structure*)this->ptr)->shallowClone());
	}
	throw ERROR_CANNOT_ITERATE_TYPE;
}

//TODO: Implement struct compare
double value::compare(value* value)
{
	if (value->type == this->type)
	{
		switch (type)
		{
		case VALUE_TYPE_CHAR:
			return (double)*(char*)this->ptr - (double)*(char*)value->ptr;
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

bool value::contains(value* key) {
	if (this == key) {
		return true;
	}
	if (type == VALUE_TYPE_ARRAY) {
		value_array* array = (value_array*)ptr;
		for (size_t i = 0; i < array->size; i++)
		{
			if (array->collection[i]->get_var_ptr() == key) {
				return true;
			}
		}
	}
	else if (type == VALUE_TYPE_STRUCT) {
		structure* structure = (class structure*)ptr;
		for (size_t i = 0; i < structure->properties->size; i++)
		{
			if (structure->properties->collection[i]->unique_ref->get_var_ptr() == key) {
				return true;
			}
		}
	}
	return false;
}

bool value::check_delete(value* to_delete) {
	if (to_delete->contains(this)) {
		return false;
	}
	if (type == VALUE_TYPE_ARRAY) {
		value_array* array = (value_array*)ptr;
		for (size_t i = 0; i < array->size; i++)
		{
			if (array->collection[i]->get_var_ptr()->check_delete(to_delete)) {
				//array->collection[i] =
			}
		}
	}
	return true;
}

unique_refrence::unique_refrence(value* value_ptr, unique_refrence* parent_refrence, var_context* parent_context)
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

unique_refrence::~unique_refrence()
{
	if (this->is_root_refrence())
	{
		delete value_ptr;
	}
}

void unique_refrence::set_var_ptr(value* new_ptr, bool alter_parent)
{
	if (this->parent_refrence != nullptr) {
		this->parent_refrence = refrence_check(this->parent_refrence);
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

void unique_refrence::change_refrence(unique_refrence* new_ref) {
	this->parent_refrence = refrence_check(new_ref);
}

bool unique_refrence::is_root_refrence()
{
	return (parent_refrence == nullptr);
}

value* unique_refrence::get_var_ptr() {
	if (this->parent_refrence == nullptr) {
		return value_ptr;
	}
	this->parent_refrence = refrence_check(this->parent_refrence);
	return parent_refrence->value_ptr;
}

bool unique_refrence::context_check(var_context* del_context) {
	if (this->parent_context == del_context) {
		this->parent_context = nullptr;
	}
	if (this->parent_refrence != nullptr) {
		this->parent_refrence = refrence_check(this->parent_refrence);
		if (parent_refrence->parent_context == del_context) {
			parent_refrence->parent_context = nullptr;
		}
	}
	value* val_ptr = get_var_ptr();
	if (val_ptr->type == VALUE_TYPE_ARRAY) {
		value_array* array = (value_array*)val_ptr->ptr;
		for (size_t i = 0; i < array->size; i++)
		{
			array->collection[i]->context_check(del_context);
		}
	}
	else if (val_ptr->type == VALUE_TYPE_STRUCT) {
		structure* structure = (class structure*)val_ptr->ptr;
		if (structure->properties->parent_context == del_context) {
			structure->properties->parent_context = nullptr;
			for (size_t i = 0; i < structure->properties->size; i++)
			{
				structure->properties->collection[i]->unique_ref->parent_context = nullptr;
			}
		}
		else {
			for (size_t i = 0; i < structure->properties->size; i++)
			{
				structure->properties->collection[i]->unique_ref->context_check(del_context);
			}
		}
	}
	return true;
	/*unique_refrence* current_ref = this;
	unique_refrence* last_good_ref = nullptr;
	while (current_ref->parent_refrence != nullptr)
	{
		if (current_ref->parent_context != del_context) {
			last_good_ref = current_ref;
		}
		current_ref = current_ref->parent_refrence;
	}
	if (current_ref->parent_context == del_context) {
		if (last_good_ref == nullptr) {
			return false;
		}
		else {
			last_good_ref->value_ptr = current_ref->value_ptr->shallowClone();
			last_good_ref->parent_refrence = nullptr;
		}
	}
	else if (last_good_ref == nullptr) {
		last_good_ref = current_ref;
	}
	value* val_ptr = current_ref->value_ptr;
	if (val_ptr->type == VALUE_TYPE_ARRAY) {
		value_array* array = (value_array*)val_ptr->ptr;
		for (size_t i = 0; i < array->size; i++)
		{
			if (!array->collection[i]->context_check(del_context)) {
				unique_refrence* old_ref = array->collection[i];
				array->collection[i] = new unique_refrence(array->collection[i]->value_ptr->shallowClone(), nullptr, nullptr);
				if (old_ref->parent_context != del_context) {
					delete old_ref;
				}
				if (!array->collection[i]->context_check(del_context))
					return false;
			}
		}
	}
	else if (val_ptr->type == VALUE_TYPE_STRUCT) {
		structure* structure = (class structure*)val_ptr->ptr;
		if (structure->properties->parent_context == del_context) {
			structure->properties->parent_context = nullptr;
		}
		for (size_t i = 0; i < structure->properties->size; i++)
		{
			if (!structure->properties->collection[i]->unique_ref->context_check(del_context)) {
				
				unique_refrence* old_ref = structure->properties->collection[i]->unique_ref;
				structure->properties->collection[i]->unique_ref = new unique_refrence(structure->properties->collection[i]->unique_ref->value_ptr->shallowClone(), nullptr, nullptr);
				if (old_ref->parent_context != del_context) {
					delete old_ref;
				}
				if (!structure->properties->collection[i]->unique_ref->context_check(del_context))
					return false;
			}
		}
	}
	return true;*/
}

void unique_refrence::replaceNullContext(var_context* new_context) {
	if (this->parent_refrence != nullptr) {
		parent_refrence->replaceNullContext(new_context);
	}
	if (this->parent_context == nullptr) {
		this->parent_context = new_context;
	}

	value* val_ptr = get_var_ptr();
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
		for (size_t i = 0; i < structure->properties->size; i++)
		{
			structure->properties->collection[i]->unique_ref->replaceNullContext(new_context);
		}
	}
}

unique_refrence* unique_refrence::refrence_check(unique_refrence* new_parent) {
	unique_refrence* current = new_parent;
	while (!current->is_root_refrence()) {
		current = current->parent_refrence;
	}
	return current;
}

value_array::value_array(int size)
{
	this->size = size;
	this->collection = new unique_refrence*[size];
}

value_array::value_array(int size, unique_refrence** collection)
{
	this->size = size;
	this->collection = collection;
}

//value_array::value_array(int size, value* fillValue)
//{
//	this->size = size;
//	this->collection = new unique_refrence*[size];
//	for (size_t i = 0; i < size; i++)
//	{
//		this->collection[i] = new unique_refrence(fillValue->clone(), nullptr, nullptr);
//	}
//}

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
		if (this->collection[i]->get_var_ptr()->type != type)
		{
			return false;
		}
	}
	return true;
}

//bool value_array::has_val_ptr(value* ptr)
//{
//	for (size_t i = 0; i < this->size; i++)
//	{
//		if (this->collection[i]->value_ptr->has_val_ptr(ptr))
//		{
//			return true;
//		}
//	}
//	return false;
//}

unique_refrence* value_array::iterate(size_t index)
{
	if (index > this->size)
	{
		throw ERROR_INDEX_OUT_OF_RANGE;
	}
	return collection[index];
}

value_array* value_array::clone()
{
	value_array* new_array = new value_array(this->size);
	for (size_t i = 0; i < this->size; i++)
	{
		new_array->collection[i] = new unique_refrence(collection[i]->get_var_ptr()->clone(), nullptr, nullptr);
	}
	return new_array;
}

value_array* value_array::shallowClone(bool take_ownership) {
	value_array* new_array = new value_array(this->size);
	for (size_t i = 0; i < size; i++)
	{
		if (take_ownership) {
			new_array->collection[i] = new unique_refrence(this->collection[i]->get_var_ptr(), nullptr, nullptr);
			this->collection[i]->change_refrence(new_array->collection[i]);
		}
		else {
			new_array->collection[i] = new unique_refrence(this->collection[i]->get_var_ptr(), new_array->collection[i], nullptr);
		}
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
		int comp = array->collection[i]->get_var_ptr()->compare(collection[i]->get_var_ptr());
		if (comp != 0)
		{
			return comp;
		}
	}
	return 0;
}

structure::structure(class struct_prototype* prototype, var_context* parent_context)
{
	this->identifier = prototype->identifier->identifier;
	this->properties = new var_context(parent_context);
	struct token* current_tok = prototype->properties->head;
	while (current_tok != nullptr) {
		identifier_token* prop = (identifier_token*)current_tok;
		properties->declare(prop->identifier, new unique_refrence(new value(), nullptr, parent_context));
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

int structure::compare(structure* tocomp)
{
	if (strcmp(identifier, tocomp->identifier) != 0)
	{
		return strcmp(identifier, tocomp->identifier);
	}
	return this == tocomp; //ptr comp
}

structure* structure::clone()
{
	structure* structure = new class structure(identifier, this->properties->parent_context == nullptr ? this->properties : this->properties->parent_context);
	for (size_t i = 0; i < this->properties->size; i++)
	{
		structure->properties->declare(this->properties->collection[i]->identifier, new unique_refrence(this->properties->collection[i]->unique_ref->get_var_ptr()->clone(), nullptr, nullptr));
	}
	return structure;
}

structure* structure::shallowClone(bool take_owenership) {
	structure* structure = new class structure(identifier, nullptr);
	for (size_t i = 0; i < this->properties->size; i++)
	{
		if (take_owenership) {
			structure->properties->declare(properties->collection[i]->identifier, this->properties->collection[i]->unique_ref);
			//structure->properties->collection[i]->unique_ref->parent_context = nullptr;
		}
		else {
			if (this->properties->collection[i]->unique_ref->is_root_refrence()) {
				structure->properties->declare(this->properties->collection[i]->identifier, new unique_refrence(this->properties->collection[i]->unique_ref->get_var_ptr(), this->properties->collection[i]->unique_ref, nullptr));
			}
			else {
				structure->properties->declare(this->properties->collection[i]->identifier, new unique_refrence(this->properties->collection[i]->unique_ref->get_var_ptr(), this->properties->collection[i]->unique_ref->parent_refrence, nullptr));
			}
		}
	}
	return structure;
}

variable::variable(char* identifier, class value* value, var_context* parent_context)
{
	this->identifier = identifier;
	this->unique_ref = new unique_refrence(value, nullptr, parent_context);
}

variable::variable(char* identifier, unique_refrence* unique_ref)
{
	this->identifier = identifier;
	this->unique_ref = unique_ref;
}

variable::variable()
{
	this->identifier = nullptr;
	this->unique_ref = nullptr;
}

variable::~variable()
{
	//delete unique_ref;
}

var_context::var_context(var_context* parent_context)
{
	allocated_size = 10; 
	size = 0;
	this->collection = new variable*[allocated_size];
	this->parent_context = parent_context;
}

void dbg() {}

var_context::~var_context()
{
	for (size_t i = 0; i < size; i++)
	{
		if (parent_context == nullptr) {
			if (collection[i]->unique_ref->parent_context == this) {
				delete collection[i]->unique_ref;
			}
		}
		else if (collection[i]->unique_ref->parent_context == parent_context) {
			delete collection[i]->unique_ref;
		}
		delete collection[i];
	}
	delete[] collection;
}

void var_context::extend(int n)
{
	variable** old_vars = this->collection;
	collection = new variable*[allocated_size + n];
	allocated_size += n;
	for (size_t i = 0; i < size; i++)
	{
		collection[i] = old_vars[i];
	}
	delete[] old_vars;
}

unique_refrence* var_context::declare(char* identifier, unique_refrence* value)
{
	if (size == allocated_size)
	{
		extend(5);
	}
	if (has_val(identifier))
	{
		throw ERROR_VARIABLE_ALREADY_DEFINED;
	}
	collection[size++] = new variable(identifier, value);
	return collection[size - 1]->unique_ref;
}

void var_context::remove(char* identifier)
{
	bool found = false;
	for (size_t i = 0; i < size; i++)
	{
		if (strcmp(collection[i]->identifier, identifier) == 0)
		{
			delete collection[i]->unique_ref;
			delete collection[i];
			found = true;
		}
		if (found && i < size - 1)
		{
			collection[i] = collection[i + 1];
		}
	}
	size--;
}

unique_refrence* var_context::searchForVal(char* identifier)
{
	for (size_t i = 0; i < size; i++)
	{
		if (strcmp(collection[i]->identifier, identifier) == 0)
		{
			return collection[i]->unique_ref;
		}
	}
	throw ERROR_NOT_IN_VAR_CONTEXT;
}

bool var_context::has_val(char* identifier)
{
	for (size_t i = 0; i < size; i++)
	{
		if (strcmp(collection[i]->identifier, identifier) == 0)
		{
			return true;
		}
	}
	return false;
}

//bool var_context::has_val_ptr(value* ptr)
//{
//	for (size_t i = 0; i < size; i++)
//	{
//		if (collection[i]->unique_ref->value_ptr->has_val_ptr(ptr))
//		{
//			return true;
//		}
//	}
//	return false;
//}

//void refrence_outflow::extend(int n) {
//	unique_refrence** old_vars = this->references;
//	references = new unique_refrence * [allocated_size + n];
//	allocated_size += n;
//	for (size_t i = 0; i < size; i++)
//	{
//		references[i] = old_vars[i];
//	}
//	delete[] old_vars;
//}
//
//refrence_outflow::refrence_outflow() {
//	allocated_size = 10; 
//	size = 0;
//	this->references = new unique_refrence *[allocated_size];
//}
//
//refrence_outflow::~refrence_outflow() {
//	for (size_t i = 0; i < size; i++)
//	{
//		delete references[i];
//	}
//	delete[] references;
//}
//
//void refrence_outflow::push(unique_refrence* refrences) {
//	if (size == allocated_size)
//	{
//		extend(5);
//	}
//	this->references[size++] = refrences;
//}

value* applyUniaryOp(char type, unique_refrence* value)
{
	switch (type)
	{
	case TOK_MINUS: {
		if (value->get_var_ptr()->type != VALUE_TYPE_DOUBLE)
		{
			throw ERROR_MUST_HAVE_DOUBLE_TYPE;
		}
		return new class value(-(*(double*)value->get_var_ptr()->ptr));
	}
	case TOK_NOT: {
		if (value->get_var_ptr()->type != VALUE_TYPE_DOUBLE)
		{
			throw ERROR_MUST_HAVE_DOUBLE_TYPE;
		}
		double dval = *(double*)value->get_var_ptr()->ptr;
		if (dval == 0.0) {
			return new class value(1.0);
		}
		else {
			return new class value(0.0);
		}
	}
	case TOK_INCRIMENT: {
		if (value->get_var_ptr()->type != VALUE_TYPE_DOUBLE)
		{
			throw ERROR_MUST_HAVE_DOUBLE_TYPE;
		}
		class value* old = value->get_var_ptr()->clone();
		*((double*)value->get_var_ptr()->ptr) = *((double*)value->get_var_ptr()->ptr) + 1;
		return old;
	}
	case TOK_DECRIMENT: {
		if (value->get_var_ptr()->type != VALUE_TYPE_DOUBLE)
		{
			throw ERROR_MUST_HAVE_DOUBLE_TYPE;
		}
		class value* old = value->get_var_ptr()->clone();
		*((double*)value->get_var_ptr()->ptr) = *((double*)value->get_var_ptr()->ptr) - 1;
		return old;
	}
	default:
		throw ERROR_UNEXPECTED_TOK;
	}
}

value* applyBinaryOp(char type, unique_refrence* a, unique_refrence* b)
{
	if (a->get_var_ptr()->type != b->get_var_ptr()->type)
	{
		switch (type)
		{
		case TOK_EQUALS:
			return new value(0.0);
		case TOK_NOT_EQUAL:
			return new value(1.0);
		}
		throw ERROR_INCOMPATIBLE_VALUE_TYPES;
	}
	switch (type)
	{
	case TOK_EQUALS: {
		return a->get_var_ptr()->compare(b->get_var_ptr()) == 0.0 ? new value(1.0) : new value(0.0);
	}
	case TOK_NOT_EQUAL: {
		return a->get_var_ptr()->compare(b->get_var_ptr()) == 0.0 ? new value(0.0) : new value(1.0);
	}
	case TOK_MORE: {
		return a->get_var_ptr()->compare(b->get_var_ptr()) > 0.0 ? new value(1.0) : new value(0.0);
	}
	case TOK_LESS: {
		return a->get_var_ptr()->compare(b->get_var_ptr()) < 0.0 ? new value(1.0) : new value(0.0);
	}
	case TOK_MORE_EQUAL: {
		return a->get_var_ptr()->compare(b->get_var_ptr()) >= 0.0 ? new value(1.0) : new value(0.0);
	}
	case TOK_LESS_EQUAL: {
		return a->get_var_ptr()->compare(b->get_var_ptr()) <= 0.0 ? new value(1.0) : new value(0.0);
	}
	case TOK_OR: {
		if (a->get_var_ptr()->type == VALUE_TYPE_DOUBLE)
		{
			double a_double = *(double*)a->get_var_ptr()->ptr;
			double b_double = *(double*)b->get_var_ptr()->ptr;
			return (a_double != 0) || (b_double != 0) ? new value(1.0) : new value(0.0);
		}
		throw ERROR_MUST_HAVE_DOUBLE_TYPE;
	}
	case TOK_AND: {
		if (a->get_var_ptr()->type == VALUE_TYPE_DOUBLE)
		{
			double a_double = *(double*)a->get_var_ptr()->ptr;
			double b_double = *(double*)b->get_var_ptr()->ptr;
			return (a_double != 0) && (b_double != 0) ? new value(1.0) : new value(0.0);
		}
		throw ERROR_MUST_HAVE_DOUBLE_TYPE;
	}
	case TOK_PLUS: {
		switch (a->get_var_ptr()->type)
		{
		case VALUE_TYPE_DOUBLE: {
			double a_double = *(double*)a->get_var_ptr()->ptr;
			double b_double = *(double*)b->get_var_ptr()->ptr;
			return new value(a_double + b_double); 
		}
		case VALUE_TYPE_ARRAY: {
			value_array* a_array = (value_array*)a->get_var_ptr()->ptr;
			value_array* b_array = (value_array*)b->get_var_ptr()->ptr;
			value_array* combined = new value_array(a_array->size + b_array->size);
			for (size_t i = 0; i < a_array->size; i++)
			{
				if (a->is_root_refrence()) {
					combined->collection[i] = new unique_refrence(a_array->collection[i]->get_var_ptr(), nullptr, nullptr);
					a_array->collection[i]->change_refrence(combined->collection[i]);
				}
				else {
					combined->collection[i] = new unique_refrence(a_array->collection[i]->get_var_ptr(), a_array->collection[i], nullptr);
				}
			}
			for (size_t i = 0; i < b_array->size; i++)
			{
				if (b->is_root_refrence()) {
					combined->collection[i + a_array->size] = new unique_refrence(b_array->collection[i]->get_var_ptr(), nullptr, nullptr);
					b_array->collection[i]->change_refrence(combined->collection[i+a_array->size]);
				}
				else {
					combined->collection[i + a_array->size] = new unique_refrence(b_array->collection[i]->get_var_ptr(), b_array->collection[i], nullptr);
				}
			}
			return new value(VALUE_TYPE_ARRAY, combined);
		}
		default:
			throw ERROR_MUST_HAVE_DOUBLE_TYPE;
		}
	}
	case TOK_MINUS: {
		if (a->get_var_ptr()->type == VALUE_TYPE_DOUBLE)
		{
			double a_double = *(double*)a->get_var_ptr()->ptr;
			double b_double = *(double*)b->get_var_ptr()->ptr;
			return new value(a_double - b_double);
		}
		throw ERROR_MUST_HAVE_DOUBLE_TYPE;
	}
	case TOK_ASTERISK: {
		if (a->get_var_ptr()->type == VALUE_TYPE_DOUBLE)
		{
			double a_double = *(double*)a->get_var_ptr()->ptr;
			double b_double = *(double*)b->get_var_ptr()->ptr;
			return new value(a_double * b_double);
		}
		throw ERROR_MUST_HAVE_DOUBLE_TYPE;
	}
	case TOK_SLASH: {
		if (a->get_var_ptr()->type == VALUE_TYPE_DOUBLE)
		{
			double a_double = *(double*)a->get_var_ptr()->ptr;
			double b_double = *(double*)b->get_var_ptr()->ptr;
			return new value(a_double / b_double);
		}
		throw ERROR_MUST_HAVE_DOUBLE_TYPE;
	}
	case TOK_MODULOUS: {
		if (a->get_var_ptr()->type == VALUE_TYPE_DOUBLE)
		{
			double a_double = *(double*)a->get_var_ptr()->ptr;
			double b_double = *(double*)b->get_var_ptr()->ptr;
			return new value(fmod(a_double, b_double));
		}
		throw ERROR_MUST_HAVE_DOUBLE_TYPE;
	}
	case TOK_CARET: {
		if (a->get_var_ptr()->type == VALUE_TYPE_DOUBLE)
		{
			double a_double = *(double*)a->get_var_ptr()->ptr;
			double b_double = *(double*)b->get_var_ptr()->ptr;
			return new value(pow(a_double, b_double));
		}
		throw ERROR_MUST_HAVE_DOUBLE_TYPE;
	}
	default:
		throw ERROR_UNEXPECTED_TOK;
	}
}