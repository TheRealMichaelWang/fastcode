#pragma once

#ifndef MATH_H
#define MATH_H

#include <vector>
#include "garbage.h"
#include "references.h"
#include "value.h"

reference_apartment* abs(std::vector<value*> arguments, garbage_collector* gc);
reference_apartment* sine(std::vector<value*> arguments, garbage_collector* gc);
reference_apartment* cosine(std::vector<value*> arguments, garbage_collector* gc);
reference_apartment* tangent(std::vector<value*> arguments, garbage_collector* gc);

#endif // !MATH_H