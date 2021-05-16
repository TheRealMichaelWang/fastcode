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
			runtime::reference_apartment* numabs(std::list<value*> arguments, runtime::garbage_collector* gc);

			//trigonometry
			runtime::reference_apartment* sin(std::list<value*> arguments, runtime::garbage_collector* gc);
			runtime::reference_apartment* cos(std::list<value*> arguments, runtime::garbage_collector* gc);
			runtime::reference_apartment* tan(std::list<value*> arguments, runtime::garbage_collector* gc);
			runtime::reference_apartment* asin(std::list<value*> arguments, runtime::garbage_collector* gc);
			runtime::reference_apartment* acos(std::list<value*> arguments, runtime::garbage_collector* gc);
			runtime::reference_apartment* atan(std::list<value*> arguments, runtime::garbage_collector* gc);

			//exponential
			runtime::reference_apartment* log(std::list<value*> arguments, runtime::garbage_collector* gc);
		}
	}
}
#endif // !MATH_H