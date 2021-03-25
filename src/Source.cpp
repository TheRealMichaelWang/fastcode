#include <iostream>
#include <fstream>
#include <stack>
#include <set>
#include <map>
#include "flib.h"
#include "lexer.h"
#include "value.h"
#include "token.h"
#include "error.h"

struct CompareIdentifiers
{
	bool operator()(const char* lhs, const char* rhs) const
	{
		return strcmp(lhs, rhs) < 0;
	}
};

class call_frame
{
public:
	token_set* instructions;
	var_context* context;
	bool isFinished;
	bool reqBreak;
	call_frame(token_set* instructions);
	call_frame(var_context* context);
	~call_frame();
};

call_frame::call_frame(token_set* instructions)
{
	this->instructions = instructions;
	this->context = new var_context(nullptr);
	isFinished = false;
	reqBreak = false;
}

call_frame::call_frame(var_context* context)
{
	this->instructions = nullptr;
	this->context = context;
	isFinished = false;
	reqBreak = false;
}

call_frame::~call_frame()
{
	delete this->context;
}

//bool can_delete(var_context* context, value* var_ptr);
unique_refrence* getVarPtr(var_context* context, identifier_token* identifier);
unique_refrence* getValue(var_context* context, token* token, bool force_refrence);
unique_refrence* execute(call_frame* call_frame, token_set* instructions);

std::map<char*, function_prototype*, CompareIdentifiers>* functionDefinitions;
std::map<char*, struct_prototype*, CompareIdentifiers>* structDefinitions;
std::set<unsigned long>* importSet;
std::stack<token_set*>* importedTokens;
var_context* static_context;
token* last_tok;
bool req_exit;

//bool can_delete(var_context* context, value* var_ptr)
//{
//	return (context->has_val_ptr(var_ptr) || static_context->has_val_ptr(var_ptr));
//}

