///////////////////////////////////////////////////////////////////////////////////
// School:      Brno University of Technology, Faculty of Information Technology //
// Course:      Formal Languages and Compilers                                   //
// Project:     IFJ17                                                            //
// Module:      Semantic control                                                 //
// Authors:     Kristián Liščinský  (xlisci01)                                   //
//              Matúš Liščinský     (xlisci02)                                   //
//              Šimon Stupinský     (xstupi00)                                   //
//              Vladimír Marcin     (xmarci10)                                   //
///////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include "semantic_control.h"
#include "symtable.h"
#include "error.h"
#include "scanner.h"
#include "generate.h"

htab_t *global_symtable;
htab_t *const_symtable;

void init_globals(){
    global_symtable = htab_init(SIZE_HTABLE);
    const_symtable = htab_init(SIZE_HTABLE);
    current_function_name = strInit(STR_INIT);
    current_variable_name = strInit(STR_INIT);
    if ( (label_stack = (stack_t *) malloc(sizeof(stack_t))) == NULL )
        print_err(99);
    S_Init(label_stack);
    initToken();
    list_init();
}

void free_function (function_t * f){
    htab_free(f->local_symtable);
    free_string(f->params);
    if(f->return_var)
        free(f->return_var);
}

variable_t *init_variable(){
    variable_t *new_var = (variable_t*)malloc(sizeof(variable_t));
    if(new_var == NULL)
        print_err(99);
    
    new_var->data_type = 0;
    new_var->constant = false;
    
    return new_var;
}

function_t *init_function(){
    function_t *new_fun = (function_t*)malloc(sizeof(function_t));
    if(new_fun == NULL)
        print_err(99);

    new_fun->return_type = 0;
    new_fun->defined = false;
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
        //fun->return_var = found_function->return_var;
        //i am not sure if free is valid here
        //free(f->data.fun);
        //free_function(f->data.fun);
        f->data.fun = fun;
    }
}

void store_var_in_symtable(function_t *fun, variable_t *var, const char *var_name){
    if(htab_find(fun->local_symtable, var_name) || htab_find(global_symtable, var_name))
        print_err(3);
    if(!strcmp(current_function_name->string, var_name))
        print_err(3);
    htab_item_t *new_var = htab_insert(fun->local_symtable,var_name);
    if(!new_var)
        print_err(99);
    new_var->data.var = var;
    new_var->data.var->data.str = new_var->key;
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

char code_param_type(int type){
    switch(type){
        case INTEGER:
            return 'i';
        case DOUBLE:
            return 'd';
        case STRING:
            return 's';
    }
    return 'e';    
}

variable_t *find_variable(htab_t *symtable, const char *key){
    htab_item_t *item = htab_find(symtable,key);
    if(item == NULL)
        return NULL;
    if(item != NULL && item->is_function)
        return NULL;
    return item->data.var;
}

void store_current_variable_name(token_t *token){
    if(current_variable_name->capacity < token->str->length){
		extendStr(current_variable_name ,token->str->length);
	}
	strcpy(current_variable_name->string,token->str->string);
	current_variable_name->length = token->str->length;
}

void store_current_function_name(token_t *token){
    if(current_function_name->capacity < token->str->length){
		extendStr(current_function_name,token->str->length);
	}
	strcpy(current_function_name->string,token->str->string);
	current_function_name->length = token->str->length;
}
