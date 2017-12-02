#ifndef GENERATE_H
#define GENERATE_H

#include "semantic_control.h"

extern struct list_t * list;	///< global instrucion tape as list

#define S "string"
#define I "int"
#define F "float"

/**
 * @brief      structure represents instruction
 */
typedef struct instruction_t{
	char * instr_name;			///< string represents key word with instruction name (operating code)
	struct variable_t * op1;	///< pointer to 1st variable
	struct variable_t * op2;	///< pointer to 2nd variable
	struct variable_t * op3;	///< pointer to 3rd variable
	struct instruction_t* next;	///< ponter to next instruction in list
}instruction_t;

/**
 * @brief      structure of list
 */
typedef struct list_t{
	instruction_t * First;		///< pointer to first item of list
	instruction_t * Last;		///< pointer to last item of list
}list_t;

/**
 * @brief      Function to initialize global list (instruction tape)
 */
void list_init();

/**
 * @brief      Function to print list
 */
void print_list();

/**
 * @brief      Function to insert instruction for default retype according to var
 *
 * @param      var   Pointer to variable
 */
void retype(variable_t * var);

/**
 * @brief      Proccess string to acceptable form for interpret
 *
 * @param      orig_string  Original string
 */
void process_string (char * orig_string);

/**
 * @brief      Function to generate & add instructions to instr.tape for built-in function length
 *
 * @param      l_value  L_value in case of retyping
 */
void length_of_str(variable_t * l_value);

/**
 * @brief      Function to insert instruction to instr.tape 
 *
 * @param      instr  Instruction name (key word)
 * @param      par1   pointer to 1st variable
 * @param      par2   pointer to 2nd variable
 * @param      par3   pointer to 3rd variable
 */
void list_insert(char * instr, variable_t * par1, variable_t * par2, variable_t * par3 );

/**
 * @brief      Function to generate & add instructions to instr.tape for concate two strings
 */
void concat();

/**
 * @brief      Creates a variable.
 *
 * @param      str1      Instruction name (key word)
 * @param[in]  constant  Define if variable is constant or not
 *
 * @return     Pointer to created variable
 */
variable_t * create_var(char *str1, bool constant);

/**
 * @brief      Function to generate & add instructions to instr.tape for built-in function substr
 */
void substr();

/**
 * @brief      Function to generate & add instructions to instr.tape for built-in function asc
 *
 * @param      l_value  L_value in case of retyping
 */
void asc(variable_t * l_value);

/**
 * @brief      Function to generate & add instructions to instr.tape for built-in function chr
 */
void chr();

#endif