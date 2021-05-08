#pragma once

#ifndef LINQ_H
#define LINQ_H

#include <list>
#include "value.h"
#include "references.h"
#include "garbage.h"

reference_apartment* allocate_array(std::list<value*> arguments, garbage_collector* gc);
reference_apartment* get_length(std::list<value*> arguments, garbage_collector* gc);

#endif // !LINQ_H