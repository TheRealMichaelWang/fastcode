#pragma once

#ifndef LEXER_H
#define LEXER_H

#include <list>
#include <set>
#include <map>

#include "builtins.h"
#include "tokens.h"

class lexer {
public:
	lexer(const char* source, unsigned long source_length, std::map<unsigned long, value*>* constants, std::set<unsigned long>* group_excluded_ids);
	~lexer();

	inline bool eos() {
		return last_tok == nullptr ? last_char == 0 : last_tok->type == 18 + MAX_TOKEN_LIMIT;
	}

	inline unsigned int get_pos() {
		return this->position;
	}

	std::list<token*> tokenize(bool interactive_mode);
private:
	const char* source;
	unsigned long position;
	unsigned long source_length;
	char last_char;
	token* last_tok;

	std::map<unsigned long, value*>* constants;
	std::list<char*> group_stack;
	std::set<unsigned long>* group_excluded_ids;
	std::list<unsigned long> to_exclude;

	//reads the next availible character.
	char read_char();

	//gets the next availible character without eating it
	inline char peek() {
		if (position == source_length) {
			return 0;
		}
		return source[position];
	}

	char read_data_char();

	//reads the next top-level token
	token* read_token();
	token* tokenize_statement(bool interactive_mode);
	std::list<token*> tokenize_body();
	identifier_token* apply_groups(identifier_token* id, bool creating);
	variable_access_token* tokenize_var_access();
	variable_access_token* tokenize_var_access(identifier_token* identifier);
	token* tokenize_expression(unsigned char min = 0);
	token* tokenize_value();
};

#endif // !LEXER_H
