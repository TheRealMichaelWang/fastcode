#pragma once

#ifndef CONSOLE_H
#define CONSOLE_H

#include <vector>
#include "references.h"
#include "value.h"

reference_apartment* print(std::vector<value*> arguments, garbage_collector* gc);
reference_apartment* print_line(std::vector<value*> arguments, garbage_collector* gc);
reference_apartment* get_input(std::vector<value*> arguments, garbage_collector* gc);

#endif // !IO_H