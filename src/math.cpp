#include <math.h>
#include "builtins.h"
#include "math.h"

namespace fastcode {
	namespace builtins {
		namespace math {
			runtime::reference_apartment* numabs(std::list<value*> arguments, runtime::garbage_collector* gc) {
				match_arg_len(arguments, 1);
				match_arg_type(arguments.front(), VALUE_TYPE_NUMERICAL);
				return gc->new_apartment(new value(VALUE_TYPE_NUMERICAL, new long double(std::abs(*arguments.front()->get_numerical()))));
			}

			runtime::reference_apartment* sin(std::list<value*> arguments, runtime::garbage_collector* gc) {
				match_arg_len(arguments, 1);
				match_arg_type(arguments.front(), VALUE_TYPE_NUMERICAL);
				return gc->new_apartment(new value(VALUE_TYPE_NUMERICAL, new long double(::sin(*arguments.front()->get_numerical()))));
			}

			runtime::reference_apartment* cos(std::list<value*> arguments, runtime::garbage_collector* gc) {
				match_arg_len(arguments, 1);
				match_arg_type(arguments.front(), VALUE_TYPE_NUMERICAL);
				return gc->new_apartment(new value(VALUE_TYPE_NUMERICAL, new long double(::cos(*arguments.front()->get_numerical()))));
			}

			runtime::reference_apartment* tan(std::list<value*> arguments, runtime::garbage_collector* gc) {
				match_arg_len(arguments, 1);
				match_arg_type(arguments.front(), VALUE_TYPE_NUMERICAL);
				return gc->new_apartment(new value(VALUE_TYPE_NUMERICAL, new long double(::tan(*arguments.front()->get_numerical()))));
			}

			runtime::reference_apartment* asin(std::list<value*> arguments, runtime::garbage_collector* gc) {
				match_arg_len(arguments, 1);
				match_arg_type(arguments.front(), VALUE_TYPE_NUMERICAL);
				return gc->new_apartment(new value(VALUE_TYPE_NUMERICAL, new long double(::asin(*arguments.front()->get_numerical()))));
			}

			runtime::reference_apartment* acos(std::list<value*> arguments, runtime::garbage_collector* gc) {
				match_arg_len(arguments, 1);
				match_arg_type(arguments.front(), VALUE_TYPE_NUMERICAL);
				return gc->new_apartment(new value(VALUE_TYPE_NUMERICAL, new long double(::acos(*arguments.front()->get_numerical()))));
			}

			runtime::reference_apartment* atan(std::list<value*> arguments, runtime::garbage_collector* gc) {
				match_arg_len(arguments, 1);
				match_arg_type(arguments.front(), VALUE_TYPE_NUMERICAL);
				return gc->new_apartment(new value(VALUE_TYPE_NUMERICAL, new long double(::atan(*arguments.front()->get_numerical()))));
			}

			runtime::reference_apartment* log(std::list<value*> arguments, runtime::garbage_collector* gc) {
				match_arg_len(arguments, 1);
				match_arg_type(arguments.front(), VALUE_TYPE_NUMERICAL);
				return gc->new_apartment(new value(VALUE_TYPE_NUMERICAL, new long double(::log(*arguments.front()->get_numerical()))));
			}
		}
	}
}