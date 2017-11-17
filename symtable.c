#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "symtable.h"
#include "error.h"

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