unique_refrence* getValue(var_context* context, token* token, bool force_refrence)
{
	switch (token->type)
	{
	case TOK_VALUE:{
		value_token* val_tok = (value_token*)token;
		unique_refrence* ref = new unique_refrence(val_tok->value->clone(), nullptr, nullptr);
		//ref->replaceNullContext(context);
		return ref;
	}
	case TOK_IDENTIFIER:{
		unique_refrence* var_ptr = getVarPtr(context, (identifier_token*)token);
		if (var_ptr->get_var_ptr()->type == VALUE_TYPE_ARRAY || var_ptr->get_var_ptr()->type == VALUE_TYPE_STRUCT || force_refrence)
		{
			return new unique_refrence(var_ptr->get_var_ptr(), var_ptr, nullptr);
		}/*
		else if(var_ptr->get_var_ptr()->type == VALUE_TYPE_STRUCT){
			structure* shallow_copy = ((structure*)var_ptr->get_var_ptr()->ptr)->shallowClone(false);
			return new unique_refrence(new value(VALUE_TYPE_STRUCT, shallow_copy), nullptr, nullptr);
		}*/
		return new unique_refrence(var_ptr->get_var_ptr()->clone(), nullptr, nullptr);
	}
	case TOK_REFRENCE: {
		refrence_token* refrence = (refrence_token*)token;
		return getValue(context, refrence->value, true);
	}
	case TOK_UNIARY_OP:{
		uniary_operator_token* uni_op = (uniary_operator_token*)token;
		unique_refrence* temp_a = getValue(context, uni_op->value,true);
		temp_a->replaceNullContext(context);
		value* result = applyUniaryOp(uni_op->op_type, temp_a);
		delete temp_a;
		return new unique_refrence(result, nullptr, nullptr);
	}
	case TOK_BINARY_OP:{
		binary_operator_token* bin_op = (binary_operator_token*)token;
		unique_refrence* temp_a = getValue(context, bin_op->left, false);
		temp_a->replaceNullContext(context);
		unique_refrence* temp_b = getValue(context, bin_op->right, false);
		temp_a->replaceNullContext(context);
		value* result = applyBinaryOp(bin_op->op_type, temp_a, temp_b);
		delete temp_a;
		delete temp_b;
		return new unique_refrence(result, nullptr, nullptr);
	}
	case TOK_NEW_STRUCT: {
		create_struct* new_struct_req = (create_struct*)token;
		if (!structDefinitions->count(new_struct_req->identifier->identifier))
		{
			throw ERROR_STRUCT_NOT_FOUND;
		}
		struct_prototype* prototype = structDefinitions->operator[](new_struct_req->identifier->identifier);
		return new unique_refrence(new value(VALUE_TYPE_STRUCT, new structure(prototype, nullptr)), nullptr, nullptr);
	}
	case TOK_CREATE_ARRAY: {
		create_array* new_array_req = (create_array*)token;
		value_array* array = new value_array(new_array_req->items->size);
		/*for (size_t i = 0; i < new_array_req->items->size; i++)
		{
			array->collection[i] = getValue(context, new_array_req->items->tokens[i], false);
		}*/
		struct token* current_tok = new_array_req->items->head;
		unsigned int i = 0;
		while (current_tok != nullptr)
		{
			array->collection[i++] = getValue(context, current_tok, false);
			current_tok = current_tok->next_tok;
		}
		return new unique_refrence(new value(VALUE_TYPE_ARRAY, array), nullptr, nullptr);
	}
	case TOK_CALL_FUNCTION: {
		function_call_token* func_call = (function_call_token*)token;
		if (functionDefinitions->count(func_call->identifier->identifier)){
			function_prototype* prototype = functionDefinitions->operator[](func_call->identifier->identifier);
			if (func_call->arguments->size != prototype->params->size)
			{
				throw ERROR_UNEXPECTED_ARGUMENT_LENGTH;
			}
			call_frame* to_execute = new call_frame(prototype->body);
			struct token* current_param_tok = prototype->params->head;
			struct token* current_arg_tok = func_call->arguments->head;
			var_node* param_begin_node = nullptr;
			while (current_param_tok != nullptr)
			{
				identifier_token* param = (identifier_token*)current_param_tok;
				unique_refrence* arg_dat = getValue(context, current_arg_tok, true);
				if (to_execute->context->has_val(param->identifier)) {
					throw ERROR_VARIABLE_ALREADY_DEFINED;
				}
				if (arg_dat->is_root_refrence()) {
					arg_dat->replaceNullContext(to_execute->context);
					to_execute->context->declare(param->identifier, arg_dat);
				}
				else {
					to_execute->context->declare(param->identifier, arg_dat->parent_refrence);
					delete arg_dat;
				}
				param_begin_node = to_execute->context->head;
				current_param_tok = current_param_tok->next_tok;
				current_arg_tok = current_arg_tok->next_tok;
			}
			unique_refrence* toret = execute(to_execute, prototype->body);
			toret->context_check(to_execute->context);
			if (!toret->is_root_refrence()) {
				if (toret->parent_context == nullptr) {
					unique_refrence* parent = toret;
					toret = toret->parent_refrence;
					delete parent;
				}
				toret->replaceNullContext(context);
			}
			while (param_begin_node != nullptr)
			{
				if (param_begin_node->unique_ref->parent_context != to_execute->context) {
					param_begin_node->unique_ref->context_check(to_execute->context);
					param_begin_node->unique_ref->replaceNullContext(context);
				}
				param_begin_node = param_begin_node->next;
			}

			delete to_execute;
			return toret;
		}
		else {
			value_array* arguments = new value_array(func_call->arguments->size);

			struct token* current_tok = func_call->arguments->head;
			unsigned int i = 0;
			while (current_tok != nullptr)
			{
				arguments->collection[i++] = getValue(context, current_tok, true);
				current_tok = current_tok->next_tok;
			}

			value* toret;
			unsigned long func_id_hash = dj2b(func_call->identifier->identifier);

			if (func_id_hash == 276049397) {
				toret = readLine(context);
			}
			else if (func_id_hash == 275790354)
			{
				write(arguments);
				toret = new value();
			}
			else if (func_id_hash == 205349534) {
				writeLine(arguments);
				toret = new value();
			}
			else if (func_id_hash == 193500228) {
				toret = objSize(arguments);
			}
			else if (func_id_hash == 281262564) {
				toret = newArray(arguments, context);
			}
			else if (func_id_hash == 258007862) {
				toret = arguments->collection[0]->get_var_ptr()->clone();
			}
			else if (func_id_hash == 275940093 || func_id_hash == 2090623371)
			{
				req_exit = true;
				toret = new value();
			}
			else {
				throw ERROR_FUNCTION_NOT_FOUND;
			}
			delete arguments;
			return new unique_refrence(toret, nullptr, context);
		}
	}
	default:
		throw ERROR_UNEXPECTED_TOK;
	}
}

