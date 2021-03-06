#pragma once

#include <iostream>

#ifndef ERROR_H
#define ERROR_H

#define ERROR_STR_OVERFLOW 1
#define ERROR_UNRECOGNIZED_CONTROL_SEQ 2
#define ERROR_UNEXPECTED_EOF 3
#define ERROR_UNRECOGNIZED_TOK 4
#define ERROR_UNEXPECTED_TOK 5

#define ERROR_INDEX_OUT_OF_RANGE 6
#define ERROR_UNEXPECTED_ARGUMENT_LENGTH 7

#define ERROR_CANNOT_ITERATE_TYPE 8
#define ERROR_INCOMPATIBLE_VALUE_TYPES 9

#define ERROR_MUST_HAVE_STRUCT_TYPE 10
#define ERROR_MUST_HAVE_DOUBLE_TYPE 12

#define ERROR_FUNCTION_ALREADY_DEFINED 13
#define ERROR_VARIABLE_ALREADY_DEFINED 14
#define ERROR_NOT_IN_VAR_CONTEXT 15

#define ERROR_STRUCT_ALREADY_DEFINED 16
#define ERROR_FUNCTION_NOT_FOUND 17
#define ERROR_STRUCT_NOT_FOUND 18

void error_info(int error);

#endif // !ERROR_H
