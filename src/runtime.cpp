#include <fstream>
#include "lexer.h"
#include "collection.h"
#include "operators.h"
#include "runtime.h"
#include "hash.h"

//built in top-level functions
#include "types.h"
#include "console.h"
#include "linq.h"

call_frame::call_frame(function_prototype* prototype, class garbage_collector* garbage_collector) {
	this->prototype = prototype;
	this->garbage_collector = garbage_collector;
	garbage_collector->new_frame();
	this->manager = new variable_manager(garbage_collector);
}

call_frame::~call_frame() {
	delete manager;
	garbage_collector->sweep();
}

interpreter::interpreter() {
	static_var_manager = new variable_manager(&garbage_collector);
	call_stack.push(new call_frame(nullptr, &garbage_collector));
	new_constant("true", new value(VALUE_TYPE_NUMERICAL, new long double(1)));
	new_constant("false", new value(VALUE_TYPE_NUMERICAL, new long double(0)));
	new_constant("null", new value(VALUE_TYPE_NULL, nullptr));
	new_constant("numtype", new value(VALUE_TYPE_CHAR, new char(VALUE_TYPE_NUMERICAL)));
	new_constant("chartype", new value(VALUE_TYPE_CHAR, new char(VALUE_TYPE_CHAR)));
	new_constant("coltype", new value(VALUE_TYPE_CHAR, new char(VALUE_TYPE_COLLECTION)));
	new_constant("structtype", new value(VALUE_TYPE_CHAR, new char(VALUE_TYPE_STRUCT)));
	import_func("typeof", get_type);
	import_func("num", to_numerical);
	import_func("str", to_string);
	import_func("print", print);
	import_func("printl", print_line);
	import_func("input", get_input);
	import_func("array", allocate_array);
	import_func("len", get_length);
}

interpreter::~interpreter() {
	delete call_stack.top();
	call_stack.pop();
	delete static_var_manager;

	for (auto it = this->function_definitions.begin(); it != this->function_definitions.end(); ++it) {
		delete (*it).second;
	}

	for (auto it = this->struct_definitions.begin(); it != this->struct_definitions.end(); ++it) {
		delete (*it).second;
	}

	for (auto it = this->constants.begin(); it != this->constants.end(); ++it) {
		delete (*it).second;
	}
}

long double interpreter::run(const char* source, bool interactive_mode) {
	lexer* lexer = nullptr;
	std::vector<token*> to_execute;
	try {
		lexer = new class lexer(source, std::strlen(source), &constants);
		to_execute = lexer->tokenize(interactive_mode);
		delete lexer;
	}
	catch (int syntax_err) {
		delete lexer;
		//handle syntax error
		last_error = syntax_err;
		handle_syntax_err(syntax_err, lexer == nullptr ? 0 : lexer->get_pos(), source);
		return -1;
	}

	this->break_mode = false;

	value_eval* ret_val;
	bool err = false;
	try {
		ret_val = execute_block(to_execute);
		if (break_mode) {
			throw ERROR_UNEXPECTED_BREAK;
		}
	}
	catch (int runtime_error){
		last_error = runtime_error;
		//cleanup
		while (call_stack.size() > 1)
		{
			delete call_stack.top();
			call_stack.pop();
		}
		ret_val = nullptr;

		handle_runtime_err(runtime_error, last_tok);
		err = true;
	}
	
	for (auto it = to_execute.begin(); it != to_execute.end(); ++it) {
		if ((*it)->type != TOKEN_FUNC_PROTO && (*it)->type != TOKEN_STRUCT_PROTO)
			destroy_top_lvl_tok(*it);
	}
	
	if (err)
		return -1;
	if (ret_val == nullptr)
		return 0;
	long double exit_code = *ret_val->get_value()->get_numerical();
	delete ret_val;
	return exit_code;
}

void interpreter::include(const char* file_path) {
	unsigned long path_hash = insecure_hash(file_path);
	if (included_files.count(path_hash)) {
		return;
	}
	included_files.insert(path_hash);

	std::ifstream infile(file_path, std::ifstream::binary);
	if (!infile.is_open()) {
		included_files.erase(path_hash);
		throw ERROR_CANNOT_INCLUDE_FILE;
	}
	infile.seekg(0, std::ios::end);
	int buffer_length = infile.tellg();
	infile.seekg(0, std::ios::beg);
	char* buffer = new char[buffer_length + 1];
	infile.read(buffer, buffer_length);
	infile.close();
	buffer[buffer_length] = '\0';
	unsigned long ret_code = run(buffer, false);
	delete[] buffer;
	if (ret_code != 0) {
		included_files.erase(path_hash); //stop inport guarding on error
	}
}

