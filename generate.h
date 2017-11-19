#ifndef GENERATE_H
#define GENERATE_H
#include "semantic_control.h"
extern struct list_t * list;

typedef struct instruction_t{
	char instr_name [15];
	struct variable_t * op1;
	struct variable_t * op2;
	struct variable_t * op3;
	struct instruction_t* next;
}instruction_t;

typedef struct list_t{
	instruction_t * First;
	instruction_t * Last;
}list_t;

void list_init();
void retype(variable_t * var);
void process_string (char * orig_string);
void length_of_str(variable_t * var);
void list_insert(char * instr, variable_t * par1, variable_t * par2, variable_t * par3 );
void concat(variable_t * op1, variable_t * op2);
variable_t * create_var(int i);

#endif