#include <math.h>
#include "builtins.h"
#include "math.h"

reference_apartment* numabs(std::list<value*> arguments, garbage_collector* gc) {
	match_arg_len(arguments, 1);
	match_arg_type(arguments.front(), VALUE_TYPE_NUMERICAL);
	return gc->new_apartment(new value(VALUE_TYPE_NUMERICAL, new long double(abs(*arguments.front()->get_numerical()))));
}

reference_apartment* sine(std::list<value*> arguments, garbage_collector* gc) {
	match_arg_len(arguments, 1);
	match_arg_type(arguments.front(), VALUE_TYPE_NUMERICAL);
	return gc->new_apartment(new value(VALUE_TYPE_NUMERICAL, new long double(sin(*arguments.front()->get_numerical()))));
}

reference_apartment* cosine(std::list<value*> arguments, garbage_collector* gc) {
	match_arg_len(arguments, 1);
	match_arg_type(arguments.front(), VALUE_TYPE_NUMERICAL);
	return gc->new_apartment(new value(VALUE_TYPE_NUMERICAL, new long double(cos(*arguments.front()->get_numerical()))));
}

reference_apartment* tangent(std::list<value*> arguments, garbage_collector* gc) {
	match_arg_len(arguments, 1);
	match_arg_type(arguments.front(), VALUE_TYPE_NUMERICAL);
	return gc->new_apartment(new value(VALUE_TYPE_NUMERICAL, new long double(tan(*arguments.front()->get_numerical()))));
}

reference_apartment* inverse_sine(std::list<value*> arguments, garbage_collector* gc) {
	match_arg_len(arguments, 1);
	match_arg_type(arguments.front(), VALUE_TYPE_NUMERICAL);
	return gc->new_apartment(new value(VALUE_TYPE_NUMERICAL, new long double(asin(*arguments.front()->get_numerical()))));
}

reference_apartment* inverse_cosine(std::list<value*> arguments, garbage_collector* gc) {
	match_arg_len(arguments, 1);
	match_arg_type(arguments.front(), VALUE_TYPE_NUMERICAL);
	return gc->new_apartment(new value(VALUE_TYPE_NUMERICAL, new long double(acos(*arguments.front()->get_numerical()))));
}

reference_apartment* inverse_tan(std::list<value*> arguments, garbage_collector* gc) {
	match_arg_len(arguments, 1);
	match_arg_type(arguments.front(), VALUE_TYPE_NUMERICAL);
	return gc->new_apartment(new value(VALUE_TYPE_NUMERICAL, new long double(atan(*arguments.front()->get_numerical()))));
}