#include <math.h>
#include "builtins.h"
#include "math.h"

reference_apartment* abs(std::vector<value*> arguments, garbage_collector* gc) {
	match_arg_len(arguments, 1);
	match_arg_type(arguments[0], VALUE_TYPE_NUMERICAL);
	return gc->new_apartment(new value(VALUE_TYPE_NUMERICAL, new long double(abs(*arguments[0]->get_numerical()))));
}

reference_apartment* sine(std::vector<value*> arguments, garbage_collector* gc) {
	match_arg_len(arguments, 1);
	match_arg_type(arguments[0], VALUE_TYPE_NUMERICAL);
	return gc->new_apartment(new value(VALUE_TYPE_NUMERICAL, new long double(sin(*arguments[0]->get_numerical()))));
}

reference_apartment* cosine(std::vector<value*> arguments, garbage_collector* gc) {
	match_arg_len(arguments, 1);
	match_arg_type(arguments[0], VALUE_TYPE_NUMERICAL);
	return gc->new_apartment(new value(VALUE_TYPE_NUMERICAL, new long double(cos(*arguments[0]->get_numerical()))));
}

reference_apartment* tangent(std::vector<value*> arguments, garbage_collector* gc) {
	match_arg_len(arguments, 1);
	match_arg_type(arguments[0], VALUE_TYPE_NUMERICAL);
	return gc->new_apartment(new value(VALUE_TYPE_NUMERICAL, new long double(tan(*arguments[0]->get_numerical()))));
}