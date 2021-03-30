#pragma once

#include <map>
#include "value.h"

#ifndef BUILTINS_H
#define BUILTINS_H

//static hashmap of all built-ins

typedef value* (*built_in_function)(value_array* args);

//built-in flib standard library functions

//really basic stl functions
value* readLine(value_array* args);
value* writeLine(value_array* args);
value* write(value_array* value);
value* objSize(value_array* args);
value* newArray(value_array* args);
value* cloneValue(value_array* args);
//value* get_obj_type(value_array* args);
//
////file io related functions
//value* read_text();
//value* write_text();

#endif // !BUILTINS_H
