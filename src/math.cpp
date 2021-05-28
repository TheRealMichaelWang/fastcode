#include <math.h>
#include "builtins.h"
#include "math.h"

namespace fastcode {
	namespace builtins {
		namespace math {
			runtime::reference_apartment* numabs(const std::vector<value*> arguments, runtime::garbage_collector* gc) {
				match_arg_len(arguments, 1);
				match_arg_type(arguments[0], VALUE_TYPE_NUMERICAL);
				return gc->new_apartment(new value(VALUE_TYPE_NUMERICAL, new long double(std::abs(*arguments[0]->get_numerical()))));
			}

			runtime::reference_apartment* sin(const std::vector<value*> arguments, runtime::garbage_collector* gc) {
				match_arg_len(arguments, 1);
				match_arg_type(arguments[0], VALUE_TYPE_NUMERICAL);
				return gc->new_apartment(new value(VALUE_TYPE_NUMERICAL, new long double(::sin(*arguments[0]->get_numerical()))));
			}

			runtime::reference_apartment* cos(const std::vector<value*> arguments, runtime::garbage_collector* gc) {
				match_arg_len(arguments, 1);
				match_arg_type(arguments[0], VALUE_TYPE_NUMERICAL);
				return gc->new_apartment(new value(VALUE_TYPE_NUMERICAL, new long double(::cos(*arguments[0]->get_numerical()))));
			}

			runtime::reference_apartment* tan(const std::vector<value*> arguments, runtime::garbage_collector* gc) {
				match_arg_len(arguments, 1);
				match_arg_type(arguments[0], VALUE_TYPE_NUMERICAL);
				return gc->new_apartment(new value(VALUE_TYPE_NUMERICAL, new long double(::tan(*arguments[0]->get_numerical()))));
			}

			runtime::reference_apartment* asin(const std::vector<value*> arguments, runtime::garbage_collector* gc) {
				match_arg_len(arguments, 1);
				match_arg_type(arguments[0], VALUE_TYPE_NUMERICAL);
				return gc->new_apartment(new value(VALUE_TYPE_NUMERICAL, new long double(::asin(*arguments[0]->get_numerical()))));
			}

			runtime::reference_apartment* acos(const std::vector<value*> arguments, runtime::garbage_collector* gc) {
				match_arg_len(arguments, 1);
				match_arg_type(arguments[0], VALUE_TYPE_NUMERICAL);
				return gc->new_apartment(new value(VALUE_TYPE_NUMERICAL, new long double(::acos(*arguments[0]->get_numerical()))));
			}

			runtime::reference_apartment* atan(const std::vector<value*> arguments, runtime::garbage_collector* gc) {
				match_arg_len(arguments, 1);
				match_arg_type(arguments[0], VALUE_TYPE_NUMERICAL);
				return gc->new_apartment(new value(VALUE_TYPE_NUMERICAL, new long double(::atan(*arguments[0]->get_numerical()))));
			}

			runtime::reference_apartment* log(const std::vector<value*> arguments, runtime::garbage_collector* gc) {
				match_arg_len(arguments, 1);
				match_arg_type(arguments[0], VALUE_TYPE_NUMERICAL);
				return gc->new_apartment(new value(VALUE_TYPE_NUMERICAL, new long double(::log(*arguments[0]->get_numerical()))));
			}
		}
	}
}