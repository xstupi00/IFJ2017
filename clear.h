#ifndef CLEAR_H
#define CLEAR_H

#include "stack.h"

extern stack_t * ptr_stack;

void init_ptr_stack();

void * _realloc(void *ptr, size_t new_size, size_t old_size);

void * _malloc(size_t size);

void * _calloc(size_t num, size_t size);

void clear_all();


#endif