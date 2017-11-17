#include <stdio.h>
#include <stdlib.h>
#include "semantic_control.h"
#include "symtable.h"
#include "error.h"

htab_t *global_symtable;

void init_global_symtable(){
    global_symtable = htab_init(SIZE_HTABLE);
}

variable_t *init_variable(){
    variable_t *new_var = (variable_t*)malloc(sizeof(variable_t));
    if(new_var == NULL)
        print_err(99);
    
    new_var->data_type = 0;
    
    return new_var;
}

function_t *init_function(){
    function_t *new_fun = (function_t*)malloc(sizeof(function_t));
    if(new_fun == NULL)
        print_err(99);

    new_fun->return_type = 0;
    new_fun->defined = false;
    new_fun->locals_count = 0;
    new_fun->local_symtable = htab_init(SIZE_HTABLE);
    new_fun->params = strInit(PARAM_COUNT);
    new_fun->return_var = init_variable();
    
    return new_fun;
}   

void store_fun_in_symtable(function_t *fun, const char *fun_name){
    htab_item_t *f = htab_find(global_symtable,fun_name);
    if(f == NULL){
        htab_item_t *new_fun = htab_insert(global_symtable,fun_name);
        new_fun->data.fun = fun;
        new_fun->is_function = true;
    }
    else{
        function_t *found_function = f->data.fun;
        unsigned long i;
        if(found_function->defined)
            print_err(3);
        for(i = 0; i<fun->params->length; i++){
            if( found_function->params->string[i] != fun->params->string[i])
                break;
        }
        if(i != found_function->params->length || i != fun->params->length)
            print_err(3);
        if(found_function->return_type != fun->return_type)
            print_err(3);
        //check it
        fun->return_var = found_function->return_var;
        f->data.fun = fun;
    }
}

void store_var_in_symtable(function_t *fun, variable_t *var, const char *var_name){
    if(htab_find(fun->local_symtable, var_name) || htab_find(global_symtable, var_name))
        print_err(3);
    
    htab_item_t *new_var = htab_insert(fun->local_symtable,var_name);
    new_var->data.var = var;
    new_var->is_function = false;
}

void check_function_definitions(){
    function_t *f;
    for(unsigned i = 0; i < global_symtable->arr_size; i++){
        if(global_symtable->ptr[i] == NULL)
            continue;
        for(htab_item_t *tmp = global_symtable->ptr[i]; tmp != NULL; tmp = tmp->next){
            f = tmp->data.fun;
            if(!f->defined)
                print_err(3);
        }
    }    
}
