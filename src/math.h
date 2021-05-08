#pragma once

#ifndef MATH_H
#define MATH_H

#include <list>
#include "garbage.h"
#include "references.h"
#include "value.h"

reference_apartment* abs(std::list<value*> arguments, garbage_collector* gc);
reference_apartment* sine(std::list<value*> arguments, garbage_collector* gc);
reference_apartment* cosine(std::list<value*> arguments, garbage_collector* gc);
reference_apartment* tangent(std::list<value*> arguments, garbage_collector* gc);
reference_apartment* inverse_sine(std::list<value*> arguments, garbage_collector* gc);
reference_apartment* inverse_cosine(std::list<value*> arguments, garbage_collector* gc);
reference_apartment* inverse_tan(std::list<value*> arguments, garbage_collector* gc);
#endif // !MATH_H