#include "runtime.h"
#include "builtins.h"
#include <fstream>
#include <iostream>

//standard libraries
#include "math.h"

bool stop = false;

reference_apartment* quit_repl(std::vector<value*> args, garbage_collector* gc) {
	stop = true;
	return gc->new_apartment(new value(VALUE_TYPE_NULL, nullptr));
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
	interpreter interpreter;

	interpreter.new_constant("pi", new value(VALUE_TYPE_NUMERICAL, new long double(3.1415926)));
	interpreter.new_constant("E", new value(VALUE_TYPE_NUMERICAL, new long double(2.71828182)));

	interpreter.import_func("quit", quit_repl);
	interpreter.import_func("sin", sine);
	interpreter.import_func("cos", cosine);
	interpreter.import_func("tan", tangent);
	interpreter.import_func("asin", inverse_sine);
	interpreter.import_func("acos", inverse_cosine);
	interpreter.import_func("atan", inverse_tan);

	if (argc > 1) {
		std::ifstream infile(argv[1], std::ifstream::binary);
		if (!infile.is_open()) {
			std::cout << "Cannot open source file \"" << argv[1] << "\".";
			return 1;
		}
		infile.seekg(0, std::ios::end);
		int buffer_length = infile.tellg();
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
		std::cout << "FastCode [Version 2.0, written and designed by Michael Wang]" << std::endl << "Type \"quit()\" to exit the REPL." << std::endl;

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