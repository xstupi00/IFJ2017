#ifndef GENERATE_H
#define GENERATE_H

#include "semantic_control.h"

extern struct list_t * list;	///<

#define S "string"
#define I "int"
#define F "float"

/**
 * @brief      { item_description }
 */
typedef struct instruction_t{
	char * instr_name;			///<
	struct variable_t * op1;	///<
	struct variable_t * op2;	///<
	struct variable_t * op3;	///<
	struct instruction_t* next;	///<
}instruction_t;

/**
 * @brief      { item_description } 
 */
typedef struct list_t{
	instruction_t * First;		///<
	instruction_t * Last;		///<
}list_t;

/**
 * @brief      { function_description }
 */
void list_init();

/**
 * @brief      { function_description }
 */
void print_list();

/**
 * @brief      { function_description }
 *
 * @param      var   The variable
 */
void retype(variable_t * var);

/**
 * @brief      { function_description }
 *
 * @param      orig_string  The original string
 */
void process_string (char * orig_string);

/**
 * @brief      { function_description }
 *
 * @param      l_value  The l value
 */
void length_of_str(variable_t * l_value);

/**
 * @brief      { function_description }
 *
 * @param      instr  The instr
 * @param      par1   The par 1
 * @param      par2   The par 2
 * @param      par3   The par 3
 */
void list_insert(char * instr, variable_t * par1, variable_t * par2, variable_t * par3 );

/**
 * @brief      { function_description }
 */
void concat();

/**
 * @brief      Creates a variable.
 *
 * @param      str1      The string 1
 * @param[in]  constant  The constant
 *
 * @return     { description_of_the_return_value }
 */
variable_t * create_var(char *str1, bool constant);

/**
 * @brief      { function_description }
 */
void substr();

/**
 * @brief      { function_description }
 *
 * @param      l_value  The l value
 */
void asc(variable_t * l_value);

/**
 * @brief      { function_description }
 */
void chr();

/**
 * @brief      { function_description }
 *
 * @param[in]  i     { parameter_description }
 * @param[in]  c     { parameter_description }
 *
 * @return     { description_of_the_return_value }
 */
char * gen_label_name(int i, char c);

/**
 * @brief      { function_description }
 *
 * @param      v     { parameter_description }
 */
void free_var(variable_t * v);

#endif