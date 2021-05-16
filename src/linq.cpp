#include "builtins.h"
#include "collection.h"
#include "linq.h"

namespace fastcode {
	namespace builtins {
		runtime::reference_apartment* allocate_array(std::list<value*> arguments, runtime::garbage_collector* gc) {
			match_arg_len(arguments, 1);
			match_arg_type(arguments.front(), VALUE_TYPE_NUMERICAL);

			if (*arguments.front()->get_numerical() < 0) {
				throw ERROR_INVALID_VALUE_TYPE;
			}

			unsigned long req_size = (unsigned long)*arguments.front()->get_numerical();

			runtime::collection* allocated_array = new runtime::collection(req_size, gc);
			return allocated_array->get_parent_ref();
		}

		runtime::reference_apartment* get_length(std::list<value*> arguments, runtime::garbage_collector* gc) {
			match_arg_len(arguments, 1);
			match_arg_type(arguments.front(), VALUE_TYPE_COLLECTION);

			runtime::collection* collection = (runtime::collection*)arguments.front()->ptr;
			return gc->new_apartment(new value(VALUE_TYPE_NUMERICAL, new long double((long double)collection->size)));
		}

		runtime::reference_apartment* count_instances(std::list<value*> arguments, runtime::garbage_collector* gc) {
			match_arg_len(arguments, 2);
			match_arg_type(arguments.front(), VALUE_TYPE_COLLECTION);

			runtime::collection* collection = (runtime::collection*)arguments.front()->ptr;
			unsigned int instances = 0;

			for (size_t i = 0; i < collection->size; i++)
			{
				if (collection->get_value(i)->compare(arguments.back()) == 0)
					instances++;
			}
			return gc->new_apartment(new value(VALUE_TYPE_NUMERICAL, new long double((long double)instances)));
		}
	}
}