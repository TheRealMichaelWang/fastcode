#include <ctype.h>
#include "errors.h"
#include "hash.h"
#include "operators.h"
#include "structure.h"
#include "lexer.h"

//encapsulation chars
#define TOKEN_OPEN_PARAM 0 + MAX_TOKEN_LIMIT
#define TOKEN_CLOSE_PARAM 1 + MAX_TOKEN_LIMIT
#define TOKEN_OPEN_BRACE 2 + MAX_TOKEN_LIMIT
#define TOKEN_CLOSE_BRACE 3 + MAX_TOKEN_LIMIT
#define TOKEN_OPEN_BRACKET 4 + MAX_TOKEN_LIMIT
#define TOKEN_CLOSE_BRACKET 5 + MAX_TOKEN_LIMIT

#define TOKEN_PERIOD 6 + MAX_TOKEN_LIMIT
#define TOKEN_COMMA 7 + MAX_TOKEN_LIMIT

#define TOKEN_STATIC 8 + MAX_TOKEN_LIMIT
#define TOKEN_CONST 9 + MAX_TOKEN_LIMIT

#define TOKEN_QUICK_BODY 10 + MAX_TOKEN_LIMIT

#define TOKEN_GROUP 11 + MAX_TOKEN_LIMIT
#define TOKEN_END_GROUP 12 + MAX_TOKEN_LIMIT

#define TOKEN_IN 13 + MAX_TOKEN_LIMIT

namespace fastcode {
	namespace parsing {
		//throws an exception if the token type doesn't match the expected type
		inline void match_tok(token* token, unsigned char type) {
			if (token == nullptr)
				throw ERROR_UNEXPECTED_END;
			if (token->type != type)
				throw ERROR_UNEXPECTED_TOKEN;
		}

		//prints a statement
		inline function_call_token* print_encapsulate(token* token) {
			if (token->type == TOKEN_FUNCTION_CALL) {
				function_call_token* call_tok = (function_call_token*)token;
				if (call_tok->identifier->id_hash == 275790354) {
					return call_tok;
				}
			}
			std::list<class token*> args;
			args.push_back(token);
			return new function_call_token(new identifier_token("print"), args);
		}

		lexer::lexer(const char* source, unsigned long source_length, struct lexer_state* lexer_state) {
			this->source = source;
			this->source_length = source_length;
			this->position = 0;
			this->last_char = 0;
			this->last_tok = nullptr;

			this->lexer_state = lexer_state;

			read_char();
			read_token();
		}

		char lexer::read_char() {
			if (position == source_length) {
				this->last_char = 0;
				return 0;
			}
			return last_char = source[position++];
		}

