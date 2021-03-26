#include <ctype.h>
#include <iostream>
#include "error.h"
#include "token.h"
#include "lexer.h"
#include "flib.h"

#define EndOfFile 0

marker::marker(int index, int row, int col)
{
	this->index = index;
	this->row = row;
	this->col = col;
}

lexer::lexer(const char* source)
{
	this->source = source;
	this->position = new marker(-1, 0, -1);
	this->last_char = readChar();
	this->last_tok = nullptr;
}

lexer::~lexer()
{
	delete position;
}

char lexer::Peek()
{
	if (position->index + 1 >= strlen(source))
	{
		return EndOfFile;
	}
	return source[position->index + 1];
}

char lexer::readChar()
{
	position->col++;
	position->index++;
	if (position->index >= strlen(source)){
		last_char = EndOfFile;
		return last_char;
	}
	last_char = source[position->index];
	if (last_char == '\n'){
		position->col = 0;
		position->row++;
	}
	return last_char;
}

token* lexer::readNextToken()
{
	while (last_char == ' ' || last_char == '\t' || last_char == '\r' || last_char == '\n' || last_char == ';')
	{
		readChar();
	}
	if (isalpha(last_char) || last_char == '_' || last_char == '@')
	{
		char* identifier = new char[26];
		int i = 0;
		identifier[i++] = last_char;
		while (isalnum(readChar()) || last_char == '_' || last_char == '@')
		{
			if (i == 25)
			{
				throw ERROR_STR_OVERFLOW;
			}
			identifier[i++] = last_char;
		}
		identifier[i++] = EndOfFile;
		unsigned long id_hash = dj2b(identifier);
		switch (id_hash)
		{
		case 193489624: 
			delete[] identifier; return new token(TOK_AND);
		case 193507094:
			delete[] identifier; return new token(TOK_NOT);
		case 5863782: 
			delete[] identifier; return new token(TOK_OR);
		case 5863644:
			delete[] identifier; return new token(TOK_IN);
		case 5863380:
			delete[] identifier; return new token(TOK_IF);
		case 2090232142:
			delete[] identifier; return new token(TOK_ELSE);
		case 2090257189:
			delete[] identifier; return new token(TOK_ELIF);
		case 193504908:
			delete[] identifier; return new token(TOK_FOR);
		case 257929342:
			delete[] identifier; return new token(TOK_WHILE);
		case 2090156121:
		case 998468366:
		case 1574308811:
			delete[] identifier; return new token(TOK_FUNCTION);
		case 498533450:
		case 4184890820:
			delete[] identifier; return new token(TOK_STRUCT);
		case 264645514:
			delete[] identifier; return new token(TOK_BREAK);
		case 281511589:
			delete[] identifier; return new token(TOK_RETURN);
		case 516104224:
			delete[] identifier; return new token(TOK_IMPORT);
		case 182392118:
		case 4135260141:
			delete[] identifier; return new token(TOK_GLOBAL);
		case 193491522:
			delete[] identifier; return new token(TOK_REFRENCE);
		case 193510031:
			delete[] identifier; return new token(TOK_NEW);
		case 193499145:{
			delete[] identifier;
			readTillNextLine();
			return readNextToken();
		}
		case 2090234533:
			delete[] identifier; return new value_token(new value(1.0));
		case 258183920:
			delete[] identifier; return new value_token(new value(0.0));
		case 2090476384:
			delete[] identifier; return new value_token(new value());
		default:
			return (token*)new identifier_token(identifier);
		}
	}
	else if (isdigit(last_char))
	{
		char* numstr = new char[26];
		int i = 0;
		numstr[i++] = last_char;
		while (isdigit(readChar()) || last_char == '.')
		{
			if (i == 25)
			{
				throw ERROR_STR_OVERFLOW;
			}
			numstr[i++] = last_char;
		}
		numstr[i++] = EndOfFile;
		token* ret = (token*)new value_token(new value(atof(numstr)));
		delete[] numstr;
		return ret;
	}
	else if (last_char == '"')
	{
		char* strbuf = new char[251];
		int i = 0;
		while (readChar() != '"')
		{
			if(last_char == EndOfFile)
			{
				throw ERROR_UNEXPECTED_EOF;
			}
			if (i == 250)
			{
				throw ERROR_STR_OVERFLOW;
			}
			strbuf[i++] = parseChar(last_char);
		}
		strbuf[i++] = EndOfFile;
		readChar();
		token* toret = (token*)new value_token(new value(strbuf));
		delete[] strbuf;
		return toret;
	}
	else if (last_char == '\'')
	{
		readChar();
		value* val = new value(parseChar(last_char));
		readChar();
		readChar();
		return (token*)new value_token(val);
	}
	switch (last_char)
	{
	case ',': 
		readChar(); return new token(TOK_COMMA);
	case '.':
		readChar(); return new token(TOK_PERIOD);
	case '(':
		readChar(); return new token(TOK_OPEN_PAREN);
	case ')':
		readChar(); return new token(TOK_CLOSE_PAREN);
	case '{':
		readChar(); return new token(TOK_OPEN_BRACE);
	case '}':
		readChar(); return new token(TOK_CLOSE_BRACE);
	case '[':
		readChar(); return new token(TOK_OPEN_BRACKET);
	case ']': 
		readChar(); return new token(TOK_CLOSE_BRACKET);
	case '+':
		if (Peek() == '+') {
			readChar(); readChar(); return new token(TOK_INCRIMENT);
		}
		readChar(); return new token(TOK_PLUS);
	case '-':
		if (Peek() == '-') {
			readChar(); readChar();  return new token(TOK_DECRIMENT);
		}
		readChar(); return new token(TOK_MINUS);
	case '*': 
		readChar(); return new token(TOK_ASTERISK);
	case '/':
		if (Peek() == '/')
		{
			readTillNextLine();
			return readNextToken();
		}
		readChar(); return new token(TOK_SLASH);
	case '^': 
		readChar(); return new token(TOK_CARET);
	case '%': 
		readChar(); return new token(TOK_MODULOUS);
	case '&':
		if (Peek() == '&') {
			readChar(); readChar(); return new token(TOK_AND);
		}
		readChar(); return new token(TOK_REFRENCE);
	case '|':
		if (Peek() == '|') {
			readChar(); readChar(); return new token(TOK_OR);
		}
		throw ERROR_UNRECOGNIZED_TOK;
	case '=':
		if (Peek() == '=') {
			readChar(); readChar(); return new token(TOK_EQUALS);
		}
		else if (Peek() == '>') {
			readChar(); readChar(); return new token(TOK_QUICK_BLOCK);
		}
		readChar(); return new token(TOK_SET);
	case '!':
		if (Peek() == '=') {
			readChar(); readChar(); return new token(TOK_NOT_EQUAL);
		}
		readChar(); return new token(TOK_NOT);
	case '>':
		if (Peek() == '=') {
			readChar(); readChar(); return new token(TOK_MORE_EQUAL);
		}
		readChar(); return new token(TOK_MORE);
	case '<':
		if (Peek() == '=') {
			readChar(); readChar(); return new token(TOK_LESS_EQUAL);
		}
		readChar(); return new token(TOK_LESS);
	case EndOfFile:
		return new token(TOK_EOF);
	default:
		throw ERROR_UNRECOGNIZED_TOK;
	}
}