void interpreter::import_func(const char* identifier, built_in_function function) {
	unsigned long id_hash = insecure_hash(identifier);
	if (built_in_functions.count(id_hash))
		throw ERROR_FUNCTION_PROTO_ALREADY_DEFINED;
	built_in_functions[id_hash] = function;
}

void interpreter::set_ref(variable_access_token* access, reference_apartment* reference) {
	if (access->modifiers.size() == 1) {
		if (static_var_manager->has_var(access->get_identifier()))
			static_var_manager->set_var_reference(access->get_identifier(), reference);
		else if (call_stack.top()->manager->has_var(access->get_identifier()))
			call_stack.top()->manager->set_var_reference(access->get_identifier(), reference);
		else
			throw ERROR_UNRECOGNIZED_VARIABLE;
	}
	else {
		reference_apartment* current;
		if (static_var_manager->has_var(access->get_identifier()))
			current = static_var_manager->get_var_reference(access->get_identifier());
		else if (call_stack.top()->manager->has_var(access->get_identifier()))
			current = call_stack.top()->manager->get_var_reference(access->get_identifier());
		else
			throw ERROR_UNRECOGNIZED_VARIABLE;
		for (auto i = access->modifiers.begin() + 1; i != access->modifiers.end(); ++i) {
			if (i == access->modifiers.end() - 1) {
				if ((*i)->type == TOKEN_IDENTIFIER) {
					identifier_token* prop = (identifier_token*)(*i);
					if (current->value->type != VALUE_TYPE_STRUCT)
						throw ERROR_MUST_HAVE_STRUCT_TYPE;
					structure* parent = (structure*)current->value->ptr;
					parent->set_reference(prop, reference);
				}
				else if ((*i)->type == TOKEN_INDEX) {
					index_token* index = (index_token*)(*i);
					if (current->value->type != VALUE_TYPE_COLLECTION)
						throw ERROR_MUST_HAVE_COLLECTION_TYPE;
					collection* parent = (collection*)current->value->ptr;
					value_eval* index_eval = evaluate(index->value, false);
					if (index_eval->get_value()->type != VALUE_TYPE_NUMERICAL)
						throw ERROR_MUST_HAVE_NUM_TYPE;
					unsigned long index_ul = (unsigned long)*(long double*)index_eval->get_value()->ptr;
					parent->set_reference(index_ul, reference);
					delete index_eval;
				}
			}
			else {
				if ((*i)->type == TOKEN_IDENTIFIER) {
					identifier_token* prop = (identifier_token*)(*i);
					if (current->value->type != VALUE_TYPE_STRUCT)
						throw ERROR_MUST_HAVE_STRUCT_TYPE;
					structure* parent = (structure*)current->value->ptr;
					current = parent->get_reference(prop);
				}
				else if ((*i)->type == TOKEN_INDEX) {
					index_token* index = (index_token*)(*i);
					if (current->value->type != VALUE_TYPE_COLLECTION)
						throw ERROR_MUST_HAVE_COLLECTION_TYPE;
					collection* parent = (collection*)current->value->ptr;
					value_eval* index_eval = evaluate(index->value, false);
					if (index_eval->get_value()->type != VALUE_TYPE_NUMERICAL)
						throw ERROR_MUST_HAVE_NUM_TYPE;
					unsigned long index_ul = (unsigned long)*(long double*)index_eval->get_value()->ptr;
					current = parent->get_reference(index_ul);
					delete index_eval;
				}
			}
		}
	}
}

