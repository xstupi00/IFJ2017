#ifndef EXPR_H
#define EXPR_H

#include "symtable.h"
#include "semantic_control.h"

#define SIZE_TABLE 16
#define FIRST_TOKEN 99

void expression (function_t *act_function, variable_t *l_value);

#endif // EXPR_H