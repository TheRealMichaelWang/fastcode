#pragma once

#ifndef MATH_H
#define MATH_H

#include <list>
#include "garbage.h"
#include "references.h"
#include "value.h"

namespace fastcode {
	namespace builtins {
		namespace math {
			runtime::reference_apartment* numabs(const std::vector<value*> arguments, runtime::garbage_collector* gc);

			//trigonometry
			runtime::reference_apartment* sin(const std::vector<value*> arguments, runtime::garbage_collector* gc);
			runtime::reference_apartment* cos(const std::vector<value*> arguments, runtime::garbage_collector* gc);
			runtime::reference_apartment* tan(const std::vector<value*> arguments, runtime::garbage_collector* gc);
			runtime::reference_apartment* asin(const std::vector<value*> arguments, runtime::garbage_collector* gc);
			runtime::reference_apartment* acos(const std::vector<value*> arguments, runtime::garbage_collector* gc);
			runtime::reference_apartment* atan(const std::vector<value*> arguments, runtime::garbage_collector* gc);

			//exponential
			runtime::reference_apartment* log(const std::vector<value*> arguments, runtime::garbage_collector* gc);
		}
	}
}
#endif // !MATH_H