reference_apartment* interpreter::get_ref(variable_access_token* access) {
	reference_apartment* current;
	if (static_var_manager->has_var(access->get_identifier()))
		current = static_var_manager->get_var_reference(access->get_identifier());
	else if (call_stack.top()->manager->has_var(access->get_identifier()))
		current = call_stack.top()->manager->get_var_reference(access->get_identifier());
	else
		throw ERROR_UNRECOGNIZED_VARIABLE;
	for (auto i = access->modifiers.begin() + 1; i != access->modifiers.end(); ++i)
	{
		if ((*i)->type == TOKEN_IDENTIFIER) {
			identifier_token* prop = (identifier_token*)(*i);
			if (current->value->type != VALUE_TYPE_STRUCT)
				throw ERROR_MUST_HAVE_STRUCT_TYPE;
			structure* parent = (structure*)current->value->ptr;
			current = parent->get_reference(prop);
		}
		else if((*i)->type == TOKEN_INDEX) {
			index_token* index = (index_token*)(*i);
			if (current->value->type != VALUE_TYPE_COLLECTION)
				throw ERROR_MUST_HAVE_COLLECTION_TYPE;
			collection* parent = (collection*)current->value->ptr;
			value_eval* index_eval = evaluate(index->value, false);
			if (index_eval->get_value()->type != VALUE_TYPE_NUMERICAL)
				throw ERROR_MUST_HAVE_NUM_TYPE;
			unsigned long index_ul = (unsigned long)* (long double*)index_eval->get_value()->ptr;
			if (index_ul >= parent->size || index < 0)
				throw ERROR_INDEX_OUT_OF_RANGE;
			current = parent->get_reference(index_ul);
			delete index_eval;
		}
	}
	return current;
}

