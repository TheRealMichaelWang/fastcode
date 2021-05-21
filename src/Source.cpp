#include "runtime.h"
#include "builtins.h"
#include <fstream>
#include <iostream>

//standard libraries
#include "math.h"

using namespace fastcode;

bool stop = false;

runtime::reference_apartment* quit_repl(std::list<value*> args, runtime::garbage_collector* gc) {
	stop = true;
	return gc->new_apartment(new value(VALUE_TYPE_NULL, nullptr));
}

runtime::reference_apartment* get_help(std::list<value*> args, runtime::garbage_collector* gc) {
	std::cout << "Welcome to FastCode!\n\n\tIf this is your first time using FastCode, we urge you to read the documentation at https://github.com/TheRealMichaelWang/fastcode/wiki, or at least check out the section labled ,A Quick Guide, before reading the entirety of this document."<<std::endl;
	return gc->new_apartment(new value(VALUE_TYPE_CHAR, new char(' ')));
}

unsigned int code_checksum(const char* code) {
	int brackets = 0;
	int braces = 0;
	int params = 0;
	for (size_t i = 0; i < std::strlen(code); i++)
	{
		switch (code[i]) {
		case '{': braces++; break;
		case '}': braces--; break;
		case '[': brackets++; break;
		case ']': brackets--; break;
		case '(': params++; break;
		case ')': params--; break;
		}
	}
	return std::abs(braces) + std::abs(brackets) + std::abs(params);
}

char* str_append(char* buf, char* toappend) {
	char* newbuf = new char[strlen(buf) + strlen(toappend) + 1];
	for (size_t i = 0; i < strlen(buf); i++)
		newbuf[i] = buf[i];
	for (size_t i = 0; i < strlen(toappend); i++)
		newbuf[i + strlen(buf)] = toappend[i];
	newbuf[strlen(buf) + strlen(toappend)] = 0;
	delete[] buf;
	return newbuf;
}

int main(int argc, char** argv) {
	const char* working_dir = argv[0];
	runtime::interpreter interpreter;

	interpreter.new_constant("pi@math", new value(VALUE_TYPE_NUMERICAL, new long double(3.1415926)));
	interpreter.new_constant("e@math", new value(VALUE_TYPE_NUMERICAL, new long double(2.71828182)));

	interpreter.import_func("quit", quit_repl);
	interpreter.import_func("help", get_help);

	interpreter.import_func("abs", builtins::math::numabs);
	interpreter.import_func("sin@math", builtins::math::sin);
	interpreter.import_func("cos@math", builtins::math::cos);
	interpreter.import_func("tan@math", builtins::math::tan);
	interpreter.import_func("asin@math", builtins::math::asin);
	interpreter.import_func("acos@math", builtins::math::acos);
	interpreter.import_func("atan@math", builtins::math::atan);
	interpreter.import_func("log@math", builtins::math::log);

	if (argc > 1) {
		std::ifstream infile(argv[1], std::ifstream::binary);
		if (!infile.is_open()) {
			std::cout << "Cannot open source file \"" << argv[1] << "\".";
			return 1;
		}
		infile.seekg(0, std::ios::end);
		unsigned int buffer_length = infile.tellg();
		infile.seekg(0, std::ios::beg);
		char* buffer = new char[buffer_length + 1];
		infile.read(buffer, buffer_length);
		infile.close();
		buffer[buffer_length] = '\0';
		long double exit_code = interpreter.run(buffer, false);
		delete[] buffer;
		if (exit_code != 0)
			return (int)exit_code;
	}
	else {
		std::cout << "FastCode [Version 2.0, written and designed by Michael Wang]" << std::endl << "Type \"quit()\" to exit the REPL, type \"help()\" for help. " << std::endl << "For more information or documentation, go to https://github.com/TheRealMichaelWang/fastcode/wiki." << std::endl;

		while (!stop)
		{
			std::cout << std::endl;
			char* buf = nullptr;
			while (true)
			{
				std::cout << ">>> ";
				char* line = new char[250];
				std::cin.getline(line, 250);
				unsigned int line_size = strlen(line);
				line[line_size++] = '\n';
				line[line_size] = 0;
				if (buf == nullptr)
					buf = line;
				else {
					buf = str_append(buf, line);
					delete[] line;
				}
				if (code_checksum(buf) == 0)
					break;
			}
			interpreter.run(buf, true);
			delete[] buf;
		}
	}
	return 0;
}