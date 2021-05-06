#pragma once

#ifndef TYPES_H
#define TYPES_H

#include <vector>
#include "garbage.h"
#include "references.h"
#include "value.h"

reference_apartment* get_type(std::vector<value*> arguments, garbage_collector* gc);
reference_apartment* to_string(std::vector<value*> arguments, garbage_collector* gc);
reference_apartment* to_numerical(std::vector<value*> arguments, garbage_collector* gc);

#endif // !TYPES_H