value_eval* interpreter::evaluate(token* eval_tok, bool force_reference) {
	switch (eval_tok->type)
	{
	case TOKEN_VAR_ACCESS: {
		variable_access_token* access = (variable_access_token*)eval_tok;
		reference_apartment* ref = get_ref(access);
		if (force_reference || !ref->value->is_primitive()) {
			return new value_eval(ref);
		}
		return new value_eval(ref->value->clone());
	}
	case TOKEN_GET_REFERENCE: {
		get_reference_token* get_ref_tok = (get_reference_token*)eval_tok;
		return new value_eval(get_ref(get_ref_tok->var_access));
	}
	case TOKEN_VALUE: {
		value_token* val_tok = (value_token*)eval_tok;
		return new value_eval(val_tok->get_value());
	}
	case TOKEN_CREATE_STRUCT: {
		create_struct_token* create_struct = (create_struct_token*)eval_tok;
		if (!struct_definitions.count(create_struct->prototype_identifier->id_hash))
			throw ERROR_STRUCT_PROTO_NOT_DEFINED;
		structure* created_struct = new structure(struct_definitions[create_struct->prototype_identifier->id_hash], &garbage_collector);
		return new value_eval(created_struct->get_parent_ref());
	}
	case TOKEN_CREATE_ARRAY: {
		create_array_token* create_array = (create_array_token*)eval_tok;
		collection* col = new collection(create_array->values.size(), &garbage_collector);
		for (size_t i = 0; i < create_array->values.size(); i++)
		{
			value_eval* item_eval = evaluate(create_array->values[i], false);
			if (item_eval->type == VALUE_EVAL_TYPE_REF)
				col->set_reference(i, item_eval->get_reference());
			else {
				item_eval->keep();
				col->set_value(i, item_eval->get_value());
			}
			delete item_eval;
		}
		return new value_eval(col->get_parent());
	}
	case TOKEN_SET: {
		set_token* set_tok = (set_token*)eval_tok;
		value_eval* eval = evaluate(set_tok->value, false);
		if (set_tok->destination->modifiers.size() == 1) {
			if (set_tok->create_static) {
				if (static_var_manager->has_var(set_tok->destination->get_identifier()))
					throw ERROR_UNEXPECTED_TOKEN;
				if (eval->type == VALUE_EVAL_TYPE_REF)
					static_var_manager->declare_var(set_tok->destination->get_identifier(), eval->get_reference());
				else {
					eval->keep();
					static_var_manager->declare_var(set_tok->destination->get_identifier(), eval->get_value());
				}
			}
			else {
				if (call_stack.top()->manager->has_var(set_tok->destination->get_identifier())) {
					if (eval->type == VALUE_EVAL_TYPE_REF)
						call_stack.top()->manager->set_var_reference(set_tok->destination->get_identifier(), eval->get_reference());
					else {
						eval->keep();
						call_stack.top()->manager->set_var_value(set_tok->destination->get_identifier(), eval->get_value());
					}
				}
				else if (static_var_manager->has_var(set_tok->destination->get_identifier())) {
					if (eval->type == VALUE_EVAL_TYPE_REF)
						static_var_manager->set_var_reference(set_tok->destination->get_identifier(), eval->get_reference());
					else {
						eval->keep();
						static_var_manager->set_var_value(set_tok->destination->get_identifier(), eval->get_value());
					}
				}
				else {
					if (eval->type == VALUE_EVAL_TYPE_REF) {
						call_stack.top()->manager->declare_var(set_tok->destination->get_identifier(), eval->get_reference());
					}
					else {
						eval->keep();
						call_stack.top()->manager->declare_var(set_tok->destination->get_identifier(), eval->get_value());
					}
				}
			}
		}
		else {
			if (eval->type == VALUE_EVAL_TYPE_REF) {
				set_ref(set_tok->destination, eval->get_reference());
			}
			else {
				eval->keep();
				set_val(set_tok->destination, eval->get_value());
			}
		}
		if (eval->type == VALUE_EVAL_TYPE_VAL) {
			value* eval_clone = eval->get_value()->clone();
			delete eval;
			return new value_eval(eval_clone);
		}
		else {
			return eval;
		}
	}
	case TOKEN_BINARY_OP: {
		binary_operator_token* binop = (binary_operator_token*)eval_tok;
		value_eval* a_eval = evaluate(binop->left, false);
		if (binop->type == OP_AND) {
			if (*a_eval->get_value()->get_numerical() == 0) {
				delete a_eval;
				return new value_eval(new value(VALUE_TYPE_NUMERICAL, new long double(1)));
			}
			else {
				delete a_eval;
				return new value_eval(new value(VALUE_TYPE_NUMERICAL, new long double(0)));
			}
		}
		else if (binop->type == OP_OR) {
			if (*a_eval->get_value()->get_numerical() != 0)
				delete a_eval;
				return new value_eval(new value(VALUE_TYPE_NUMERICAL, new long double(1)));
		}
		value_eval* b_eval = evaluate(binop->right, false);
		value_eval* result;
		if (a_eval->get_value()->type == VALUE_TYPE_COLLECTION && b_eval->get_value()->type == VALUE_TYPE_COLLECTION && binop->op == OP_ADD) {
			reference_apartment* appartment = garbage_collector.new_apartment(nullptr);
			collection* c = new collection((collection*)a_eval->get_value()->ptr, (collection*)b_eval->get_value()->ptr, appartment);
			appartment->value = new value(VALUE_TYPE_COLLECTION, c);
			return new value_eval(appartment);
		}
		else {
			result = new value_eval(evaluate_binary_op(binop->op, a_eval->get_value(), b_eval->get_value()));
		}
		delete a_eval;
		delete b_eval;
		return result;
	}
	case TOKEN_UNIARY_OP: {
		uniary_operator_token* uniop = (uniary_operator_token*)eval_tok;
		value_eval* a_eval = evaluate(uniop->value, true);
		value_eval* result = new value_eval(evaluate_uniary_op(uniop->op, a_eval->get_value()));
		delete a_eval;
		return result;
	}
	case TOKEN_FUNCTION_CALL: {
		function_call_token* func_call = (function_call_token*)eval_tok;
		if (function_definitions.count(func_call->identifier->id_hash)) {
			function_prototype* to_execute = function_definitions[func_call->identifier->id_hash];
			if (func_call->arguments.size() != to_execute->argument_identifiers.size())
				throw ERROR_UNEXPECTED_ARGUMENT_SIZE;
			call_frame* new_frame = new call_frame(to_execute, &garbage_collector);
			unsigned int i = 0;
			for (auto it = func_call->arguments.begin(); it != func_call->arguments.end(); ++it) {
				value_eval* arg_eval = evaluate(*it, true);
				if (arg_eval->type == VALUE_EVAL_TYPE_REF)
					new_frame->manager->declare_var(to_execute->argument_identifiers[i++], arg_eval->get_reference());
				else {
					arg_eval->keep();
					new_frame->manager->declare_var(to_execute->argument_identifiers[i++], arg_eval->get_value());
				}
				delete arg_eval;
			}
			call_stack.push(new_frame);
			value_eval* ret_val = execute_block(to_execute->tokens);
			if (ret_val == nullptr) {
				if (break_mode)
					throw ERROR_UNEXPECTED_BREAK;
				ret_val = new value_eval(new value(VALUE_TYPE_NULL, nullptr));
			}
			if (ret_val->type == VALUE_EVAL_TYPE_REF)
				ret_val->get_reference()->add_reference(); //add and incrememnt before garbage collection to preserve defer a references deletion to the callee call frame for further use
			delete call_stack.top();
			call_stack.pop();
			if (ret_val->type == VALUE_EVAL_TYPE_REF)
				ret_val->get_reference()->remove_reference();
			return ret_val;
		}
		else if (built_in_functions.count(func_call->identifier->id_hash)) {
			std::vector<value*> arguments;
			std::vector<bool> can_delete;
			for (auto it = func_call->arguments.begin(); it != func_call->arguments.end(); ++it) {
				value_eval* arg_eval = evaluate(*it, false);
				arguments.push_back(arg_eval->get_value());
				can_delete.push_back(arg_eval->type != VALUE_EVAL_TYPE_REF);
				if (arg_eval->type == VALUE_EVAL_TYPE_VAL)
					arg_eval->keep();
				delete arg_eval;
			}
			reference_apartment* val = built_in_functions[func_call->identifier->id_hash](arguments, &garbage_collector);
			auto del = can_delete.begin();
			for (auto it = arguments.begin(); it != arguments.end(); ++it) {
				if(*del)
					delete* it;
				++del;
			}
			return new value_eval(val);
		}
		throw ERROR_FUNCTION_PROTO_NOT_DEFINED;
	}
	}
	throw ERROR_UNEXPECTED_TOKEN;
}

