#include "structure.h"
#include "collection.h"
#include "runtime.h"
#include "builtins.h"
#include "errors.h"

#include <iostream>

namespace fastcode {
	namespace builtins {
		runtime::reference_apartment* get_handle(const std::vector<value*> arguments, runtime::garbage_collector* gc) {
			match_arg_len(arguments, 1);
			if (arguments[0]->type == VALUE_TYPE_STRUCT) {
				return gc->new_apartment(new value(VALUE_TYPE_HANDLE, ((runtime::structure*)arguments[0]->ptr)->get_parent_ref()));
			}
			else if (arguments[0]->type == VALUE_TYPE_COLLECTION) {
				return gc->new_apartment(new value(VALUE_TYPE_HANDLE, ((runtime::collection*)arguments[0]->ptr)->get_parent_ref()));
			}
			throw ERROR_INVALID_VALUE_TYPE;
		}

		runtime::reference_apartment* set_struct_property(const std::vector<value*> arguments, runtime::garbage_collector* gc) {
			match_arg_len(arguments, 3);
			match_arg_type(arguments[0], VALUE_TYPE_STRUCT);
			match_arg_type(arguments[1], VALUE_TYPE_NUMERICAL);
			runtime::structure* structure = (runtime::structure*)arguments[0]->ptr;
			unsigned int property_index = unsigned int(*arguments[1]->get_numerical());

			if (property_index >= structure->get_size())
				throw ERROR_INVALID_VALUE_TYPE;

			if (arguments[2]->type == VALUE_TYPE_COLLECTION) {
				runtime::collection* col = (runtime::collection*)arguments[2]->ptr;
				structure->set_reference_at(property_index, col->get_parent_ref());
			}
			else if (arguments[2]->type == VALUE_TYPE_STRUCT) {
				runtime::structure* s = (runtime::structure*)arguments[2]->ptr;
				structure->set_reference_at(property_index, s->get_parent_ref());
			}
			else {
				structure->set_value_at(property_index, arguments[2]->clone());
			}
			return gc->new_apartment(new value(VALUE_TYPE_NULL, nullptr));
		}

		runtime::reference_apartment* abort_program(const std::vector<value*> arguments, runtime::garbage_collector* gc) {
			if (arguments.size() > 0) {
				std::cout << "The program was aborted with the following message:" << std::endl;
				for (auto i = arguments.begin(); i != arguments.end(); ++i) {
					if ((*i)->type == VALUE_TYPE_COLLECTION) {
						char* msg = to_c_str(*i);
						std::cout << msg;
						delete[] msg;
					}
					else if ((*i)->type == VALUE_TYPE_CHAR)
						std::cout << *(*i)->get_char();
					else if ((*i)->type == VALUE_TYPE_NUMERICAL)
						std::cout << *(*i)->get_numerical();
				}
				std::cout << std::endl;
			}
			throw ERROR_ABORTED;
		}

		runtime::reference_apartment* get_hash(const std::vector<value*> arguments, runtime::garbage_collector* gc) {
			match_arg_len(arguments, 1);
			return gc->new_apartment(new value(VALUE_TYPE_NUMERICAL, new double((unsigned int)arguments[0]->hash())));
		}
	}
}