		token* lexer::read_token() {
			while (last_char == ' ' || last_char == '\t' || last_char == '\r' || last_char == '\n' || last_char == ';') {
				read_char();
			}
			if (isalpha(last_char) || last_char == '_' || last_char == '@') {
				std::list<char> id_chars;
				do {
					id_chars.push_back(last_char);
				} while (isalpha(read_char()) || last_char == '_' || last_char == '@');
				char* id_buf = new char[id_chars.size() + 1];
				int index = 0;
				for (auto it = id_chars.begin(); it != id_chars.end(); ++it)
					id_buf[index++] = *it;
				id_buf[index] = 0; //remeber to add a nul terminator
				unsigned long hash = insecure_hash(id_buf);
				//switches use pre-computed hashes because only constants are allowed
				switch (hash)
				{
				case 257929342: //while
					delete[] id_buf;
					return last_tok = new token(TOKEN_WHILE);
				case 5863380: //if
					delete[] id_buf;
					return last_tok = new token(TOKEN_IF);
				case 2090257189: //elif
					delete[] id_buf;
					return last_tok = new token(TOKEN_ELIF);
				case 2090232142: //else
					delete[] id_buf;
					return last_tok = new token(TOKEN_ELSE);
				case 193510031: // new
					delete[] id_buf;
					return last_tok = new token(TOKEN_CREATE_STRUCT);
				case 498533450: //struct
				case 4184890820: //record
					return last_tok = new token(TOKEN_STRUCT_PROTO);
				case 998468366: //proc
				case 2090156121: //procedure
				case 1574308811: //function
					return last_tok = new token(TOKEN_FUNC_PROTO);
				case 193491522: //ref
					return last_tok = new token(TOKEN_GET_REFERENCE);
				case 281511589: //return
					return last_tok = new token(TOKEN_RETURN);
				case 193489624: //and
					return last_tok = new token(OP_AND);
				case 5863782: //or
					return last_tok = new token(OP_OR);
				case 4135260141:
					return last_tok = new token(TOKEN_STATIC);
				case 275975372:
					return last_tok = new token(TOKEN_CONST);
				case 1413452809:
					return last_tok = new token(TOKEN_INCLUDE);
				case 264645514: //break
					return last_tok = new token(TOKEN_BREAK);
				case 271304754:
					return last_tok = new token(TOKEN_GROUP);
				case 303295209:
					return last_tok = new token(TOKEN_END_GROUP);
				case 193504908:
					return last_tok = new token(TOKEN_FOR);
				case 5863644:
					return last_tok = new token(TOKEN_IN);
				case 193499145:
					while (last_char != '\n')
						read_char();
					return read_token();
				default: {
					return last_tok = new identifier_token(id_buf, hash);
				}
				}
			}
			else if (isdigit(last_char)) {
				std::list<char> num_chars;
				do {
					num_chars.push_back(last_char);
				} while (isdigit(read_char()) || last_char == '.');
				char* num_buf = new char[num_chars.size() + 1];
				int index = 0;
				for (auto it = num_chars.begin(); it != num_chars.end(); ++it)
					num_buf[index++] = *it;
				num_buf[index] = 0;
				value_token* to_ret = new value_token(new value(VALUE_TYPE_NUMERICAL, new long double(std::strtold(num_buf, NULL))));
				delete[] num_buf;
				return last_tok = to_ret;
			}
			else if (last_char == '\"') {
				std::list<token*> chars;
				read_char();
				while (last_char != 0 && last_char != '\"')
				{
					chars.push_back(new value_token(new value(VALUE_TYPE_CHAR, new char(read_data_char()))));
				}
				if (last_char == 0)
					throw ERROR_UNEXPECTED_END;
				read_char();
				return last_tok = new create_array_token(chars);
			}
			else if (last_char == '\'') {
				read_char();
				char dat_char = read_data_char();
				read_char();
				return last_tok = new value_token(new value(VALUE_TYPE_CHAR, new char(dat_char)));
			}
			char old = last_char;
			read_char();
			switch (old)
			{
			case '(':
				return last_tok = new token(TOKEN_OPEN_PARAM);
			case ')':
				return last_tok = new token(TOKEN_CLOSE_PARAM);
			case '{':
				return last_tok = new token(TOKEN_OPEN_BRACE);
			case '}':
				return last_tok = new token(TOKEN_CLOSE_BRACE);
			case '[':
				return last_tok = new token(TOKEN_OPEN_BRACKET);
			case ']':
				return last_tok = new token(TOKEN_CLOSE_BRACKET);
			case '.':
				return last_tok = new token(TOKEN_PERIOD);
			case ',':
				return last_tok = new token(TOKEN_COMMA);
			case '=':
				if (last_char == '=') {
					read_char();
					return last_tok = new token(OP_EQUALS);
				}
				else if (last_char == '>') {
					read_char();
					return last_tok = new token(TOKEN_QUICK_BODY);
				}
				return last_tok = new token(TOKEN_SET);
			case '!':
				if (last_char == '=') {
					read_char();
					return last_tok = new token(OP_NOT_EQUAL);
				}
				return last_tok = new token(OP_INVERT);
			case '>':
				if (last_char == '=') {
					read_char();
					return last_tok = new token(OP_MORE_EQUAL);
				}
				return last_tok = new token(OP_MORE);
			case '<':
				if (last_char == '=') {
					read_char();
					return last_tok = new token(OP_LESS_EQUAL);
				}
				return last_tok = new token(OP_LESS);
			case '+':
				if (last_char == '+') {
					read_char();
					return last_tok = new token(OP_INCRIMENT);
				}
				return last_tok = new token(OP_ADD);
			case '-':
				if (last_char == '-') {
					read_char();
					return last_tok = new token(OP_DECRIMENT);
				}
				return last_tok = new token(OP_SUBTRACT);
			case '*':
				return last_tok = new token(OP_MULTIPLY);
			case '/':
				return last_tok = new token(OP_DIVIDE);
			case '%':
				return last_tok = new token(OP_MODULOUS);
			case '^':
				return last_tok = new token(OP_POWER);
			case 0:
				return last_tok = nullptr;
			}
			throw ERROR_UNRECOGNIZED_TOKEN;
		}

