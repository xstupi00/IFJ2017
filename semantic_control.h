#ifndef SEM_CONTROL
#define SEM_CONTROL

#include <stdbool.h>
#include "symtable.h"
#include "strlib.h"
#include "scanner.h"
#include "stack.h"

#define PARAM_COUNT 10

extern htab_t *global_symtable;
extern htab_t *const_symtable;
extern string_t *current_function_name;
extern string_t *current_variable_name;
extern stack_t *label_stack;

typedef struct variable_t{
    int data_type;
    union{
        int i;
        double d;
        char *str;
    }data;
    bool constant;
}variable_t;

typedef struct function_t{
    int return_type;
    bool defined;
    struct htab_t *local_symtable;
    string_t *params;
    variable_t *return_var;        
}function_t;

void init_globals();

variable_t *init_variable();

function_t *init_function();

void store_fun_in_symtable(function_t *f,const char *f_name);

void store_var_in_symtable(function_t *f,variable_t *v,const char *v_name);

void check_function_definitions();

variable_t *find_variable(struct htab_t* symtable, const char *key);

char code_param_type(int type);

void store_current_variable_name(struct token_t *token);

void store_current_function_name(struct token_t *token);

void free_function(function_t * f);

#endif