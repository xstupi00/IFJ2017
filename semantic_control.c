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
    new_fun->params_count = 0;
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
        
        //redefinition
        if(found_function->defined)
            print_err(3);

        for(i = 0; i<fun->params->length; i++){
            if( found_function->params->string[i] !=
                fun->params->string[i]){
                break;
                }
            }
        //count or types of parameters mismatch
        if(i != found_function->params_count)
            print_err(3);
        
        //incompatible types in definition and declaration
        if(found_function->return_type != fun->return_type)
            print_err(3);

        fun->return_var = found_function->return_var;
        
        
    }
}

void store_var_in_symtable(function_t *fun, variable_t *var){

}
