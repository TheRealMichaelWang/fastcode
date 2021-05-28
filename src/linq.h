#pragma once

#ifndef LINQ_H
#define LINQ_H

#include <list>
#include "value.h"
#include "references.h"
#include "garbage.h"

namespace fastcode {
	namespace builtins {
		runtime::reference_apartment* allocate_array(const std::vector<value*> arguments, runtime::garbage_collector* gc);
		runtime::reference_apartment* get_length(const std::vector<value*> arguments, runtime::garbage_collector* gc);
		runtime::reference_apartment* count_instances(const std::vector<value*> arguments, runtime::garbage_collector* gc);
		runtime::reference_apartment* get_range(const std::vector<value*> arguments, runtime::garbage_collector* gc);
	}
}

#endif // !LINQ_H