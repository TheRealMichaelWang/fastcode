#include <string>
#include "collection.h"
#include "builtins.h"
#include "types.h"

namespace fastcode {
	namespace builtins {
		runtime::reference_apartment* get_type(const std::vector<value*> arguments, runtime::garbage_collector* gc) {
			match_arg_len(arguments, 1);
			return gc->new_apartment(new value(VALUE_TYPE_CHAR, new char(arguments.front()->type)));
		}

		runtime::reference_apartment* to_string(const std::vector<value*> arguments, runtime::garbage_collector* gc) {
			match_arg_len(arguments, 1);
			match_arg_type(arguments[0], VALUE_TYPE_NUMERICAL);
			long double num = *arguments[0]->get_numerical();

			std::string str = std::to_string(num);

			runtime::collection* strcol = from_c_str(str.c_str(), gc);
			return strcol->get_parent_ref();
		}

		runtime::reference_apartment* to_numerical(const std::vector<value*> arguments, runtime::garbage_collector* gc) {
			match_arg_len(arguments, 1);
			if (arguments[0]->type == VALUE_TYPE_COLLECTION) {
				runtime::collection* strcol = (runtime::collection*)arguments[0]->ptr;
				char* str = to_c_str(arguments[0]);

				long double num = std::strtold(str, NULL);
				delete[] str;

				return gc->new_apartment(new value(VALUE_TYPE_NUMERICAL, new long double(num)));
			}
			else if (arguments[0]->type == VALUE_TYPE_CHAR) {
				return gc->new_apartment(new value(VALUE_TYPE_NUMERICAL, new long double(*arguments[0]->get_char())));
			}
			throw ERROR_INVALID_VALUE_TYPE;
		}

		runtime::reference_apartment* to_char(const std::vector<value*> arguments, runtime::garbage_collector* gc) {
			match_arg_len(arguments, 1);
			match_arg_type(arguments[0], VALUE_TYPE_NUMERICAL);
			return gc->new_apartment(new value(VALUE_TYPE_CHAR, new char(*arguments[0]->get_numerical())));
		}
	}
}