unique_refrence* getVarPtr(var_context* context, identifier_token* identifier)
{
	unique_refrence* value;
	if (context->has_val(identifier->identifier)) {
		value = context->searchForVal(identifier->identifier);
	}
	else if (static_context->has_val(identifier->identifier)) {
		value = static_context->searchForVal(identifier->identifier);
	}
	else {
		throw ERROR_NOT_IN_VAR_CONTEXT;
	}
	if (identifier->hasModifiers()) {
		token* current_tok = identifier->modifiers->head;
		while(current_tok != nullptr)
		{
			if (current_tok->type == TOK_PROPERTY)
			{
				property_token* prop = (property_token*)current_tok;
				if (value->get_var_ptr()->type != VALUE_TYPE_STRUCT)
				{
					throw ERROR_MUST_HAVE_STRUCT_TYPE;
				}
				structure* structure = (class structure*)value->get_var_ptr()->ptr;
				value = structure->properties->searchForVal(prop->property_identifier);
			}
			else if (current_tok->type == TOK_INDEX)
			{
				indexer_token* indexer = (indexer_token*)current_tok;
				unique_refrence* index = getValue(context, indexer->index, false);
				if (index->get_var_ptr()->type != VALUE_TYPE_DOUBLE)
				{
					throw ERROR_MUST_HAVE_DOUBLE_TYPE;
				}
				int i_index = (int)*(double*)index->get_var_ptr()->ptr;
				if (i_index >= value->get_var_ptr()->length() || i_index < 0)
				{
					throw ERROR_INDEX_OUT_OF_RANGE;
				}
				value = value->get_var_ptr()->iterate(i_index);
				delete index;
			}
			current_tok = current_tok->next_tok;
		}
	}
	return value;
}

