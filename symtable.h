#ifndef SYMTABLE_H
#define SYMTABLE_H

#include <stdbool.h>
//#include "semantic_control.h"

#define SIZE_HTABLE 97

typedef struct htab_listitem{
    char *key;
    bool is_function;
    union{
        struct variable_t *var;
        struct function_t *fun;
    }data;
    struct htab_listitem *next;
}htab_item_t;

typedef struct htab_t{
    unsigned arr_size;      //size of table 
    unsigned n;             //number of items 
    htab_item_t *ptr[];   
}htab_t;

unsigned hash_function(const char *key);

htab_t* htab_init(unsigned size);

htab_item_t* htab_insert(htab_t *table, const char *key);

htab_item_t* htab_find(htab_t *table, const char *key);

void htab_remove(htab_t *table, const char *key);

void htab_clear(htab_t *table);

void htab_free(htab_t *table);

void htab_foreach(htab_t *table,void(*func)(char*,bool,void*));

void htab_print(char *key,bool f, void* data);

#endif