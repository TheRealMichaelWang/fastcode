#include <math.h>
#include "builtins.h"
#include "math.h"

namespace fastcode {
	namespace builtins {
		namespace math {
			runtime::reference_apartment* numabs(std::vector<value*> arguments, runtime::garbage_collector* gc) {
				match_arg_len(arguments, 1);
				match_arg_type(arguments[0], VALUE_TYPE_NUMERICAL);
				return gc->new_apartment(new value(VALUE_TYPE_NUMERICAL, new long double(std::abs(*arguments[0]->get_numerical()))));
			}

			runtime::reference_apartment* sin(std::vector<value*> arguments, runtime::garbage_collector* gc) {
				match_arg_len(arguments, 1);
				match_arg_type(arguments[0], VALUE_TYPE_NUMERICAL);
				return gc->new_apartment(new value(VALUE_TYPE_NUMERICAL, new long double(::sin(*arguments[0]->get_numerical()))));
			}

			runtime::reference_apartment* cos(std::vector<value*> arguments, runtime::garbage_collector* gc) {
				match_arg_len(arguments, 1);
				match_arg_type(arguments[0], VALUE_TYPE_NUMERICAL);
				return gc->new_apartment(new value(VALUE_TYPE_NUMERICAL, new long double(::cos(*arguments[0]->get_numerical()))));
			}

			runtime::reference_apartment* tan(std::vector<value*> arguments, runtime::garbage_collector* gc) {
				match_arg_len(arguments, 1);
				match_arg_type(arguments[0], VALUE_TYPE_NUMERICAL);
				return gc->new_apartment(new value(VALUE_TYPE_NUMERICAL, new long double(::tan(*arguments[0]->get_numerical()))));
			}

			runtime::reference_apartment* asin(std::vector<value*> arguments, runtime::garbage_collector* gc) {
				match_arg_len(arguments, 1);
				match_arg_type(arguments[0], VALUE_TYPE_NUMERICAL);
				return gc->new_apartment(new value(VALUE_TYPE_NUMERICAL, new long double(::asin(*arguments[0]->get_numerical()))));
			}

			runtime::reference_apartment* acos(std::vector<value*> arguments, runtime::garbage_collector* gc) {
				match_arg_len(arguments, 1);
				match_arg_type(arguments[0], VALUE_TYPE_NUMERICAL);
				return gc->new_apartment(new value(VALUE_TYPE_NUMERICAL, new long double(::acos(*arguments[0]->get_numerical()))));
			}

			runtime::reference_apartment* atan(std::vector<value*> arguments, runtime::garbage_collector* gc) {
				match_arg_len(arguments, 1);
				match_arg_type(arguments[0], VALUE_TYPE_NUMERICAL);
				return gc->new_apartment(new value(VALUE_TYPE_NUMERICAL, new long double(::atan(*arguments[0]->get_numerical()))));
			}

			runtime::reference_apartment* log(std::vector<value*> arguments, runtime::garbage_collector* gc) {
				match_arg_len(arguments, 1);
				match_arg_type(arguments[0], VALUE_TYPE_NUMERICAL);
				return gc->new_apartment(new value(VALUE_TYPE_NUMERICAL, new long double(::log(*arguments[0]->get_numerical()))));
			}
		}
	}
}