		char lexer::read_data_char() {
			if (last_char == 0)
				throw ERROR_UNEXPECTED_END;
			char ret_char;
			if (last_char == '\\') {
				switch (read_char())
				{
				case 'N':
				case 'n':
					ret_char = '\n';
					break;
				case 'T':
				case 't':
					ret_char = '\t';
					break;
				case 'R':
				case 'r':
					ret_char = '\r';
					break;
				case 'B':
				case 'b':
					ret_char = '\b';
					break;
				case '\\':
					ret_char = '\\';
					break;
				case '\"':
					ret_char = '\"';
					break;
				case '\'':
					ret_char = '\'';
					break;
				case '0':
					ret_char = 0;
					break;
				default:
					throw ERROR_UNRECOGNIZED_ESCAPE_SEQ;
				}
			}
			else
				ret_char = last_char;
			read_char();
			return ret_char;
		}

		std::list<token*> lexer::tokenize(bool interactive_mode) {
			std::list<token*> tokens;
			while (last_tok != nullptr && last_tok->type != TOKEN_CLOSE_BRACE)
			{
				token* tok = tokenize_statement(interactive_mode);
				if (tok != nullptr)
					tokens.push_back(tok);
			}
			if (last_tok != nullptr)
				match_tok(last_tok, TOKEN_CLOSE_BRACE);
			if (interactive_mode)
				lexer_state->group_all_references();
			return tokens;
		}

