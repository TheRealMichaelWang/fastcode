#pragma once

#ifndef LEXER_H
#define LEXER_H

#include <list>
#include <set>
#include <map>

#include "builtins.h"
#include "tokens.h"

struct group {
private:
	std::set<unsigned long> declerations;
	std::list<identifier_token*> to_process;
	identifier_token* identifier;

	void proc_id(identifier_token* id);

public:
	group* parent;

	group(struct identifier_token* identifier, group* parent = nullptr) {
		this->identifier = identifier;
		this->parent = parent;
	}

	inline void proc_decleration(identifier_token* id) {
		declerations.insert(id->id_hash);
		proc_id(id);
	}

	inline void proc_reference(identifier_token* id) {
		to_process.push_back(id);
	}

	~group() {
		for (auto i = to_process.begin(); i != to_process.end(); ++i) {
			if (declerations.count((*i)->id_hash))
				proc_id(*i);
		}
	}
};

struct lexer_state {
private:
	group* top_group = nullptr;
public:
	std::map<unsigned long, value*> constants;
	
	~lexer_state() {
		for (auto it = this->constants.begin(); it != this->constants.end(); ++it) 
			delete (*it).second;
	}

	inline void declare_id(identifier_token* id) {
		if (top_group != nullptr)
			top_group->proc_decleration(id);
	}

	inline void reference_id(identifier_token* id) {
		if (top_group != nullptr)
			top_group->proc_reference(id);
	}

	inline group* current_group() {
		return this->top_group;
	}

	inline void new_group(identifier_token* identifier) {
		top_group = new group(identifier, top_group);
	}

	inline void pop_group() {
		group* to_delete = top_group;
		top_group = top_group->parent;
		delete to_delete;
	}
};

class lexer {
public:
	lexer(const char* source, unsigned long source_length, lexer_state* lexer_state);
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

	lexer_state* lexer_state;

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
	variable_access_token* tokenize_var_access();
	variable_access_token* tokenize_var_access(identifier_token* identifier);
	token* tokenize_expression(unsigned char min = 0);
	token* tokenize_value();
};

#endif // !LEXER_H
