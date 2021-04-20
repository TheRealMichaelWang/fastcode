#pragma once

#ifndef OPERATORS_H
#define OPERATORS_H

value* applyUniaryOp(char type, unique_reference* value);
value* applyBinaryOp(char type, unique_reference* a, unique_reference* b);

#endif // !OPERATORS_H