value_eval* interpreter::execute_block(std::vector<token*> tokens) {
	for (auto it = tokens.begin(); it != tokens.end(); ++it) {
		last_tok = *it;
		switch ((*it)->type)
		{
		case TOKEN_BREAK:
			this->break_mode = true;
			break;
		case TOKEN_INCLUDE: {
			include_token* include_tok = (include_token*)*it;
			include(include_tok->get_file_path());
			break;
		}
		case TOKEN_IF: 
		case TOKEN_WHILE: {
			conditional_token* current = (conditional_token*)*it;
			while (current != nullptr) {
				if (current->condition == nullptr) {
					value_eval* eval = execute_block(current->instructions);
					if (eval != nullptr) {
						if (break_mode)
							return nullptr;
						return eval;
					}
					break;
				}
				value_eval* cond_eval = evaluate(current->condition, false);
				if (*cond_eval->get_value()->get_numerical() == 0)
					current = current->get_next_conditional(false);
				else {
					value_eval* ret_eval = execute_block(current->instructions);
					if (ret_eval != nullptr) {
						delete cond_eval;
						return ret_eval;
					}
					else if (break_mode) {
						delete cond_eval;
						if (current->type == TOKEN_WHILE) {
							break_mode = false;
							break;
						}
						return nullptr;
					}
					current = current->get_next_conditional(true);
				}
				delete cond_eval;
			}
			break;
		}
		case TOKEN_UNIARY_OP:
		case TOKEN_FUNCTION_CALL:
		case TOKEN_SET:
			delete evaluate(*it, false);
			break;
		case TOKEN_RETURN: {
			return_token* ret_tok = (return_token*)*it;
			return evaluate(ret_tok->value, false); 
		}
		case TOKEN_FUNC_PROTO: {
			function_prototype* proto = (function_prototype*)*it;
			if (function_definitions.count(proto->identifier->id_hash))
				throw ERROR_FUNCTION_PROTO_ALREADY_DEFINED;
			function_definitions[proto->identifier->id_hash] = proto;
			break;
		}
		case TOKEN_STRUCT_PROTO: {
			structure_prototype* proto = (structure_prototype*)*it;
			if (struct_definitions.count(proto->identifier->id_hash))
				throw ERROR_STRUCT_PROTO_ALREADY_DEFINED;
			struct_definitions[proto->identifier->id_hash] = proto;
			break;
		}
		default:
			throw ERROR_UNEXPECTED_TOKEN;
		}
	}
	return nullptr;
}