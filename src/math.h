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
			runtime::reference_apartment* numabs(std::vector<value*> arguments, runtime::garbage_collector* gc);

			//trigonometry
			runtime::reference_apartment* sin(std::vector<value*> arguments, runtime::garbage_collector* gc);
			runtime::reference_apartment* cos(std::vector<value*> arguments, runtime::garbage_collector* gc);
			runtime::reference_apartment* tan(std::vector<value*> arguments, runtime::garbage_collector* gc);
			runtime::reference_apartment* asin(std::vector<value*> arguments, runtime::garbage_collector* gc);
			runtime::reference_apartment* acos(std::vector<value*> arguments, runtime::garbage_collector* gc);
			runtime::reference_apartment* atan(std::vector<value*> arguments, runtime::garbage_collector* gc);

			//exponential
			runtime::reference_apartment* log(std::vector<value*> arguments, runtime::garbage_collector* gc);
		}
	}
}
#endif // !MATH_H