void lexer::readTillNextLine()
{
	while (last_char != '\n' && last_char != EndOfFile)
	{
		readChar();
	}
	readChar();
}

char lexer::parseChar(char c)
{
	if (c == '\\')
	{
		switch (Peek())
		{
		case 'n':
			readChar(); return '\n';
		case 'r':
			readChar(); return '\r';
		case 't':
			readChar(); return '\t';
		case 'b':
			readChar(); return '\n';
		case '"':
			readChar(); return '"';
		case '\'':
			readChar(); return '\'';
		case '\\':
			readChar(); return '\\';
		default:
			throw ERROR_UNRECOGNIZED_CONTROL_SEQ;
		}
	}
	return c;
}

void lexer::matchTok(char matchType, char type)
{
	if (matchType != type)
	{
		throw ERROR_UNEXPECTED_TOK;
	}
}

token_set* lexer::tokenize()
{
	token_set* tok_set = new token_set();
	last_tok = readNextToken();
	bool req_make_glob_var = false;
	while (last_tok->type != TOK_EOF)
	{
		switch (last_tok->type)
		{
		case TOK_IDENTIFIER:
		{
			identifier_token* identifier = tokenizeIdentifier();
			switch (last_tok->type)
			{
			case TOK_SET:
			{
				delete last_tok;
				last_tok = readNextToken();
				set_variable_token* set_var_tok = new set_variable_token(identifier, tokenizeExpression(), req_make_glob_var);
				tok_set->push(set_var_tok);
				req_make_glob_var = false;
				break;
			}
			case TOK_OPEN_PAREN:
			{
				delete last_tok;
				//function call

				//fetch arguments
				token_set* arguments = new token_set();

				while (true)
				{
					last_tok = readNextToken();
					if (last_tok->type == TOK_CLOSE_PAREN)
					{
						delete last_tok;
						break;
					}
					arguments->push(tokenizeExpression());
					if (last_tok->type == TOK_CLOSE_PAREN)
					{
						delete last_tok;
						break;
					}
					else if (last_tok->type != TOK_COMMA)
					{
						throw ERROR_UNEXPECTED_TOK;
					}
					delete last_tok;
				}
				tok_set->push(new function_call_token(identifier, arguments));
				last_tok = readNextToken();
				break;
			}
			case TOK_INCRIMENT:
			{
				delete last_tok;
				tok_set->push(new uniary_operator_token(TOK_INCRIMENT, identifier));
				last_tok = readNextToken();
				break;
			}
			case TOK_DECRIMENT:
			{
				delete last_tok;
				tok_set->push(new uniary_operator_token(TOK_DECRIMENT, identifier));
				last_tok = readNextToken();
				break;
			}
			case EOF:
				throw ERROR_UNEXPECTED_EOF;
			default:
				throw ERROR_UNEXPECTED_TOK;
			}
			break;
		}
		case TOK_FUNCTION:
		{
			delete last_tok;
			matchTok((last_tok = readNextToken())->type, TOK_IDENTIFIER);

			//get parameter identifiers
			identifier_token* identifier = (identifier_token*)last_tok;
			matchTok((last_tok = readNextToken())->type, TOK_OPEN_PAREN);

			token_set* params = new token_set();
			while (true)
			{
				delete last_tok;
				last_tok = readNextToken();
				if (last_tok->type == TOK_CLOSE_PAREN)
				{
					delete last_tok;
					break;
				}
				matchTok(last_tok->type, TOK_IDENTIFIER);
				params->push(last_tok);
				last_tok = readNextToken();
				if (last_tok->type == TOK_CLOSE_PAREN)
				{
					delete last_tok;
					break;
				}
				else if (last_tok->type != TOK_COMMA)
				{
					throw ERROR_UNEXPECTED_TOK;
				}
			}

			//push params to a function tok, and call tokenize to read function body again.
			
			last_tok = readNextToken();
			if (last_tok->type == TOK_OPEN_BRACE) {
				delete last_tok;

				token_set* body = tokenize(); //tokenize function body

				struct function_prototype* function_prototype = new struct function_prototype(identifier, params, body);
				tok_set->push(function_prototype);
				last_tok = readNextToken();
			}
			else if (last_tok->type == TOK_QUICK_BLOCK) {
				delete last_tok;
				last_tok = readNextToken();
				token_set* body = new token_set();
				body->push(new return_token(tokenizeExpression()));

				struct function_prototype* function_prototype = new struct function_prototype(identifier, params, body);
				tok_set->push(function_prototype);
			}
			else {
				throw ERROR_UNEXPECTED_TOK;
			}
			break;
		}
		case TOK_STRUCT:
		{
			delete last_tok;
			matchTok((last_tok = readNextToken())->type, TOK_IDENTIFIER);
			identifier_token* identifier = (identifier_token*)last_tok;

			//get properties
			matchTok((last_tok = readNextToken())->type, TOK_OPEN_BRACE);
			delete last_tok;

			token_set* properties = new token_set();
			while ((last_tok = readNextToken())->type != TOK_CLOSE_BRACE)
			{
				if (last_tok->type == TOK_COMMA)
				{
					delete last_tok;
					last_tok = readNextToken();
				}
				matchTok(last_tok->type, TOK_IDENTIFIER);
				properties->push(last_tok);
			}
			delete last_tok;
			struct struct_prototype* struct_prototype = new struct struct_prototype(identifier, properties);
			tok_set->push(struct_prototype);
			last_tok = readNextToken();
			break;
		}
		case TOK_IF: {
			delete last_tok;
			last_tok = readNextToken();
			token* condition = tokenizeExpression(); //read condition
			token_set* body;
			if (last_tok->type == TOK_OPEN_BRACE) {
				delete last_tok;

				body = tokenize();
				last_tok = readNextToken();
			}
			else if (last_tok->type == TOK_QUICK_BLOCK) {
				delete last_tok;
				last_tok = readNextToken();
				body = new token_set();
				body->push(new return_token(tokenizeExpression()));
			}
			else {
				throw ERROR_UNEXPECTED_TOK;
			}
			tok_set->push(new conditional_token(TOK_IF, condition, body, nullptr));
			break;
		}
		case TOK_ELIF: {
			delete last_tok;

			last_tok = readNextToken(); //read condition
			token* condition = tokenizeExpression();

			token_set* body;
			if (last_tok->type == TOK_OPEN_BRACE) {
				delete last_tok;

				body = tokenize();
				last_tok = readNextToken();
			}
			else if (last_tok->type == TOK_QUICK_BLOCK) {
				delete last_tok;
				last_tok = readNextToken();
				body = new token_set();
				body->push(new return_token(tokenizeExpression()));
			}
			else {
				throw ERROR_UNEXPECTED_TOK;
			}

			//load and check previous conditional
			conditional_token* conditional = (conditional_token*)tok_set->tail;
			while (conditional->next_condition != nullptr)
			{
				if (conditional->type != TOK_IF && conditional->type != TOK_ELIF)
				{
					throw ERROR_UNEXPECTED_TOK;
				}
				conditional = conditional->next_condition;
			}
			conditional->next_condition = new conditional_token(TOK_ELIF, condition, body, nullptr);
			break;
		}
		case TOK_ELSE: {
			delete last_tok;

			last_tok = readNextToken();
			token_set* body;
			if (last_tok->type == TOK_OPEN_BRACE) {
				delete last_tok;

				body = tokenize();
				last_tok = readNextToken();
			}
			else if (last_tok->type == TOK_QUICK_BLOCK) {
				delete last_tok;
				last_tok = readNextToken();
				body = new token_set();
				body->push(new return_token(tokenizeExpression()));
			}
			else {
				throw ERROR_UNEXPECTED_TOK;
			}

			if (body->size > 0) { //check is body size is greater than zero
				//load and check previous conditional
				conditional_token* conditional = (conditional_token*)tok_set->tail;
				while (conditional->next_condition != nullptr)
				{
					if (conditional->type != TOK_IF && conditional->type != TOK_ELIF)
					{
						throw ERROR_UNEXPECTED_TOK;
					}
					conditional = conditional->next_condition;
				}
				conditional->next_condition = new conditional_token(TOK_ELSE, nullptr, body, nullptr);
			}
			else
			{
				delete body;
			}
			break;
		}
		case TOK_WHILE: {
			delete last_tok;
			last_tok = readNextToken(); //read condition
			token* condition = tokenizeExpression();

			matchTok(last_tok->type, TOK_OPEN_BRACE);
			delete last_tok;
			token_set* body = tokenize(); //read body

			conditional_token* while_loop = new conditional_token(TOK_WHILE, condition, body, nullptr);
			while_loop->next_condition = while_loop;

			tok_set->push(while_loop);

			last_tok = readNextToken();
			break;
		}
		case TOK_FOR: {
			delete last_tok;

			matchTok((last_tok = readNextToken())->type, TOK_IDENTIFIER); //get iterator
			identifier_token* identifier = (identifier_token*)last_tok;

			matchTok((last_tok = readNextToken())->type, TOK_IN);
			delete last_tok;

			last_tok = readNextToken();
			token* to_iterate = tokenizeExpression();

			matchTok(last_tok->type, TOK_OPEN_BRACE);
			delete last_tok;
			token_set* body = tokenize(); //read body

			tok_set->push(new for_token(to_iterate, identifier, body));

			last_tok = readNextToken();
			break;
		}
		case TOK_RETURN:
		{
			delete last_tok;
			last_tok = readNextToken();
			if (last_tok->type == TOK_IDENTIFIER || last_tok->type == TOK_VALUE || last_tok->type == TOK_OPEN_BRACKET || is_op_token(last_tok->type))
			{
				tok_set->push(new return_token(tokenizeExpression()));
			}
			else
			{
				tok_set->push(new return_token());
			}
			break;
		}
		case TOK_CLOSE_BRACE: {
			delete last_tok;
			return tok_set; //time to return to prev read function;
		}
		case TOK_BREAK: {
			tok_set->push(last_tok);
			last_tok = readNextToken();
			break;
		}
		case TOK_GLOBAL: {
			delete last_tok;
			last_tok = readNextToken();
			req_make_glob_var = true;
			break;
		}
		case TOK_IMPORT: {
			delete last_tok;
			value_token* val_tok = (value_token*)readNextToken();
			if (val_tok->value->type != VALUE_TYPE_ARRAY) {
				throw ERROR_UNEXPECTED_TOK;
			}
			value_array* char_array = (value_array*)val_tok->value->ptr;
			if (!char_array->checktype(VALUE_TYPE_CHAR)) {
				throw ERROR_UNEXPECTED_TOK;
			}
			char* str = new char[char_array->size+1];
			for (size_t i = 0; i < char_array->size; i++)
			{
				str[i] = *(char*)char_array->collection[i]->get_var_ptr()->ptr;
			}
			str[char_array->size] = '\0';
			delete val_tok;

			tok_set->push(new import_token(str));

			last_tok = readNextToken();
			break;
		}
		default:
			throw ERROR_UNEXPECTED_TOK;
		}
	}
	delete last_tok;
	return tok_set;
}

