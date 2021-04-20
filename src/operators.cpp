#include "value.h"
#include "error.h"
#include "operators.h"

value* applyUniaryOp(char type, unique_reference* value)
{
	switch (type)
	{
	case TOK_MINUS: {
		if (value->get_value_ptr()->type != VALUE_TYPE_DOUBLE)
		{
			throw ERROR_MUST_HAVE_DOUBLE_TYPE;
		}
		return new class value(-(*(double*)value->get_value_ptr()->ptr));
	}
	case TOK_NOT: {
		if (value->get_value_ptr()->type != VALUE_TYPE_DOUBLE)
		{
			throw ERROR_MUST_HAVE_DOUBLE_TYPE;
		}
		double dval = *(double*)value->get_value_ptr()->ptr;
		if (dval == 0.0) {
			return new class value(1.0);
		}
		else {
			return new class value(0.0);
		}
	}
	case TOK_INCRIMENT: {
		if (value->get_value_ptr()->type != VALUE_TYPE_DOUBLE)
		{
			throw ERROR_MUST_HAVE_DOUBLE_TYPE;
		}
		class value* old = value->get_value_ptr()->clone();
		*((double*)value->get_value_ptr()->ptr) = *((double*)value->get_value_ptr()->ptr) + 1;
		return old;
	}
	case TOK_DECRIMENT: {
		if (value->get_value_ptr()->type != VALUE_TYPE_DOUBLE)
		{
			throw ERROR_MUST_HAVE_DOUBLE_TYPE;
		}
		class value* old = value->get_value_ptr()->clone();
		*((double*)value->get_value_ptr()->ptr) = *((double*)value->get_value_ptr()->ptr) - 1;
		return old;
	}
	default:
		throw ERROR_UNEXPECTED_TOK;
	}
}

