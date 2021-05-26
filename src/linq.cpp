#include "builtins.h"
#include "collection.h"
#include "linq.h"

namespace fastcode {
	namespace builtins {
		runtime::reference_apartment* allocate_array(std::vector<value*> arguments, runtime::garbage_collector* gc) {
			match_arg_len(arguments, 1);
			match_arg_type(arguments[0], VALUE_TYPE_NUMERICAL);

			if (*arguments[0]->get_numerical() < 0) {
				throw ERROR_INVALID_VALUE_TYPE;
			}

			unsigned long req_size = (unsigned long)*arguments[0]->get_numerical();

			runtime::collection* allocated_array = new runtime::collection(req_size, gc);
			return allocated_array->get_parent_ref();
		}

		runtime::reference_apartment* get_length(std::vector<value*> arguments, runtime::garbage_collector* gc) {
			match_arg_len(arguments, 1);
			match_arg_type(arguments[0], VALUE_TYPE_COLLECTION);

			runtime::collection* collection = (runtime::collection*)arguments[0]->ptr;
			return gc->new_apartment(new value(VALUE_TYPE_NUMERICAL, new long double((long double)collection->size)));
		}

		runtime::reference_apartment* count_instances(std::vector<value*> arguments, runtime::garbage_collector* gc) {
			match_arg_len(arguments, 2);
			match_arg_type(arguments[0], VALUE_TYPE_COLLECTION);

			runtime::collection* collection = (runtime::collection*)arguments[0]->ptr;
			unsigned int instances = 0;

			for (size_t i = 0; i < collection->size; i++)
			{
				if (collection->get_value(i)->compare(arguments[1]) == 0)
					instances++;
			}
			return gc->new_apartment(new value(VALUE_TYPE_NUMERICAL, new long double((long double)instances)));
		}

		runtime::reference_apartment* get_range(std::vector<value*> arguments, runtime::garbage_collector* gc) {
			long double start = 0;
			long double step = 1;
			long double stop;
			if (arguments.size() == 1)
				stop = *arguments[0]->get_numerical();
			else if (arguments.size() == 2) {
				start = *arguments[0]->get_numerical();
				stop = *arguments[1]->get_numerical();
			}
			else if (arguments.size() == 3) {
				start = *arguments[0]->get_numerical();
				stop = *arguments[1]->get_numerical();
				step = *arguments[2]->get_numerical();
			}
			else
				throw ERROR_UNEXPECTED_ARGUMENT_SIZE;

			unsigned long size = abs(stop - start) / abs(step);

			runtime::collection* range = new runtime::collection(size, gc);

			unsigned int j = 0;

			for (long double i = start; step > 0 ? i < stop : i > stop; i += step)
				range->set_value(j++, new value(VALUE_TYPE_NUMERICAL, new long double(i)));
			
			return range->get_parent_ref();
		}
	}
}