		token* lexer::tokenize_statement(bool interactive_mode) {
			switch (last_tok->type)
			{
			case TOKEN_GROUP: {
				delete last_tok;
				match_tok(read_token(), TOKEN_IDENTIFIER);
				identifier_token* id = (identifier_token*)last_tok;

				lexer_state->new_group(id);

				read_token();

				return nullptr;
			}
			case TOKEN_END_GROUP: {
				delete last_tok;
				lexer_state->pop_group();

				read_token();

				return nullptr;
			}
			case TOKEN_CONST: {
				delete last_tok;
				match_tok(read_token(), TOKEN_IDENTIFIER);
				identifier_token* id = (identifier_token*)last_tok;
				if (lexer_state->constants.count(id->id_hash)) {
					delete lexer_state->constants[id->id_hash];
				}
				match_tok(read_token(), TOKEN_SET);
				delete last_tok;
				match_tok(read_token(), TOKEN_VALUE);
				value_token* value_tok = (value_token*)last_tok;
				lexer_state->constants[id->id_hash] = value_tok;
				delete id;
				read_token();
				return nullptr;
			}
			case TOKEN_STATIC: {
				delete last_tok;
				match_tok(read_token(), TOKEN_IDENTIFIER);
				identifier_token* id = (identifier_token*)last_tok;
				lexer_state->declare_id(id, GROUP_TYPE_VAR);
				match_tok(read_token(), TOKEN_SET);
				delete last_tok;
				read_token();
				token* val_tok = tokenize_expression();
				std::list<token*> modifiers;
				modifiers.push_back(id);
				return new set_token(new variable_access_token(modifiers), val_tok, true);
			}
			case TOKEN_BREAK: {
				token* tok = last_tok;
				read_token();
				return tok;
			}
			case TOKEN_INCLUDE: {
				delete last_tok;
				match_tok(read_token(), TOKEN_CREATE_ARRAY);
				create_array_token* create_str = (create_array_token*)last_tok;
				char* buf = new char[create_str->values.size() + 1];
				unsigned char size = 0;
				for (auto it = create_str->values.begin(); it != create_str->values.end(); ++it) {
					value_token* val_tok = (value_token*)*it;
					value* val = val_tok->get_value();
					buf[size++] = *val->get_char();
					delete val;
				}
				buf[size] = 0;
				delete create_str;
				read_token();
				return new include_token(buf);
			}
			case TOKEN_IDENTIFIER:
				if (interactive_mode) {
					return print_encapsulate(tokenize_expression());
				}
				return tokenize_expression();
			case TOKEN_OPEN_PARAM:
			case TOKEN_OPEN_BRACKET:
			case TOKEN_CREATE_ARRAY:
			case TOKEN_CREATE_STRUCT:
			case TOKEN_BINARY_OP:
			case TOKEN_unary_OP:
			case TOKEN_VALUE:
			case OP_SUBTRACT:
			case OP_INVERT:
				if (interactive_mode) {
					return print_encapsulate(tokenize_expression());
				}
				throw ERROR_UNEXPECTED_TOKEN;
			case TOKEN_RETURN:
				delete last_tok;
				read_token();
				return new return_token(tokenize_expression());
			case TOKEN_IF: {
				delete last_tok;
				read_token();
				token* condition = tokenize_expression();
				conditional_token* if_struct = new conditional_token(TOKEN_IF, condition, tokenize_body(), nullptr);
				conditional_token* current = if_struct;
				while (last_tok != nullptr)
				{
					if (last_tok->type == TOKEN_ELIF) {
						delete last_tok;
						read_token();
						condition = tokenize_expression();
						current->next = new conditional_token(TOKEN_ELIF, condition, tokenize_body(), nullptr);
						current = current->next;
					}
					else if (last_tok->type == TOKEN_ELSE) {
						delete last_tok;
						read_token();
						current->next = new conditional_token(TOKEN_ELSE, nullptr, tokenize_body(), nullptr);
						break;
					}
					else
						break;
				}
				return if_struct;
			}
			case TOKEN_WHILE: {
				delete last_tok;
				read_token();
				token* condition = tokenize_expression();
				return new conditional_token(TOKEN_WHILE, condition, tokenize_body(), nullptr);
			}
			case TOKEN_FOR: {
				delete last_tok;
				match_tok(read_token(), TOKEN_IDENTIFIER);
				identifier_token* id = (identifier_token*)last_tok;
				lexer_state->declare_id(id, GROUP_TYPE_VAR);
				match_tok(read_token(), TOKEN_IN);
				delete last_tok;
				read_token();
				token* collection = tokenize_value();
				return new for_token(id, collection, tokenize_body());
			}
			case TOKEN_STRUCT_PROTO: {
				delete last_tok;
				match_tok(read_token(), TOKEN_IDENTIFIER);
				identifier_token* proto_id = (identifier_token*)last_tok;
				lexer_state->declare_id(proto_id, GROUP_TYPE_STRUCT);
				match_tok(read_token(), TOKEN_OPEN_BRACE);
				delete last_tok;
				std::list<identifier_token*> properties;
				while (last_tok != nullptr && read_token()->type != TOKEN_CLOSE_BRACE)
				{
					match_tok(last_tok, TOKEN_IDENTIFIER);
					properties.push_back((identifier_token*)last_tok);
				}
				if (last_tok == nullptr)
					throw ERROR_UNEXPECTED_END;
				delete last_tok;
				read_token();
				return new structure_prototype(proto_id, properties);
			}
			case TOKEN_FUNC_PROTO: {
				delete last_tok;
				match_tok(read_token(), TOKEN_IDENTIFIER);
				identifier_token* proto_id = (identifier_token*)last_tok;
				lexer_state->declare_id(proto_id, GROUP_TYPE_FUNC);
				match_tok(read_token(), TOKEN_OPEN_PARAM);
				delete last_tok;
				std::list<identifier_token*> params;
				while (last_tok != nullptr && read_token()->type != TOKEN_CLOSE_PARAM)
				{
					if (last_tok->type == TOKEN_COMMA) {
						delete last_tok;
						read_token();
					}
					match_tok(last_tok, TOKEN_IDENTIFIER);
					params.push_back((identifier_token*)last_tok);
					lexer_state->declare_id((identifier_token*)last_tok, GROUP_TYPE_VAR);
				}
				if (last_tok == nullptr)
					throw ERROR_UNEXPECTED_TOKEN;
				delete last_tok;
				read_token();
				return new function_prototype(proto_id, params, tokenize_body());
			}
			}
			throw ERROR_UNEXPECTED_TOKEN;
		}

