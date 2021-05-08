#include "builtins.h"
#include "collection.h"
#include "linq.h"

reference_apartment* allocate_array(std::list<value*> arguments, garbage_collector* gc) {
	match_arg_len(arguments, 1);
	match_arg_type(arguments.front(), VALUE_TYPE_NUMERICAL);

	if (*arguments.front()->get_numerical() < 0) {
		throw ERROR_INVALID_VALUE_TYPE;
	}

	unsigned long req_size = (unsigned long)*arguments.front()->get_numerical();

	collection* allocated_array = new collection(req_size, gc);
	return allocated_array->get_parent();
}

reference_apartment* get_length(std::list<value*> arguments, garbage_collector* gc) {
	match_arg_len(arguments, 1);
	match_arg_type(arguments.front(), VALUE_TYPE_COLLECTION);

	collection* collection = (class collection*)arguments.front()->ptr;
	return gc->new_apartment(new value(VALUE_TYPE_NUMERICAL, new long double((long double)collection->size)));
}