unique_refrence* execute(call_frame* call_frame, token_set* instructions)
{
	unique_refrence* return_value = new unique_refrence(new value(), nullptr, nullptr);
	token* current_token = instructions->head;
	while(current_token != nullptr)
	{
		last_tok = current_token;
		switch (current_token->type)
		{
		case TOK_SET_VARIABLE: {
			unique_refrence* var_ptr = nullptr;
			set_variable_token* set_var = (set_variable_token*)current_token; 
			if (!set_var->identifier->hasModifiers() && !call_frame->context->has_val(set_var->identifier->identifier) && !static_context->has_val(set_var->identifier->identifier))
			{
				if (set_var->global) {
					var_ptr = static_context->declare(set_var->identifier->identifier, new unique_refrence(new value(), nullptr, static_context));
				}
				else {
					var_ptr = call_frame->context->declare(set_var->identifier->identifier, new unique_refrence(new value(), nullptr, call_frame->context));
				}
			}
			if (var_ptr == nullptr) {
				var_ptr = getVarPtr(call_frame->context, set_var->identifier);
			}
			unique_refrence* val_ptr = getValue(call_frame->context, set_var->set_tok, false);
			if (val_ptr->is_root_refrence()) {
				//var_ptr->parent_context = call_frame->context;
				var_ptr->set_var_ptr(val_ptr->get_var_ptr());
				var_ptr->replaceNullContext(call_frame->context);
				val_ptr->parent_refrence = var_ptr;
			}
			else {
				var_ptr->set_var_ptr(val_ptr->get_var_ptr(), false);
				var_ptr->change_refrence(val_ptr->parent_refrence);
			}
			delete val_ptr;
			break;
		}
		case TOK_RETURN: {
			return_token* ret_tok = (return_token*)current_token;
			if (ret_tok->ret_tok != nullptr)
			{
				call_frame->isFinished = true;
				unique_refrence* val_ptr = getValue(call_frame->context, ret_tok->ret_tok, false);
				return_value->set_var_ptr(val_ptr->get_var_ptr());
				if (val_ptr->is_root_refrence()) {
					val_ptr->replaceNullContext(call_frame->context);
					val_ptr->parent_refrence = return_value;
				}
				else {
					return_value->change_refrence(val_ptr->parent_refrence);
				}
				delete val_ptr;
			}
			goto escape;
		}
		case TOK_BREAK: {
			call_frame->reqBreak = true;
			goto escape;
		}
		case TOK_IF: {
			conditional_token* current_conditional = (conditional_token*)current_token;
			while (current_conditional != nullptr)
			{
				if (current_conditional->condition == nullptr)
				{
					unique_refrence* p_return_val = execute(call_frame, current_conditional->instructions);
					if (call_frame->isFinished)
					{
						delete return_value;
						return_value = p_return_val;
						goto escape;
					}
					delete p_return_val;
					current_conditional = current_conditional->next_condition;
					break;
				}
				unique_refrence* condition_result_val = getValue(call_frame->context, current_conditional->condition, false);
				double condition_result = *(double*)condition_result_val->get_var_ptr()->ptr;
				delete condition_result_val;
				if (condition_result == 0)
				{
					current_conditional = current_conditional->next_condition;
				}
				else
				{
					unique_refrence* p_return_val = execute(call_frame, current_conditional->instructions);
					if (req_exit) {
						goto escape;
					}
					else if (call_frame->isFinished)
					{
						delete return_value;
						return_value = p_return_val;
						goto escape;
					}
					else if (call_frame->reqBreak)
					{
						delete p_return_val;
						goto escape;
					}
					delete p_return_val;
					break;
				}
			}
			break;
		}
		case TOK_WHILE: {
			conditional_token* current_conditional = (conditional_token*)current_token;
			while (true)
			{
				unique_refrence* condition_result_val = getValue(call_frame->context, current_conditional->condition, false);
				double condition_result = *(double*)condition_result_val->get_var_ptr()->ptr;
				delete condition_result_val;
				if (condition_result == 0)
				{
					break;
				}
				unique_refrence* p_return_val = execute(call_frame, current_conditional->instructions);
				if (req_exit) {
					goto escape;
				}
				else if (call_frame->isFinished)
				{
					delete return_value;
					return_value = p_return_val;
					goto escape;
				}
				else if (call_frame->reqBreak)
				{
					call_frame->reqBreak = false;
					delete p_return_val;
					break;
				}
				delete p_return_val;
			}
			break;
		}
		case TOK_FOR: {
			for_token* for_tok = (for_token*)current_token;
			unique_refrence* to_iterate = getValue(call_frame->context, for_tok->to_iterate, false);
			if (call_frame->context->has_val(for_tok->iterator_identifier->identifier)) {
				throw ERROR_VARIABLE_ALREADY_DEFINED;
			}
			unique_refrence* iterator = call_frame->context->declare(for_tok->iterator_identifier->identifier, new unique_refrence(nullptr, nullptr, call_frame->context));
			double limit = to_iterate->get_var_ptr()->length();
			for (size_t i = 0; i < limit; i++)
			{
				unique_refrence* i_ref = to_iterate->get_var_ptr()->iterate(i);
				iterator->set_var_ptr(i_ref->get_var_ptr(), false);
				iterator->change_refrence(i_ref);
				unique_refrence* p_return_val = execute(call_frame, for_tok->instructions);
				if (req_exit) {
					goto escape;
				}
				else if (call_frame->isFinished)
				{
					delete return_value;
					return_value = p_return_val;
					goto escape;
				}
				else if (call_frame->reqBreak)
				{
					call_frame->reqBreak = false;
					delete p_return_val;
					break;
				}
				delete p_return_val;
			}
			delete to_iterate;
			call_frame->context->remove(for_tok->iterator_identifier->identifier);
			break;
		}
		case TOK_IMPORT: {
			import_token* import_tok = (import_token*)current_token;
			unsigned long file_hash = file_path_hash(import_tok->path);
			if (importSet->count(file_hash)) {
				break;
			}
			std::ifstream infile;
			infile.open(import_tok->path, std::ifstream::binary);
			if (infile.is_open()) {
				importSet->insert(file_path_hash(import_tok->path));
				infile.seekg(0, std::ios::end);
				int buffer_length = infile.tellg();
				infile.seekg(0, std::ios::beg);
				char* buffer = new char[buffer_length + 1];
				infile.read(buffer, buffer_length);
				infile.close();
				buffer[buffer_length] = '\0';
				lexer* lexer = new class lexer(buffer);
				token_set* tokens = nullptr;
				try {
					tokens = lexer->tokenize();
				}
				catch (int error) {
					error_info(error);
					std::cout << " Occured at ROW: " << lexer->position->row << ", COL: " << lexer->position->col << ". Fastcode has stopped import-guarding \"" << import_tok->path << "\"."<< std::endl;
					importSet->erase(file_hash);
				}
				delete lexer;
				delete[] buffer;
				if (tokens != nullptr) {
					importedTokens->push(tokens);
					delete execute(call_frame, tokens);
				}
				else {
					throw ERROR_IMPORT_SYNTAX_ERR; //wait to after delete lexer and delete[] buffer to throw exception
				}
			}
			else {
				throw ERROR_FILE_NOT_FOUND;
			}
			break;
		}
		case TOK_CALL_FUNCTION: {
			delete getValue(call_frame->context, current_token, false);
			break;
		}
		case TOK_UNIARY_OP: {
			delete getValue(call_frame->context, current_token, false);
			break;
		}
		case TOK_FUNCTION_PROTO: {
			function_prototype* func_proto = (function_prototype*)current_token;
			if (functionDefinitions->count(func_proto->identifier->identifier))
			{
				throw ERROR_FUNCTION_ALREADY_DEFINED;
			}
			functionDefinitions->insert(std::pair<char*, function_prototype*>(func_proto->identifier->identifier, func_proto));
			break;
		}
		case TOK_STRUCT_PROTO: {
			struct_prototype* struct_proto = (struct_prototype*)current_token;
			if (structDefinitions->count(struct_proto->identifier->identifier))
			{
				throw ERROR_STRUCT_ALREADY_DEFINED;
			}
			structDefinitions->insert(std::pair<char*, struct_prototype*>(struct_proto->identifier->identifier, struct_proto));
			break;
		}
		default:
			throw ERROR_UNEXPECTED_TOK;
		}
		if (req_exit)
		{
			goto escape;
		}
		current_token = current_token->next_tok;
	}

escape:
	if (instructions == call_frame->instructions) {
		call_frame->isFinished = true;
	}
	return return_value;
}

