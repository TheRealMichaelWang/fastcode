#include <cmath>
#include "errors.h"
#include "operators.h"
#include "collection.h"

binary_operator_token::binary_operator_token(token* left, token* right, unsigned char op) : token(TOKEN_BINARY_OP) {
	if (!is_binary_operator(op))
		throw ERROR_INVALID_BINARY_OPERATOR;
	if (!is_value_tok(right) || !is_value_tok(left))
		throw ERROR_UNEXPECTED_TOKEN;
	this->left = left;
	this->right = right;
	this->op = op;
}

binary_operator_token::~binary_operator_token() {
	destroy_value_tok(this->left);
	destroy_value_tok(this->right);
}

uniary_operator_token::uniary_operator_token(token* value, unsigned char op) : token(TOKEN_UNIARY_OP) {
	if (!is_uniary_operator(op))
		throw ERROR_INVALID_UNIARY_OPERATOR;
	if (!is_value_tok(value)) 
		throw ERROR_UNEXPECTED_TOKEN;
	this->value = value;
	this->op = op;
}

uniary_operator_token::~uniary_operator_token() {
	destroy_value_tok(this->value);
}

value* evaluate_binary_op(unsigned char op, value* a, value* b) {
	if (is_uniary_operator(op))
		throw ERROR_UNEXPECTED_TOKEN;
	switch (op)
	{
	case OP_EQUALS:
		return new value(VALUE_TYPE_NUMERICAL, new long double(a->compare(b) == 0 ? 1 : 0));
	case OP_NOT_EQUAL:
		return new value(VALUE_TYPE_NUMERICAL, new long double(a->compare(b) == 0 ? 0 : 1));
	case OP_MORE:
		return new value(VALUE_TYPE_NUMERICAL, new long double(a->compare(b) > 0 ? 1 : 0));
	case OP_LESS:
		return new value(VALUE_TYPE_NUMERICAL, new long double(a->compare(b) < 0 ? 1 : 0));
	case OP_MORE_EQUAL:
		return new value(VALUE_TYPE_NUMERICAL, new long double(a->compare(b) >= 0 ? 1 : 0));
	case OP_LESS_EQUAL:
		return new value(VALUE_TYPE_NUMERICAL, new long double(a->compare(b) <= 0 ? 1 : 0));
	case OP_AND:
		if (a->type != b->type)
			throw ERROR_OP_NOT_IMPLEMENTED;
		if (a->type != VALUE_TYPE_NUMERICAL)
			throw ERROR_MUST_HAVE_NUM_TYPE;
		return new value(VALUE_TYPE_NUMERICAL, new long double(*a->get_numerical() != 0 && *b->get_numerical() != 0 ? 1 : 0));
	case OP_OR:
		if (a->type != b->type)
			throw ERROR_OP_NOT_IMPLEMENTED;
		if (a->type != VALUE_TYPE_NUMERICAL)
			throw ERROR_MUST_HAVE_NUM_TYPE;
		return new value(VALUE_TYPE_NUMERICAL, new long double(*a->get_numerical() != 0 || *b->get_numerical() != 0 ? 1 : 0));
	case OP_ADD:
		if (a->type != b->type)
			throw ERROR_OP_NOT_IMPLEMENTED;
		if (a->type == VALUE_TYPE_NUMERICAL)
			return new value(VALUE_TYPE_NUMERICAL, new long double(*a->get_numerical() + *b->get_numerical()));
		/*else if (a->type == VALUE_TYPE_COLLECTION)
			return new value(VALUE_TYPE_COLLECTION, new collection((collection*)a->ptr, (collection*)b->ptr));*/
		throw ERROR_OP_NOT_IMPLEMENTED;
	case OP_SUBTRACT:
		if (a->type != b->type)
			throw ERROR_OP_NOT_IMPLEMENTED;
		if (a->type != VALUE_TYPE_NUMERICAL)
			throw ERROR_MUST_HAVE_NUM_TYPE;
		return new value(VALUE_TYPE_NUMERICAL, new long double(*a->get_numerical() - *b->get_numerical()));
	case OP_MULTIPLY:
		if (a->type != b->type)
			throw ERROR_OP_NOT_IMPLEMENTED;
		if (a->type != VALUE_TYPE_NUMERICAL)
			throw ERROR_MUST_HAVE_NUM_TYPE;
		return new value(VALUE_TYPE_NUMERICAL, new long double(*a->get_numerical() * *b->get_numerical()));
	case OP_DIVIDE:
		if (a->type != b->type)
			throw ERROR_OP_NOT_IMPLEMENTED;
		if (a->type != VALUE_TYPE_NUMERICAL)
			throw ERROR_MUST_HAVE_NUM_TYPE;
		if (*b->get_numerical() == 0)
			throw ERROR_DIVIDE_BY_ZERO;
		return new value(VALUE_TYPE_NUMERICAL, new long double(*a->get_numerical() / *b->get_numerical()));
	case OP_MODULOUS:
		if (a->type != b->type)
			throw ERROR_OP_NOT_IMPLEMENTED;
		if (a->type != VALUE_TYPE_NUMERICAL)
			throw ERROR_MUST_HAVE_NUM_TYPE;
		return new value(VALUE_TYPE_NUMERICAL, new long double(fmod(*a->get_numerical(), *b->get_numerical())));
	case OP_POWER:
		if (a->type != b->type)
			throw ERROR_OP_NOT_IMPLEMENTED;
		if (a->type != VALUE_TYPE_NUMERICAL)
			throw ERROR_MUST_HAVE_NUM_TYPE;
		return new value(VALUE_TYPE_NUMERICAL, new long double(pow(*a->get_numerical(), *b->get_numerical())));
	default:
		throw ERROR_OP_NOT_IMPLEMENTED;
	}
}

value* evaluate_uniary_op(unsigned char op, value* a) {
	if (!is_uniary_operator(op))
		throw ERROR_UNEXPECTED_TOKEN;
	switch (op) {
	case OP_INVERT:
		if (a->type != VALUE_TYPE_NUMERICAL)
			throw ERROR_MUST_HAVE_NUM_TYPE;
		return new value(VALUE_TYPE_NUMERICAL, new long double(*a->get_numerical() == 0 ? 1 : 0));
	case OP_NEGATE:
		if (a->type != VALUE_TYPE_NUMERICAL)
			throw ERROR_MUST_HAVE_NUM_TYPE;
		return new value(VALUE_TYPE_NUMERICAL, new long double(-*a->get_numerical()));
	case OP_INCRIMENT: {
		if (a->type != VALUE_TYPE_NUMERICAL)
			throw ERROR_MUST_HAVE_NUM_TYPE;
		long double* old_ptr = a->get_numerical();
		long double* new_ptr = new long double(*old_ptr + 1);
		a->ptr = new_ptr;
		return new value(VALUE_TYPE_NUMERICAL, old_ptr);
	}
	case OP_DECRIMENT: {
		if (a->type != VALUE_TYPE_NUMERICAL)
			throw ERROR_MUST_HAVE_NUM_TYPE;
		long double* old_ptr = a->get_numerical();
		long double* new_ptr = new long double(*old_ptr - 1);
		a->ptr = new_ptr;
		return new value(VALUE_TYPE_NUMERICAL, old_ptr);
	}
	default:
		throw ERROR_OP_NOT_IMPLEMENTED;
	}
}