		std::list<token*> lexer::tokenize_body() {
			if (last_tok->type == TOKEN_OPEN_BRACE) {
				delete last_tok;
				read_token();
				std::list<token*> body = tokenize(false);
				delete last_tok;
				read_token();
				return body;
			}
			else if (last_tok->type == TOKEN_QUICK_BODY) {
				delete last_tok;
				read_token();
				std::list<token*> body;
				body.push_back(tokenize_statement(false));
				return body;
			}
			else {
				throw ERROR_UNEXPECTED_TOKEN;
			}
		}

		variable_access_token* lexer::tokenize_var_access() {
			match_tok(last_tok, TOKEN_IDENTIFIER);
			identifier_token* identifier = (identifier_token*)last_tok;
			read_token();
			return tokenize_var_access(identifier);
		}

		variable_access_token* lexer::tokenize_var_access(identifier_token* identifier) {
			std::list<token*> toks;
			toks.push_back(identifier);
			while (true)
			{
				if (last_tok->type == TOKEN_PERIOD) {
					delete last_tok;
					match_tok(read_token(), TOKEN_IDENTIFIER);
					toks.push_back(last_tok);
				}
				else if (last_tok->type == TOKEN_OPEN_BRACKET) {
					delete last_tok;
					read_token();
					toks.push_back(new index_token(tokenize_expression()));
					match_tok(last_tok, TOKEN_CLOSE_BRACKET);
					delete last_tok;
				}
				else {
					break;
				}
				read_token();
			}
			return new variable_access_token(toks);
		}
		token* lexer::tokenize_value() {
			if (last_tok->type == TOKEN_IDENTIFIER) {
				identifier_token* identifier = (identifier_token*)last_tok;
				read_token();
				//tokenize function call
				if (last_tok->type == TOKEN_OPEN_PARAM) {
					delete last_tok;
					std::list<token*> arguments;
					while (true)
					{
						read_token();
						if (last_tok->type == TOKEN_CLOSE_PARAM)
							break;
						arguments.push_back(tokenize_expression());
						if (last_tok->type != TOKEN_COMMA) {
							break;
						}
						delete last_tok;
					}
					match_tok(last_tok, TOKEN_CLOSE_PARAM);
					delete last_tok;
					read_token();
					this->lexer_state->reference_id(identifier, GROUP_TYPE_FUNC);
					return new function_call_token(identifier, arguments);
				}
				else
				{
					if (lexer_state->constants.count(identifier->id_hash)) {
						unsigned long hash = identifier->id_hash;
						delete identifier;
						return new value_token(lexer_state->constants[hash]->get_value());
					}

					if (last_tok->type == TOKEN_SET)
						this->lexer_state->declare_id(identifier, GROUP_TYPE_VAR);
					else
						this->lexer_state->reference_id(identifier, GROUP_TYPE_VAR);

					variable_access_token* var_access = tokenize_var_access(identifier);
					if (last_tok->type == OP_INCRIMENT) {
						delete last_tok;
						read_token();
						return new unary_operator_token(var_access, OP_INCRIMENT);
					}
					else if (last_tok->type == OP_DECRIMENT) {
						delete last_tok;
						read_token();
						return new unary_operator_token(var_access, OP_DECRIMENT);
					}
					else if (last_tok->type == TOKEN_SET) {
						delete last_tok;
						read_token();
						token* to_set = tokenize_expression();
						return new set_token(var_access, to_set, false);
					}

					return var_access;
				}
			}
			else if (last_tok->type == TOKEN_GET_REFERENCE) {
				delete last_tok;
				match_tok(read_token(), TOKEN_IDENTIFIER);
				lexer_state->reference_id((identifier_token*)last_tok, GROUP_TYPE_VAR);
				get_reference_token* get_ref_tok = new get_reference_token(tokenize_var_access());
				return get_ref_tok;
			}
			else if (last_tok->type == TOKEN_OPEN_PARAM) {
				delete last_tok;
				read_token();
				token* val_tok = tokenize_expression();
				match_tok(last_tok, TOKEN_CLOSE_PARAM);
				delete last_tok;
				read_token();
				return val_tok;
			}
			else if (last_tok->type == TOKEN_OPEN_BRACKET) {
				delete last_tok;
				std::list<token*> values;

				while (true)
				{
					read_token();

					if (last_tok->type == TOKEN_CLOSE_BRACKET)
						break;

					values.push_back(tokenize_expression());
					if (last_tok->type != TOKEN_COMMA) {
						break;
					}
					delete last_tok;
				}
				match_tok(last_tok, TOKEN_CLOSE_BRACKET);
				delete last_tok;
				read_token();
				return new create_array_token(values);
			}
			else if (last_tok->type == TOKEN_CREATE_STRUCT) {
				delete last_tok;
				match_tok(read_token(), TOKEN_IDENTIFIER);
				create_struct_token* new_struct = new create_struct_token((identifier_token*)last_tok);
				this->lexer_state->reference_id(new_struct->identifier, GROUP_TYPE_STRUCT);
				read_token();
				return new_struct;
			}
			else if (last_tok->type == TOKEN_VALUE || last_tok->type == TOKEN_CREATE_ARRAY) {
				token* val_tok = last_tok;
				read_token();
				return val_tok;
			}
			else if (last_tok->type == OP_INVERT || last_tok->type == OP_SUBTRACT) {
				unsigned char type = last_tok->type;
				if (type == OP_SUBTRACT)
					type = OP_NEGATE;
				delete last_tok;
				read_token();
				return new unary_operator_token(tokenize_value(), type);
			}
			throw ERROR_UNEXPECTED_TOKEN;
		}

		token* lexer::tokenize_expression(unsigned char min) {
			//utilizes shunting-yard
			token* lhs = tokenize_value();
			while (last_tok != nullptr && is_op_tok(last_tok->type) && get_operator_precedence(last_tok->type) >= min) {
				unsigned char op = last_tok->type;
				unsigned char prec = get_operator_precedence(op);
				const unsigned char assoc = 0; //operator assosiativity 0 = left, 1 = right
				unsigned char nextmin = assoc == 0 ? prec : prec + 1;
				delete last_tok;
				read_token();
				token* rhs = tokenize_expression(nextmin);
				lhs = new binary_operator_token(lhs, rhs, op);
			}
			return lhs;
		}
	}
}