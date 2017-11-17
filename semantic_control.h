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
    unsigned params_count;
    unsigned locals_count;
    struct htab_t *local_symtable;
    //add instruction tape
    string_t *params;
    variable_t *return_var;        
}function_t;

void init_global_symtable();

#endif