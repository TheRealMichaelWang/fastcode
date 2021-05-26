#pragma once

#ifndef LINQ_H
#define LINQ_H

#include <list>
#include "value.h"
#include "references.h"
#include "garbage.h"

namespace fastcode {
	namespace builtins {
		runtime::reference_apartment* allocate_array(std::vector<value*> arguments, runtime::garbage_collector* gc);
		runtime::reference_apartment* get_length(std::vector<value*> arguments, runtime::garbage_collector* gc);
		runtime::reference_apartment* count_instances(std::vector<value*> arguments, runtime::garbage_collector* gc);
		runtime::reference_apartment* get_range(std::vector<value*> arguments, runtime::garbage_collector* gc);
	}
}

#endif // !LINQ_H