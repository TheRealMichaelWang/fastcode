#pragma once

#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "errors.h"
#include "value.h"
#include "references.h"
#include "runtime.h"

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
		this->lexer_state.constants[insecure_hash(identifier)] = new value_token(val);
	}

private:
	variable_manager* static_var_manager;
	garbage_collector garbage_collector;
	std::stack<call_frame*> call_stack;

	std::map<unsigned long, structure_prototype*> struct_definitions;
	std::map<unsigned long, function_prototype*> function_definitions;
	std::map<unsigned long, built_in_function> built_in_functions;

	std::set<unsigned long> included_files;

	struct lexer_state lexer_state;

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


#endif // !INTERPRETER_H
