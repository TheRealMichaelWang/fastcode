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
#define ERROR_INVALID_unary_OPERATOR 42

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

#define ERROR_ABORTED 74

//import errors
#define ERROR_CANNOT_INCLUDE_FILE 75

inline const char* get_err_info(int err) {
	switch (err)
	{
	case ERROR_INVALID_VALUE_TYPE:
		return "Invalid Value Type";
	case ERROR_INDEX_OUT_OF_RANGE:
		return "Index Out of Range";
	case ERROR_PROPERTY_NOT_FOUND:
		return "Property Not Found";
	case ERROR_MUST_HAVE_NUM_TYPE:
		return "Must Have Numerical Type";
	case ERROR_MUST_HAVE_CHAR_TYPE:
		return "Must Have Character Type";
	case ERROR_MUST_HAVE_COLLECTION_TYPE:
		return "Must Have Array/Collection Type";
	case ERROR_MUST_HAVE_STRUCT_TYPE:
		return "Must Have Structure Type";
	case ERROR_INVALID_ACCESSOR_MODIFIERS:
		return "Invalid Accessor Modifiers";
	case ERROR_INVALID_BINARY_OPERATOR:
		return "Invalid Binary Operator";
	case ERROR_INVALID_unary_OPERATOR:
		return "Invalid Unary Operator";
	case ERROR_UNEXPECTED_TOKEN:
		return "Unexpected Token";
	case ERROR_UNEXPECTED_END:
		return "Unexpected End";
	case ERROR_UNRECOGNIZED_TOKEN:
		return "Unrecognized Token";
	case ERROR_UNRECOGNIZED_ESCAPE_SEQ:
		return "Unrecognized Escape Sequence";
	case ERROR_OP_NOT_IMPLEMENTED:
		return "Operator Not Implemented";
	case ERROR_UNRECOGNIZED_VARIABLE:
		return "Unrecognized Variable";
	case ERROR_DIVIDE_BY_ZERO:
		return "Cannot Divide By Zero";
	case ERROR_UNEXPECTED_ARGUMENT_SIZE:
		return "Unexpected Argument Size";
	case ERROR_UNEXPECTED_BREAK:
		return "Unexpected Break Statment";
	case ERROR_STRUCT_PROTO_ALREADY_DEFINED:
		return "Structure Prototype Already Defined";
	case ERROR_STRUCT_PROTO_NOT_DEFINED:
		return "Structure Prototype Not Defined";
	case ERROR_FUNCTION_PROTO_ALREADY_DEFINED:
		return "Procedure Prototype Already Defined";
	case ERROR_FUNCTION_PROTO_NOT_DEFINED:
		return "Procedure Prototype Not Defined";
	case ERROR_ABORTED:
		return "The program has been manually aborted.";
	case ERROR_CANNOT_INCLUDE_FILE:
		return "Cannot Include File";
	default:
		return "Unkown Error";
	}
}

#endif // !ERRORS_H