token* lexer::tokenizeExpression(char min)
{
	token* lhs = tokenizeValue();
	while (is_op_token(last_tok->type) && get_op_prior(last_tok->type) > min)
	{
		char op = last_tok->type;
		char prec = get_op_prior(op);
		char assoc = 0;
		char nextmin = assoc == 0 ? prec : prec + 1;
		delete last_tok;
		last_tok = readNextToken();
		token* rhs = tokenizeExpression(nextmin);
		lhs = new binary_operator_token(op, lhs, rhs);
	}
	return lhs;
}

//TODO: implement this
token* lexer::tokenizeValue()
{
	token* tok;
	switch (last_tok->type)
	{
	case TOK_REFRENCE: {
		delete last_tok;
		last_tok = readNextToken();
		return new refrence_token(tokenizeValue());
	}
	case TOK_VALUE:
		tok = last_tok; break;
	case TOK_IDENTIFIER: {
		identifier_token* identifier = tokenizeIdentifier(); 
		if (last_tok->type == TOK_OPEN_PAREN)
		{
			delete last_tok;
			token_set* arguments = new token_set(); //fetch arguments
			while (true)
			{
				last_tok = readNextToken();
				if (last_tok->type == TOK_CLOSE_PAREN)
				{
					delete last_tok;
					last_tok = readNextToken();
					break;
				}
				arguments->push(tokenizeExpression());
				if (last_tok->type == TOK_CLOSE_PAREN)
				{
					delete last_tok;
					last_tok = readNextToken();
					break;
				}
				else if (last_tok->type != TOK_COMMA)
				{
					throw ERROR_UNEXPECTED_TOK;
				}
				delete last_tok;
			}
			tok = new function_call_token(identifier, arguments);
		}
		else
		{
			tok = identifier;
			if (last_tok->type == TOK_INCRIMENT) {
				delete last_tok;
				last_tok = readNextToken();
				return new uniary_operator_token(TOK_INCRIMENT, tok);
			}
			else if (last_tok->type == TOK_DECRIMENT) {
				delete last_tok;
				last_tok = readNextToken();
				return new uniary_operator_token(TOK_DECRIMENT, tok);
			}
		}
		return tok;
	}
	case TOK_NEW: {
		delete last_tok;
		matchTok((last_tok = readNextToken())->type, TOK_IDENTIFIER);
		identifier_token* identifier = (identifier_token*)last_tok;
		tok = new create_struct(identifier);
		break;
	}
	case TOK_OPEN_PAREN: {
		delete last_tok;
		last_tok = readNextToken();
		tok = tokenizeExpression();
		matchTok(last_tok->type, TOK_CLOSE_PAREN); 
		delete last_tok;
		break;
	}
	case TOK_OPEN_BRACKET: {
		token_set* array_items = new token_set();
		while (true)
		{
			delete last_tok;
			last_tok = readNextToken();
			if (last_tok->type == TOK_CLOSE_BRACKET)
			{
				delete last_tok;
				last_tok = readNextToken();
				break;
			}
			array_items->push(tokenizeExpression());
			if (last_tok->type == TOK_CLOSE_BRACKET)
			{
				delete last_tok;
				last_tok = readNextToken();
				break;
			}
			else if (last_tok->type != TOK_COMMA)
			{
				throw ERROR_UNEXPECTED_TOK;
			}
		}
		return new create_array(array_items);
	}
	case TOK_MINUS: {
		delete last_tok;
		last_tok = readNextToken();
		return new uniary_operator_token(TOK_MINUS, tokenizeValue());
	}
	case TOK_NOT: {
		delete last_tok;
		last_tok = readNextToken();
		return new uniary_operator_token(TOK_NOT, tokenizeValue());
	}
	case TOK_EOF:
		throw ERROR_UNEXPECTED_EOF;
	default: {
		if (is_op_token(last_tok->type))
		{
			if (last_tok->type == TOK_MINUS) //catch preceding uniary operators
			{
				char op_type = last_tok->type;
				delete last_tok;
				last_tok = readNextToken();
				tok = new uniary_operator_token(op_type, tokenizeValue()); //uniary operators can only be applid on single term values
				last_tok = readNextToken();
				return tok;
			}
		}
		throw ERROR_UNEXPECTED_TOK; 
	}
	}
	last_tok = readNextToken();
	return tok;
}

