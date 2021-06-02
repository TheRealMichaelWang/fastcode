#pragma once

#ifndef VALUE_H
#define VALUE_H

#define VALUE_TYPE_NULL 0
#define VALUE_TYPE_CHAR 1
#define VALUE_TYPE_NUMERICAL 2
#define VALUE_TYPE_HANDLE 3
#define VALUE_TYPE_COLLECTION 4
#define VALUE_TYPE_STRUCT 5

namespace fastcode {
	class value {
	public:
		char type;
		void* ptr;
		value(char type, void* ptr);
		~value();

		//do not call unless value is primitive
		value* clone();

		int hash();

		inline int compare(value* b) {
			if (this->type == VALUE_TYPE_NULL)
				return b->type == VALUE_TYPE_NULL ? 0 : 1;
			else if (b->type == VALUE_TYPE_NULL)
				return this->type == VALUE_TYPE_NULL ? 0 : 1;
			return this->hash() - b->hash();
		}

		//gets the ptr as numerical if ptr is a numerical
		inline long double* get_numerical() {
			return (long double*)ptr;
		}

		inline bool is_primitive() {
			return this->type < VALUE_TYPE_COLLECTION;
		}

		//gets the ptr as char if ptr is a char
		inline char* get_char() {
			return (char*)ptr;
		}
	};
}

#endif // !VALUE_H