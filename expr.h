///////////////////////////////////////////////////////////////////////////////////
// School:      Brno University of Technology, Faculty of Information Technology //
// Course:      Formal Languages and Compilers                                   //
// Project:     IFJ17                                                            //
// Module:      Heade file of predence analysis                                  //
// Authors:     Kristián Liščinský  (xlisci01)                                   //
//              Matúš Liščinský     (xlisci02)                                   //
//              Šimon Stupinský     (xstupi00)                                   //
//              Vladimír Marcin     (xmarci10)                                   //
///////////////////////////////////////////////////////////////////////////////////

#ifndef EXPR_H
#define EXPR_H

#include "semantic_control.h"

/// size of precedence table
#define SIZE_TABLE 16	

/// constant for detection begin of expression
#define FIRST_TOKEN 99

/**
 * @brief      The "main function" of this module. From this function are calling
 *             other function for needed works. Function is calling by parser modul. 
 *
 * @param      act_function  The actual function, in which the compiler is located
 * @param      l_value       The left value in case of assignment, else is NULL
 */
void expression (function_t *act_function, variable_t *l_value);

#endif // EXPR_H