value* applyBinaryOp(char type, unique_reference* a, unique_reference* b)
{
	if (a->get_value_ptr()->type != b->get_value_ptr()->type)
	{
		switch (type)
		{
		case TOK_EQUALS:
			return new value(0.0);
		case TOK_NOT_EQUAL:
			return new value(1.0);
		}
		throw ERROR_INCOMPATIBLE_VALUE_TYPES;
	}
	switch (type)
	{
	case TOK_EQUALS: {
		return a->get_value_ptr()->compare(b->get_value_ptr()) == 0.0 ? new value(1.0) : new value(0.0);
	}
	case TOK_NOT_EQUAL: {
		return a->get_value_ptr()->compare(b->get_value_ptr()) == 0.0 ? new value(0.0) : new value(1.0);
	}
	case TOK_MORE: {
		return a->get_value_ptr()->compare(b->get_value_ptr()) > 0.0 ? new value(1.0) : new value(0.0);
	}
	case TOK_LESS: {
		return a->get_value_ptr()->compare(b->get_value_ptr()) < 0.0 ? new value(1.0) : new value(0.0);
	}
	case TOK_MORE_EQUAL: {
		return a->get_value_ptr()->compare(b->get_value_ptr()) >= 0.0 ? new value(1.0) : new value(0.0);
	}
	case TOK_LESS_EQUAL: {
		return a->get_value_ptr()->compare(b->get_value_ptr()) <= 0.0 ? new value(1.0) : new value(0.0);
	}
	case TOK_OR: {
		if (a->get_value_ptr()->type == VALUE_TYPE_DOUBLE)
		{
			double a_double = *(double*)a->get_value_ptr()->ptr;
			double b_double = *(double*)b->get_value_ptr()->ptr;
			return (a_double != 0) || (b_double != 0) ? new value(1.0) : new value(0.0);
		}
		throw ERROR_MUST_HAVE_DOUBLE_TYPE;
	}
	case TOK_AND: {
		if (a->get_value_ptr()->type == VALUE_TYPE_DOUBLE)
		{
			double a_double = *(double*)a->get_value_ptr()->ptr;
			double b_double = *(double*)b->get_value_ptr()->ptr;
			return (a_double != 0) && (b_double != 0) ? new value(1.0) : new value(0.0);
		}
		throw ERROR_MUST_HAVE_DOUBLE_TYPE;
	}
	case TOK_PLUS: {
		switch (a->get_value_ptr()->type)
		{
		case VALUE_TYPE_DOUBLE: {
			double a_double = *(double*)a->get_value_ptr()->ptr;
			double b_double = *(double*)b->get_value_ptr()->ptr;
			return new value(a_double + b_double);
		}
		case VALUE_TYPE_ARRAY: {
			value_array* a_array = (value_array*)a->get_value_ptr()->ptr;
			value_array* b_array = (value_array*)b->get_value_ptr()->ptr;
			value_array* combined = new value_array(a_array->size + b_array->size);
			for (size_t i = 0; i < a_array->size; i++)
			{
				if (a->is_root_reference()) {
					combined->collection[i] = new unique_reference(a_array->collection[i]->get_value_ptr(), nullptr, nullptr);
					a_array->collection[i]->change_refrence(combined->collection[i]);
				}
				else {
					combined->collection[i] = new unique_reference(a_array->collection[i]->get_value_ptr(), a_array->collection[i], nullptr);
				}
			}
			for (size_t i = 0; i < b_array->size; i++)
			{
				if (b->is_root_reference()) {
					combined->collection[i + a_array->size] = new unique_reference(b_array->collection[i]->get_value_ptr(), nullptr, nullptr);
					b_array->collection[i]->change_refrence(combined->collection[i + a_array->size]);
				}
				else {
					combined->collection[i + a_array->size] = new unique_reference(b_array->collection[i]->get_value_ptr(), b_array->collection[i], nullptr);
				}
			}
			return new value(VALUE_TYPE_ARRAY, combined);
		}
		default:
			throw ERROR_MUST_HAVE_DOUBLE_TYPE;
		}
	}
	case TOK_MINUS: {
		if (a->get_value_ptr()->type == VALUE_TYPE_DOUBLE)
		{
			double a_double = *(double*)a->get_value_ptr()->ptr;
			double b_double = *(double*)b->get_value_ptr()->ptr;
			return new value(a_double - b_double);
		}
		throw ERROR_MUST_HAVE_DOUBLE_TYPE;
	}
	case TOK_ASTERISK: {
		if (a->get_value_ptr()->type == VALUE_TYPE_DOUBLE)
		{
			double a_double = *(double*)a->get_value_ptr()->ptr;
			double b_double = *(double*)b->get_value_ptr()->ptr;
			return new value(a_double * b_double);
		}
		throw ERROR_MUST_HAVE_DOUBLE_TYPE;
	}
	case TOK_SLASH: {
		if (a->get_value_ptr()->type == VALUE_TYPE_DOUBLE)
		{
			double a_double = *(double*)a->get_value_ptr()->ptr;
			double b_double = *(double*)b->get_value_ptr()->ptr;
			return new value(a_double / b_double);
		}
		throw ERROR_MUST_HAVE_DOUBLE_TYPE;
	}
	case TOK_MODULOUS: {
		if (a->get_value_ptr()->type == VALUE_TYPE_DOUBLE)
		{
			double a_double = *(double*)a->get_value_ptr()->ptr;
			double b_double = *(double*)b->get_value_ptr()->ptr;
			return new value(fmod(a_double, b_double));
		}
		throw ERROR_MUST_HAVE_DOUBLE_TYPE;
	}
	case TOK_CARET: {
		if (a->get_value_ptr()->type == VALUE_TYPE_DOUBLE)
		{
			double a_double = *(double*)a->get_value_ptr()->ptr;
			double b_double = *(double*)b->get_value_ptr()->ptr;
			return new value(pow(a_double, b_double));
		}
		throw ERROR_MUST_HAVE_DOUBLE_TYPE;
	}
	default:
		throw ERROR_UNEXPECTED_TOK;
	}
}