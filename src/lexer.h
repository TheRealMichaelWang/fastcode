#pragma once
#include "value.h"
#include "token.h"

#ifndef LEXER_H
#define LEXER_H

struct marker
{
public:
	int index;
	int row;
	int col;
	marker(int index, int row, int col);
};

class lexer
{
public:
	marker* position;
	lexer(const char* source_code);
	~lexer();
	token_set* tokenize();
private:
	const char* source;
	char last_char;
	token* last_tok;
	void matchTok(char matchType, char type);
	char Peek();
	char readChar();
	void readTillNextLine();
	char parseChar(char c);
	token* readNextToken();
	token* tokenizeExpression(char min=-1);
	token* tokenizeValue();
	identifier_token* tokenizeIdentifier();
};

#endif // !LEXER_H