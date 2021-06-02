#pragma once

#ifndef TYPES_H
#define TYPES_H

#include <list>
#include "garbage.h"
#include "references.h"
#include "value.h"

namespace fastcode {
	namespace builtins {
		runtime::reference_apartment* get_type(const std::vector<value*> arguments, runtime::garbage_collector* gc);
		runtime::reference_apartment* to_string(const std::vector<value*> arguments, runtime::garbage_collector* gc);
		runtime::reference_apartment* to_numerical(const std::vector<value*> arguments, runtime::garbage_collector* gc);
		runtime::reference_apartment* to_char(const std::vector<value*> arguments, runtime::garbage_collector* gc);
	}
}

#endif // !TYPES_H