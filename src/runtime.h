#pragma once

#ifndef RUNTIME_H
#define RUNTIME_H

#include <unordered_map>
#include <stack>
#include <unordered_set>

#include "errors.h"
#include "value.h"
#include "references.h"
#include "tokens.h"
#include "variables.h"
#include "garbage.h"
#include "builtins.h"
#include "structure.h"
#include "lexer.h"
#include "hash.h"

#define VALUE_EVAL_TYPE_REF 0
#define VALUE_EVAL_TYPE_VAL 1

namespace fastcode {
	namespace runtime {
		class interpreter {
		private:
			class call_frame {
			private:
				garbage_collector* garbage_collector;

			public:
				variable_manager* manager;
				parsing::function_prototype* prototype;

				call_frame(parsing::function_prototype* prototype, class garbage_collector* garbage_collector);
				~call_frame();
			};

			struct value_eval {
			private:
				bool keep_val;
				void* ptr;

				value_eval(void* ret_obj, unsigned char type, bool keep_val) {
					this->type = type;
					this->ptr = ret_obj;
					this->keep_val = keep_val;
				}

			public:
				unsigned char type;

				explicit value_eval(reference_apartment* reference) : value_eval(reference, VALUE_EVAL_TYPE_REF, true) {}
				explicit value_eval(value* value) : value_eval(value, VALUE_EVAL_TYPE_VAL, false) {}

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
			};

			variable_manager* static_var_manager;
			garbage_collector garbage_collector;
			std::stack<call_frame*> call_stack;

			std::unordered_map<unsigned long, fastcode::parsing::structure_prototype*> struct_definitions;
			std::unordered_map<unsigned long, parsing::function_prototype*> function_definitions;
			std::unordered_map<unsigned long, builtins::built_in_function> built_in_functions;

			std::unordered_set<unsigned long> included_files;

			struct parsing::lexer::lexer_state lexer_state;

			bool break_mode;

			void set_ref(parsing::variable_access_token* access, reference_apartment* reference);
			reference_apartment* get_ref(parsing::variable_access_token* access);

			inline value* get_val(parsing::variable_access_token* access) {
				return get_ref(access)->value;
			}

			inline void set_val(parsing::variable_access_token* access, value* val) {
				get_ref(access)->set_value(val);
			}

			/*
			* IMPORTANT NOTE:
			Value eval must be returned as a pointer because C++ often messes around with when we return it. Using a pointer just keeps one singular version.
			*/


			//evaluates a reference or value
			value_eval* evaluate(parsing::token* token, bool force_reference);

			//executes a block of tokens
			value_eval* execute_block(std::list<parsing::token*> tokens);

			bool multi_sweep;

			inline bool tok_internalized(parsing::token* tok) {
				if (tok->type == TOKEN_STRUCT_PROTO) {
					parsing::structure_prototype* proto = (parsing::structure_prototype*)tok;
					return this->struct_definitions[proto->identifier->id_hash] == proto;
				}
				else if (tok->type == TOKEN_FUNC_PROTO) {
					parsing::function_prototype* proto = (parsing::function_prototype*)tok;
					return this->function_definitions[proto->identifier->id_hash] == proto;
				}
				return false;
			}
		public:
			int last_error;

			//last token, often is error token
			parsing::token* err_tok;

			interpreter(bool multi_sweep);
			~interpreter();

			long double run(const char* source, bool interactive_mode);

			void include(const char* file_path);

			inline void import_func(const char* identifier, builtins::built_in_function function) {
				unsigned long id_hash = insecure_hash(identifier);
				if (built_in_functions.count(id_hash))
					throw ERROR_FUNCTION_PROTO_ALREADY_DEFINED;
				built_in_functions[id_hash] = function;
			}

			inline void import_struct(parsing::structure_prototype* struct_proto) {
				if (struct_definitions.count(struct_proto->identifier->id_hash))
					throw ERROR_STRUCT_PROTO_ALREADY_DEFINED;
				struct_definitions[struct_proto->identifier->id_hash] = struct_proto;
			}

			inline void new_constant(const char* identifier, value* val) {
				this->lexer_state.constants[insecure_hash(identifier)] = new parsing::value_token(val);
			}
		};
	}
}
#endif // !RUNTIME_H