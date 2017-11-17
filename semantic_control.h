#ifndef SEM_CONTROL
#define SEM_CONTROL

#include <stdbool.h>
#include "symtable.h"
#include "strlib.h"

extern struct htab_t *global_symtable;

typedef struct{
    int data_type;
    union{
        int i;
        double d;
        char *str;
    }data;
}variable_t;

typedef struct{
    int return_type;
    int defined;
    int params_count;
    int locals_count;
    struct htab_t *local_symtable;
    //add instruction tape
    string_t *params;
    variable_t *return_var;        
}function_t;
#endif