//TODO: implement this. This function gets the identifier with any properties or indexers
identifier_token* lexer::tokenizeIdentifier()
{
	matchTok(last_tok->type, TOK_IDENTIFIER);
	identifier_token* identifier = (identifier_token*)last_tok;
	token_set* modifiers = new token_set();
	bool search = true;
	while (search)
	{
		switch ((last_tok = readNextToken())->type)
		{
		case TOK_PERIOD:
		{
			delete last_tok;
			matchTok((last_tok = readNextToken())->type, TOK_IDENTIFIER);
			identifier_token* property_tok = (identifier_token*)last_tok;
			char* new_id = new char[strlen(property_tok->identifier) + 1];
			for (size_t i = 0; i < strlen(property_tok->identifier); i++)
			{
				new_id[i] = property_tok->identifier[i];
			}
			new_id[strlen(property_tok->identifier)] = '\0';
			modifiers->push(new property_token(new_id));
			delete property_tok;
			break;
		}
		case TOK_OPEN_BRACKET:
		{
			delete last_tok;
			last_tok = readNextToken();
			token* index = tokenizeExpression();
			matchTok(last_tok->type, TOK_CLOSE_BRACKET);
			delete last_tok;
			modifiers->push(new indexer_token(index));
			break;
		}
		default:
		{
			search = false;
			break; 
		}
		}
	}

	if (modifiers->size > 0)
	{
		identifier->modifiers = modifiers;
	}
	else
	{
		delete modifiers;
	}
	return identifier;
}