int main(int argc, char** argv)
{
	req_exit = false;
	functionDefinitions = new std::map<char*, function_prototype*, CompareIdentifiers>();
	structDefinitions = new std::map<char*, struct_prototype*, CompareIdentifiers>();
	importSet = new std::set<unsigned long>();
	importedTokens = new std::stack<token_set*>();
	static_context = new var_context(nullptr);
	last_tok = nullptr;
	if (argc > 1)
	{
		std::ifstream infile;
		infile.open(argv[1], std::ifstream::binary);
		if (infile.is_open()) {
			infile.seekg(0, std::ios::end);
			int buffer_length = infile.tellg();
			infile.seekg(0, std::ios::beg);
			char* buffer = new char[buffer_length+1];
			infile.read(buffer, buffer_length);
			infile.close();
			buffer[buffer_length] = '\0';

			lexer* lexer = new class lexer(buffer);
			token_set* tokens = nullptr;
			try {
				tokens = lexer->tokenize();
			}
			catch (int error) {
				error_info(error);
				std::cout << " Occured at ROW: " << lexer->position->row << ", COL: " << lexer->position->col << std::endl;
			}
			delete lexer;
			delete[] buffer;

			if (tokens != nullptr) {
				importSet->insert(file_path_hash(argv[1]));
				call_frame* main_frame = new call_frame(tokens);
				try {
					delete execute(main_frame, tokens);
				}
				catch (int e){
					std::cout << std::endl;
					error_info(e);
					if (last_tok != nullptr) {
						std::cout << " Occured at" << std::endl;
						print_token(last_tok);
						std::cout << std::endl << "NOTE: Debug output is ONLY meant for DIAGNOSTIC purposes, NOT source editing purposes.";
					}
				}
				delete main_frame;
				delete tokens;
			}
		}
		else {
			std::cout << "Unable to open file \"" << argv[1] << "\".";
		}

		req_exit = true;
	}
	else {
		std::cout << "FastCode [Version 1.0, written and designed by Michael Wang]" <<std::endl << "Type 'stop()' or 'abort()' to quit." << std::endl << std::endl;
		call_frame* main_frame = new call_frame(new var_context(nullptr));
		std::stack<token_set*>* all_instructions = new std::stack<token_set*>();
		while (!req_exit)
		{
			char* block = new char[1000];
			block[0] = 0;
			int line_n = 1;
			while (true)
			{
				std::cout << line_n;
				std::cout << ':';
				char* line = new char[250];
				std::cin.getline(line, 250);
				str_append(block, line);
				str_append(block, "\n");
				delete[] line;
				if (block_checksum(block))
				{
					break;
				}
				line_n++;
			}

			lexer* lexer = new class lexer((const char*)block);
			token_set* instructions = nullptr;
			try {
				instructions = lexer->tokenize();
			}
			catch (int error) {
				error_info(error);
				std::cout << " Occured at ROW: " << lexer->position->row << ", COL: " << lexer->position->col << std::endl;
			}
			delete lexer;
			delete[] block;

			if (instructions != nullptr) {
				all_instructions->push(instructions);

				main_frame->instructions = instructions;
				try {
					delete execute(main_frame, instructions);
				}
				catch (int e)
				{
					std::cout << std::endl;
					error_info(e);
					if (last_tok != nullptr) {
						std::cout << " Occured at" << std::endl;
						print_token(last_tok);
						std::cout << std::endl << "NOTE: Debug output is ONLY meant for DIAGNOSTIC purposes, NOT source editing purposes.";
					}
				}
				main_frame->isFinished = false;
				std::cout << std::endl;
			}
		}

		delete main_frame;

		while (!all_instructions->empty())
		{
			token_set* to_delete = all_instructions->top();
			all_instructions->pop();
			delete to_delete;
		}
		delete all_instructions;
	}
	while (!importedTokens->empty())
	{
		token_set* to_delete = importedTokens->top();
		importedTokens->pop();
		delete to_delete;
	}
	delete importSet;
	delete importedTokens;
	delete structDefinitions;
	delete functionDefinitions;
	delete static_context;
	return 0;
}