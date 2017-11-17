#ifndef SEM_CONTROL
#define SEM_CONTROL

#include <stdbool.h>
#include "symtable.h"
#include "strlib.h"

#define PARAM_COUNT 10

extern struct htab_t *global_symtable;

typedef struct variable_t{
    int data_type;
    union{
        int i;
        double d;
        char *str;
    }data;
}variable_t;

typedef struct function_t{
    int return_type;
    bool defined;
    unsigned locals_count;
    struct htab_t *local_symtable;
    //add instruction tape
    string_t *params;
    variable_t *return_var;        
}function_t;

void init_global_symtable();

variable_t *init_variable();

function_t *init_function();

void store_fun_in_symtable(function_t *f,const char *f_name);

void store_var_in_symtable(function_t *f,variable_t *v,const char *v_name);

void check_function_definitions();

#endif