#pragma once

#ifndef TYPES_H
#define TYPES_H

#include <list>
#include "garbage.h"
#include "references.h"
#include "value.h"

namespace fastcode {
	namespace builtins {
		runtime::reference_apartment* get_type(std::vector<value*> arguments, runtime::garbage_collector* gc);
		runtime::reference_apartment* to_string(std::vector<value*> arguments, runtime::garbage_collector* gc);
		runtime::reference_apartment* to_numerical(std::vector<value*> arguments, runtime::garbage_collector* gc);
	}
}

#endif // !TYPES_H