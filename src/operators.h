#pragma once

#ifndef OPERATORS_H
#define OPERATORS_H

#include "tokens.h"
#include "value.h"
#include "references.h"

//operator types
#define OP_AND 0 + STD_OP_TOK_OFFSET
#define OP_OR 1 + STD_OP_TOK_OFFSET

#define OP_EQUALS 10 + STD_OP_TOK_OFFSET
#define OP_NOT_EQUAL 11 + STD_OP_TOK_OFFSET
#define OP_LESS 12 + STD_OP_TOK_OFFSET
#define OP_MORE 13 + STD_OP_TOK_OFFSET
#define OP_LESS_EQUAL 14 + STD_OP_TOK_OFFSET
#define OP_MORE_EQUAL 15 + STD_OP_TOK_OFFSET

#define OP_ADD 20 + STD_OP_TOK_OFFSET
#define OP_SUBTRACT 21 + STD_OP_TOK_OFFSET

#define OP_MULTIPLY 30 + STD_OP_TOK_OFFSET
#define OP_DIVIDE 31 + STD_OP_TOK_OFFSET
#define OP_MODULOUS 32 + STD_OP_TOK_OFFSET

#define OP_POWER 40 + STD_OP_TOK_OFFSET

#define OP_INVERT 50 + STD_OP_TOK_OFFSET
#define OP_NEGATE 51 + STD_OP_TOK_OFFSET
#define OP_INCRIMENT 52 + STD_OP_TOK_OFFSET
#define OP_DECRIMENT 53 + STD_OP_TOK_OFFSET

namespace fastcode {
	namespace parsing {
		struct binary_operator_token : token {
			token* left;
			token* right;
			unsigned char op;
			binary_operator_token(token* left, token* right, unsigned char op);
			~binary_operator_token();
		
			void print();
		};

		struct unary_operator_token : token {
			token* value;
			unsigned char op;

			unary_operator_token(token* value, unsigned char op);
			~unary_operator_token();

			void print();
		};

		inline unsigned char get_operator_precedence(char op_type) {
			return (op_type - STD_OP_TOK_OFFSET) / 10;
		}

		inline bool is_op_tok(unsigned char op_type) {
			return op_type >= STD_OP_TOK_OFFSET && op_type <= STD_OP_TOK_OFFSET + 53;
		}

		inline bool is_binary_operator(unsigned char op_type) {
			return (op_type - STD_OP_TOK_OFFSET) < 50;
		}

		inline bool is_unary_operator(unsigned char op_type) {
			return (op_type - STD_OP_TOK_OFFSET) >= 50;
		}
	}

	namespace runtime {
		value* evaluate_binary_op(unsigned char op, value* a, value* b);
		value* evaluate_unary_op(unsigned char op, value* a);
	}
}
#endif // !OPERATORS_H