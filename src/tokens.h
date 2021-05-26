#pragma once

#ifndef TOKENS_H
#define TOKENS_H

#include <list>
#include "errors.h"
#include "value.h"
#include "hash.h"

//value and accessor tokens 0-4
#define TOKEN_VALUE 0
#define TOKEN_IDENTIFIER 1

#define TOKEN_VAR_ACCESS 2
#define TOKEN_INDEX 3

#define TOKEN_GET_REFERENCE 4

//operator tokens 5 - 59
#define TOKEN_BINARY_OP 5
#define TOKEN_UNARY_OP 6

#define STD_OP_TOK_OFFSET 7 //offset for individual operator tokens/ids

//top-level instruction tokens 60-69
//instruction tokens 61-64
#define TOKEN_SET 61
#define TOKEN_FUNCTION_CALL 62
#define TOKEN_RETURN 63
#define TOKEN_BREAK 64

//control structure tokens 65-69
#define TOKEN_IF 65
#define TOKEN_ELIF 66
#define TOKEN_ELSE 67
#define TOKEN_WHILE 68
#define TOKEN_FOR 69

//hardcoded create tokens
#define TOKEN_CREATE_ARRAY 70 
#define TOKEN_CREATE_STRUCT 71

#define TOKEN_STRUCT_PROTO 72
#define TOKEN_FUNC_PROTO 73

#define TOKEN_INCLUDE 74

#define MAX_TOKEN_LIMIT 75

namespace fastcode {
	namespace parsing {
		struct token {
			unsigned char type;
			token(unsigned char type);
		};

		struct value_token :token {
		public:
			value_token(class value* value);
			~value_token();

			inline value* get_value() {
				return this->inner_value_ptr->clone();
			}

			void print();
		private:
			value* inner_value_ptr;
		};

		struct identifier_token : token {
		public:
			unsigned long id_hash;
			
			identifier_token(const char* identifier);
			identifier_token(char* identifier, unsigned long id_hash, bool delete_id = true);
			~identifier_token();

			inline void no_delete() {
				this->delete_id = false;
			}

			inline const char* get_identifier() {
				return (const char*)this->id_str_ptr;
			}

			inline void set_c_str(char* id_str) {
				if (delete_id)
					delete[] this->id_str_ptr;
				delete_id = true;
				this->id_str_ptr = id_str;
				this->id_hash = insecure_hash(id_str);
			}

			void print();
		private:
			char* id_str_ptr;
			bool delete_id;
		};

		struct variable_access_token : token {
			std::list<token*> modifiers;
			
			variable_access_token(std::list<token*> modifiers);
			~variable_access_token();

			inline identifier_token* get_identifier() {
				return (identifier_token*)modifiers.front();
			}

			void print();
		};

		struct index_token : token {
			token* value;
			
			index_token(token* value);
			~index_token();

			void print();
		};

		struct get_reference_token :token {
			variable_access_token* var_access;
			
			get_reference_token(variable_access_token* var_access);
			~get_reference_token();

			void print();
		};

		struct set_token :token {
			bool create_static;
			variable_access_token* destination;
			token* value;
			
			set_token(variable_access_token* destination, token* value, bool create_static);
			~set_token();

			void print();
		};

		struct function_call_token :token {
			identifier_token* identifier;
			std::list<token*> arguments;
			
			function_call_token(identifier_token* identifier, std::list<token*> arguments);
			~function_call_token();

			void print();
		};

		struct return_token :token {
			token* value;
			
			return_token(token* value);
			~return_token();

			void print();
		};

		struct conditional_token :token {
			token* condition;
			std::list<token*> instructions;
			conditional_token* next;

			conditional_token(unsigned char type, token* condition, std::list<token*> instructions, conditional_token* next);
			~conditional_token();
			
			conditional_token* get_next_conditional(bool condition_val);
			void print(int indent = 0);
		};

		struct for_token : token {
			token* collection;
			identifier_token* identifier;
			std::list<token*> instructions;

			for_token(identifier_token* identifier, token* collection, std::list<token*> instructions);
			~for_token();

			void print(int indent = 0);
		};

		struct create_array_token :token {
			std::list<token*> values;

			create_array_token(std::list<token*> values);
			~create_array_token();

			void print();
		};

		struct create_struct_token :token {
			identifier_token* identifier;

			create_struct_token(identifier_token* identifier);
			~create_struct_token();

			void print();
		};

		struct function_prototype :token {
			identifier_token* identifier;
			std::list<identifier_token*> argument_identifiers;
			std::list<token*> tokens;
			bool params_mode;

			function_prototype(identifier_token* identifier, std::list<identifier_token*> argument_identifiers, std::list<token*> tokens, bool params_mode);
			~function_prototype();

			void print();
		};

		struct include_token :token {
		public:
			include_token(char* file_path);
			~include_token();

			inline const char* get_file_path() {
				return this->file_path;
			}

			void print();
		private:
			char* file_path;
		};

		inline bool is_top_level_tok(token* token) {
			return (token->type > 60 && token->type < 70) || token->type == TOKEN_UNARY_OP || token->type == TOKEN_INCLUDE;
		}

		void destroy_top_lvl_tok(token* token);
		void destroy_value_tok(token* val_tok);

		inline bool is_value_tok(token* value) {
			return value->type == TOKEN_VALUE || value->type == TOKEN_VAR_ACCESS || value->type == TOKEN_FUNCTION_CALL || value->type == TOKEN_UNARY_OP || value->type == TOKEN_BINARY_OP || value->type == TOKEN_GET_REFERENCE || value->type == TOKEN_CREATE_ARRAY || value->type == TOKEN_CREATE_STRUCT || value->type == TOKEN_SET || value->type == TOKEN_RETURN;
		}
	}
}

#endif // !TOKENS_H