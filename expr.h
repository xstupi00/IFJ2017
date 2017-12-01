#ifndef EXPR_H
#define EXPR_H

#include "semantic_control.h"

///
#define SIZE_TABLE 16	

///
#define FIRST_TOKEN 99

/**
 * @brief      { function_description }
 *
 * @param      act_function  The act function
 * @param      l_value       The l value
 */
void expression (function_t *act_function, variable_t *l_value);

#endif // EXPR_H