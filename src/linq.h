#pragma once

#ifndef LINQ_H
#define LINQ_H

#include <vector>
#include "value.h"
#include "references.h"
#include "garbage.h"

reference_apartment* allocate_array(std::vector<value*> arguments, garbage_collector* gc);
reference_apartment* get_length(std::vector<value*> arguments, garbage_collector* gc);

#endif // !LINQ_H