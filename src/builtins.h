#ifndef BUILTINS_H
#define BUILTINS_H

#include <vector>
#include "errors.h"
#include "value.h"
#include "references.h"
#include "garbage.h"

typedef reference_apartment* (*built_in_function)(std::vector<value*> arguments, garbage_collector* gc);

inline void match_arg_len(std::vector<value*> arguments, unsigned int expected_size) {
	if (arguments.size() != expected_size)
		throw ERROR_UNEXPECTED_ARGUMENT_SIZE;
}

inline void match_arg_type(value* value, char expected_type) {
	if (value->type != expected_type)
		throw ERROR_INVALID_VALUE_TYPE;
}

#endif // !BUILTINS_H
