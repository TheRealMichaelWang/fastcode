#pragma once

#include "value.h"

#ifndef FLIB_H

//flib standard library functions
value* readLine(var_context* parent_context);
void write(value_array* value);
void writeLine(value_array* value);
value* objSize(value_array* args);
value* newArray(value_array* args, var_context* parent_context);

//general extensions
int block_checksum(char* line);
void str_append(char* to_append, const char* append_with);

char* read_file(const char* filepath);

#define FLIB_H

#endif // !FLIB_H
