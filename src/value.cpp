#include "errors.h"
#include "value.h"

#define MAX_VALUE_TYPE 4

namespace fastcode {
	value::value(char type, void* ptr) {
		if (type > MAX_VALUE_TYPE) {
			throw ERROR_INVALID_VALUE_TYPE;
		}
		this->type = type;
		this->ptr = ptr;
	}

	value* value::clone() {
		value* clone = new value(this->type, nullptr);
		switch (this->type)
		{
		case VALUE_TYPE_NULL:
			break;
		case VALUE_TYPE_CHAR:
			clone->ptr = new char(*(char*)this->ptr);
			break;
		case VALUE_TYPE_NUMERICAL:
			clone->ptr = new long double(*(long double*)this->ptr);
			break;
		default:
			throw ERROR_INVALID_VALUE_TYPE;
		}
		return clone;
	}
}