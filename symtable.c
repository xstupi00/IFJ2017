///////////////////////////////////////////////////////////////////////////////////
// School:      Brno University of Technology, Faculty of Information Technology //
// Course:      Formal Languages and Compilers                                   //
// Project:     IFJ17                                                            //
// Module:      Hash Table                                                       //
// Authors:     Kristián Liščinský  (xlisci01)                                   //
//              Matúš Liščinský     (xlisci02)                                   //
//              Šimon Stupinský     (xstupi00)                                   //
//              Vladimír Marcin     (xmarci10)                                   //
///////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "symtable.h"
#include "error.h"
#include "scanner.h"

unsigned hash_function(const char *key){
    unsigned int h = 0;
    const unsigned char *p;
    for(p = (const unsigned char*)key; *p != '\0'; p++){
        h = 65599*h + *p;
    }
    return h;
}

htab_t* htab_init(unsigned size){
    htab_t *t = (htab_t*) malloc(sizeof(htab_t) + size * sizeof(htab_item_t*));
    if(t == NULL){
        print_err(99);
    }
    t->arr_size = size;
    t->n = 0;
    for(unsigned i = 0; i < t->arr_size; i++){
        t->ptr[i] = NULL;
    }
    return t;
}

htab_item_t* htab_find(htab_t *table, const char *key){
    if(table == NULL || key == NULL)
        return NULL;
    unsigned index = hash_function(key) % table->arr_size;
    htab_item_t *tmp = NULL;

    for(tmp = table->ptr[index]; tmp != NULL; tmp = tmp->next){
        if(!(strcmp(tmp->key,key))){
            return tmp;
        }
    }
    return NULL;
}

htab_item_t* htab_insert(htab_t *table, const char *key){
    if(table == NULL || key == NULL)
        return NULL;
    
    unsigned index = hash_function(key) % table->arr_size;
    htab_item_t *tmp = NULL; 
    htab_item_t *new_item = (htab_item_t*)malloc(sizeof(htab_item_t));
    if(new_item == NULL)
        print_err(99);
    
    new_item->key = malloc((strlen(key)+1)*sizeof(char));
    if(new_item->key == NULL){
        free(new_item);
        print_err(99);
    }
    strcpy(new_item->key, key);
    
    tmp = table->ptr[index];
    new_item->next = tmp;
    table->ptr[index] = new_item;

    table->n+=1;

    return new_item;    
}

void htab_remove(htab_t *table, const char *key){
    if(table == NULL || key == NULL)
        return; 
    
    unsigned index = hash_function(key) % table->arr_size;
    htab_item_t *tmp = NULL;
    htab_item_t *prev = NULL;
    htab_item_t *next = NULL;

    for(tmp = table->ptr[index]; tmp != NULL;){
        next = tmp->next;
        if(!(strcmp(key,tmp->key))){
            if(tmp == table->ptr[index])
                table->ptr[index] = tmp->next;
            else
                prev->next = tmp->next;
            free(tmp->key);
            free(tmp);
            table->n-=1;
            return;
        }        
        prev = tmp;
        tmp = next;
    }
}

void htab_clear(htab_t *table){
    if(table == NULL)
        return;
    htab_item_t *tmp = NULL;

    for(unsigned i = 0; i < table->arr_size; i++){
        if(table->ptr[i] == NULL)
            continue;
        while(table->ptr[i] != NULL){
            tmp = table->ptr[i];
            table->ptr[i] = table->ptr[i]->next;
            if(tmp->key != NULL)
                free(tmp->key);
            free(tmp);
        }
    }
    table->n = 0;
}

void htab_free(htab_t *table){
    if(table == NULL)
        return;
    htab_clear(table);
    free(table);
}

//debug

void htab_foreach(htab_t *table, void(*func)(char*,bool,void*)){
    for(unsigned i = 0; i < table->arr_size; i++){
        for(htab_item_t *tmp = table->ptr[i]; tmp != NULL; tmp = tmp->next){
            printf("index: %u\n",i);
            if(tmp->is_function)
                func(tmp->key, tmp->is_function, tmp->data.fun);
            else    
                func(tmp->key, tmp->is_function, tmp->data.var);
        }
    }
}

void htab_print(char *key, bool f, void *data){
    printf("id:\t\t%s\n",key);
    if(!f){
        printf("VARIABLE\n");
        printf("data type:\t%d\n",((variable_t*)data)->data_type);
        switch(((variable_t*)data)->data_type){
            case INTEGER:
                printf("data:\t\t%d\n",((variable_t*)data)->data.i);
                break;
            case DOUBLE:
                printf("data:\t\t%f\n",((variable_t*)data)->data.d);
                break;
            case STRING:
                printf("data:\t\t%s\n",((variable_t*)data)->data.str);
                break;
        }
    }
    else{
        printf("FUNCTION\n");
        printf("return type:\t%d\n",((function_t*)data)->return_type);
        printf("is defined:\t%d\n",((function_t*)data)->defined);
        printf("params count:\t%lu\n",((function_t*)data)->params->length);
        printf("params:\t\t%s\n",((function_t*)data)->params->string);
        if(((function_t*)data)->local_symtable->n){
            htab_foreach(((function_t*)data)->local_symtable,htab_print);
        }
    }
}