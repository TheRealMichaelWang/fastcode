#pragma once
#include "error.h"

void error_info(int error)
{
	switch (error)
	{
	case ERROR_STR_OVERFLOW: {
		std::cout << "Syntax Error: String Overflow."; break;
	}
	case ERROR_UNRECOGNIZED_CONTROL_SEQ: {
		std::cout << "Syntax Error: Unrecognized Control Sequence."; break;
	case ERROR_UNEXPECTED_EOF: {
		std::cout << "Syntax Error: Unexpected End of File."; break;
	}
	case ERROR_UNRECOGNIZED_TOK: {
		std::cout << "Syntax Error: Unrecognized Token."; break;
	}
	case ERROR_UNEXPECTED_TOK: {
		std::cout << "Syntax Error: Unexpected Token."; break;
	}

	case ERROR_INDEX_OUT_OF_RANGE: {
		std::cout << "Runtime Error: Specified index is outside of the selected collections range."; break;
	}
	case ERROR_UNEXPECTED_ARGUMENT_LENGTH: {
		std::cout << "Runtime Error: The specified arguments don't match the arguments requested in the function prototype."; break;
	}
	
	case ERROR_MUST_HAVE_ARRAY_TYPE: {
		std::cout << "Runtime Error: Object must have array type."; break;
	}									
	case ERROR_INCOMPATIBLE_VALUE_TYPES: {
		std::cout << "Runtime Error: Cannot apply operator on incompatible value types."; break;
	}

	case ERROR_MUST_HAVE_STRUCT_TYPE: {
		std::cout << "Runtime Error: Object must have struct type."; break;
	}
	case ERROR_MUST_HAVE_DOUBLE_TYPE: {
		std::cout << "Runtime Error: Object must have double type."; break;
	}

	case ERROR_FUNCTION_ALREADY_DEFINED: {
		std::cout << "Runtime Error: Another function prototype has been defined with the same identifier."; break;
	}
	case ERROR_VARIABLE_ALREADY_DEFINED: {
		std::cout << "Runtime Error: This is an internal error that should not have been invoked."; break;
	}
	case ERROR_STRUCT_ALREADY_DEFINED: {
		std::cout << "Runtime Error: Another structure prototype has been defined with the same identifier"; break;
	}

	case ERROR_FUNCTION_NOT_FOUND: {
		std::cout << "Runtime Error: No such function prototype corresponds to the specified identifier."; break;
	}
	case ERROR_STRUCT_NOT_FOUND: {
		std::cout << "Runtime Error: No such structure prototype corresponds to the specified identifier"; break;
	}
	case ERROR_VAR_NOT_FOUND:{
		std::cout << "Runtime Error: No such static or local variable corresponds to the specified identifier."; break;
	}
	case ERROR_FILE_NOT_FOUND: {
		std::cout << "Runtime Error: The requested file could not be opened."; break;
	}
	case ERROR_IMPORT_SYNTAX_ERR: {
		std::cout << "Runtime Error: A syntax error occured while importing external code during runtime"; break;
	}
	default:
		std::cout << "Unrecognized Error Type";
	}
	}
}