#pragma once

#ifndef IO_H
#define IO_H

#include <list>
#include "tokens.h"
#include "references.h"
#include "value.h"

reference_apartment* print(std::list<value*> arguments, garbage_collector* gc);
reference_apartment* print_line(std::list<value*> arguments, garbage_collector* gc);
reference_apartment* get_input(std::list<value*> arguments, garbage_collector* gc);

reference_apartment* file_read_text(std::list<value*> args, garbage_collector* gc);
reference_apartment* file_write_text(std::list<value*> args, garbage_collector* gc);

void handle_syntax_err(int syntax_error, unsigned int pos, const char* source);
void handle_runtime_err(int runtime_error, token* err_tok);
void print_call_stack(std::stack<function_prototype*> call_stack);

#endif // !IO_H