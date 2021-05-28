#ifndef BUILTINS_H
#define BUILTINS_H

#include <vector>
#include "errors.h"
#include "value.h"
#include "references.h"
#include "collection.h"
#include "garbage.h"

namespace fastcode {
	namespace builtins {
		typedef runtime::reference_apartment* (*built_in_function)(std::vector<value*> arguments, runtime::garbage_collector* gc);

		inline void match_arg_len(std::vector<value*> arguments, unsigned int expected_size) {
			if (arguments.size() != expected_size)
				throw ERROR_UNEXPECTED_ARGUMENT_SIZE;
		}

		inline void match_arg_type(value* value, char expected_type) {
			if (value->type != expected_type)
				throw ERROR_INVALID_VALUE_TYPE;
		}

		inline char* to_c_str(value* value) {
			match_arg_type(value, VALUE_TYPE_COLLECTION);
			runtime::collection* collection = (class runtime::collection*)value->ptr;
			char* c = new char[collection->size + 1];
			for (size_t i = 0; i < collection->size; i++)
			{
				match_arg_type(collection->get_value(i), VALUE_TYPE_CHAR);
				c[i] = *collection->get_value(i)->get_char();
			}
			c[collection->size] = 0;
			return c;
		}

		inline runtime::collection* from_c_str(const char* str, runtime::garbage_collector* gc) {
			runtime::collection* str_col = new runtime::collection(strlen(str), gc);
			for (size_t i = 0; i < str_col->size; i++)
			{
				str_col->set_value(i, new value(VALUE_TYPE_CHAR, new char(str[i])));
			}
			return str_col;
		}

		runtime::reference_apartment* get_handle(std::vector<value*> arguments, runtime::garbage_collector* gc); 
		runtime::reference_apartment* set_struct_property(std::vector<value*> arguments, runtime::garbage_collector* gc);
		runtime::reference_apartment* abort_program(std::vector<value*> arguments, runtime::garbage_collector* gc); 
		runtime::reference_apartment* get_hash(std::vector<value*> arguments, runtime::garbage_collector* gc);
	}
}

#endif // !BUILTINS_H
