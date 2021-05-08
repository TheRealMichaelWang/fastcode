#pragma once

#ifndef RUNTIME_H
#define RUNTIME_H

#include <map>
#include <stack>
#include <set>

#include "errors.h"
#include "value.h"
#include "references.h"
#include "tokens.h"
#include "variables.h"
#include "garbage.h"
#include "builtins.h"
#include "structure.h"
#include "hash.h"

class call_frame {
public:
	variable_manager* manager;
	function_prototype* prototype;

	call_frame(function_prototype* prototype, garbage_collector* garbage_collector);
	~call_frame();

private:
	garbage_collector* garbage_collector;
};

#define VALUE_EVAL_TYPE_REF 0
#define VALUE_EVAL_TYPE_VAL 1

struct value_eval {
public:
	
	unsigned char type;

	value_eval(reference_apartment* reference) : value_eval(reference, VALUE_EVAL_TYPE_REF, true) {}
	value_eval(value* value) : value_eval(value, VALUE_EVAL_TYPE_VAL, false) {}

	value_eval(void* ret_obj, unsigned char type, bool keep_val) {
		this->type = type;
		this->ptr = ret_obj;
		this->keep_val = keep_val;
	}

	~value_eval() {
		if (this->type == VALUE_EVAL_TYPE_VAL && !keep_val)
			delete (value*)this->ptr;
	}

	inline void keep() {
		this->keep_val = true;
	}

	inline reference_apartment* get_reference() {
		return (reference_apartment*)this->ptr;
	}

	inline value* get_value() {
		if (this->type == VALUE_EVAL_TYPE_REF) {
			return get_reference()->value;
		}
		return (value*)this->ptr;
	}

private:
	bool keep_val;
	void* ptr;
};

class interpreter {
public:
	int last_error;

	//last token, often is error token
	token* last_tok;

	interpreter();
	~interpreter();

	long double run(const char* source, bool interactive_mode);

	void include(const char* file_path);

	void import_func(const char* identifier, built_in_function function);

	inline void new_constant(const char* identifier, value* val) {
		this->constants[insecure_hash(identifier)] = val;
	}

	////you need the garbage collector to allocate non-primitive objects
	//inline garbage_collector* get_garbage_collector() {
	//	return &this->garbage_collector;
	//}

private:
	variable_manager* static_var_manager;
	garbage_collector garbage_collector;
	std::stack<call_frame*> call_stack;

	std::map<unsigned long, structure_prototype*> struct_definitions;
	std::map<unsigned long, function_prototype*> function_definitions;
	std::map<unsigned long, built_in_function> built_in_functions;

	std::set<unsigned long> included_files;
	std::map<unsigned long, value*> constants;

	bool break_mode;

	void set_ref(variable_access_token* access, reference_apartment* reference);
	reference_apartment* get_ref(variable_access_token* access);

	inline value* get_val(variable_access_token* access) {
		return get_ref(access)->value;
	}
	
	inline void set_val(variable_access_token* access, value* val) {
		get_ref(access)->set_value(val);
	}
	

	/*
	* IMPORTANT NOTE:
	Value eval must be returned as a pointer because C++ often messes around with when we return it. Using a pointer just keeps one singular version.
	*/


	//evaluates a reference or value
	value_eval* evaluate(token* token, bool force_reference);

	//executes a block of tokens
	value_eval* execute_block(std::list<token*> tokens);
};

#endif // !RUNTIME_H
