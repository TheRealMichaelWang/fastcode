#pragma once

#ifndef ERRORS_H
#define ERRORS_H

//value errors
#define ERROR_INVALID_VALUE_TYPE 1
#define ERROR_INDEX_OUT_OF_RANGE 2
#define ERROR_PROPERTY_NOT_FOUND 3
#define ERROR_MUST_HAVE_NUM_TYPE 4
#define ERROR_MUST_HAVE_STRUCT_TYPE 5
#define ERROR_MUST_HAVE_COLLECTION_TYPE 6
#define ERROR_MUST_HAVE_CHAR_TYPE 7

//variable errors
#define ERROR_VARIABLE_ALREADY_DEFINED 10
#define ERROR_VARIABLE_NOT_DEFINED 11
#define ERROR_CANNOT_DEREFERENCE 12

//identifier errors
#define ERROR_INVALID_ACCESSOR_MODIFIERS 30

//operator erros
#define ERROR_INVALID_BINARY_OPERATOR 41
#define ERROR_INVALID_UNIARY_OPERATOR 42

//lexographical errors
#define ERROR_UNEXPECTED_TOKEN 50
#define ERROR_UNEXPECTED_END 51
#define ERROR_UNRECOGNIZED_TOKEN 52
#define ERROR_UNRECOGNIZED_ESCAPE_SEQ 53

//runtime errors
#define ERROR_OP_NOT_IMPLEMENTED 60
#define ERROR_UNRECOGNIZED_VARIABLE 62
#define ERROR_DIVIDE_BY_ZERO 64
#define ERROR_UNEXPECTED_ARGUMENT_SIZE 65
#define ERROR_UNEXPECTED_BREAK 66

//prototype erros
#define ERROR_STRUCT_PROTO_ALREADY_DEFINED 70
#define ERROR_FUNCTION_PROTO_ALREADY_DEFINED 71
#define ERROR_STRUCT_PROTO_NOT_DEFINED 72
#define ERROR_FUNCTION_PROTO_NOT_DEFINED 73

//import errors
#define ERROR_CANNOT_INCLUDE_FILE 74

inline const char* get_err_info(int err) {
	switch (err)
	{
	case ERROR_INVALID_VALUE_TYPE:
		return "Invalid value type";
	case ERROR_INDEX_OUT_OF_RANGE:
		return "Index out of range";
	case ERROR_PROPERTY_NOT_FOUND:
		return "Property not found";
	case ERROR_MUST_HAVE_NUM_TYPE:
		return "Must have numerical type";
	case ERROR_MUST_HAVE_CHAR_TYPE:
		return "Must have character type";
	case ERROR_MUST_HAVE_COLLECTION_TYPE:
		return "Must have array/collection type";
	case ERROR_MUST_HAVE_STRUCT_TYPE:
		return "Must have structure type";
	case ERROR_INVALID_ACCESSOR_MODIFIERS:
		return "Invalid accessor modifiers";
	case ERROR_INVALID_BINARY_OPERATOR:
		return "Invalid binary operator";
	case ERROR_INVALID_UNIARY_OPERATOR:
		return "Invalid unary operator";
	case ERROR_UNEXPECTED_TOKEN:
		return "Unexpected token";
	case ERROR_UNEXPECTED_END:
		return "Unexpected end";
	case ERROR_UNRECOGNIZED_TOKEN:
		return "Unrecognized token";
	case ERROR_UNRECOGNIZED_ESCAPE_SEQ:
		return "Unrecognized escape sequence";
	case ERROR_OP_NOT_IMPLEMENTED:
		return "Operator not implemented";
	case ERROR_UNRECOGNIZED_VARIABLE:
		return "Unrecognized variable";
	case ERROR_DIVIDE_BY_ZERO:
		return "Cannot divide by zero";
	case ERROR_UNEXPECTED_ARGUMENT_SIZE:
		return "Unexpected argument size";
	case ERROR_UNEXPECTED_BREAK:
		return "Unexpected break statment";
	case ERROR_STRUCT_PROTO_ALREADY_DEFINED:
		return "Structure prototype already defined";
	case ERROR_STRUCT_PROTO_NOT_DEFINED:
		return "Structure prototype not defined";
	case ERROR_FUNCTION_PROTO_ALREADY_DEFINED:
		return "Procedure prototype already defined";
	case ERROR_FUNCTION_PROTO_NOT_DEFINED:
		return "Procedure prototype not defined";
	case ERROR_CANNOT_INCLUDE_FILE:
		return "Cannot include file";
	default:
		return "Unkown error?";
	}
}

#endif // !ERRORS_H