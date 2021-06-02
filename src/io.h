#pragma once

#ifndef IO_H
#define IO_H

#include <list>
#include "tokens.h"
#include "references.h"
#include "value.h"

namespace fastcode {
	namespace builtins {
		runtime::reference_apartment* print(const std::vector<value*> arguments, runtime::garbage_collector* gc);
		runtime::reference_apartment* print_line(const std::vector<value*> arguments, runtime::garbage_collector* gc);
		runtime::reference_apartment* get_input(const std::vector<value*> arguments, runtime::garbage_collector* gc);

		runtime::reference_apartment* file_read_text(const std::vector<value*> arguments, runtime::garbage_collector* gc);
		runtime::reference_apartment* file_write_text(const std::vector<value*> arguments, runtime::garbage_collector* gc);

		runtime::reference_apartment* system_call(const std::vector<value*> arguments, runtime::garbage_collector* gc);
	}
	void handle_syntax_err(int syntax_error, unsigned int pos, const char* source);
	void handle_runtime_err(int runtime_error, parsing::token* err_tok);
	void print_call_stack(std::stack<parsing::function_prototype*> call_stack);
}

#endif // !IO_H