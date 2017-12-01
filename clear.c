#include "stack.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "clear.h"
#include "error.h"

stack_t * ptr_stack;

void init_ptr_stack(){
	if( !(ptr_stack = (stack_t*)malloc(sizeof(stack_t))) )
        print_err(99);
    ptr_stack->top_ptr = NULL;
}

void * _realloc(void *ptr, size_t new_size, size_t old_size){
	
	elem_t * help;

	for(help = ptr_stack->top_ptr; help != NULL; help = help->next_ptr){
		if(help->data == ptr)
			break;
	}

	void * tmp = malloc(new_size);
	memcpy(tmp, ptr, old_size);
	free(help->data);
	help->data = tmp;
	return tmp;
}

void * _malloc(size_t size){

	void * new;
	elem_t * new_element;
	if(!(new = malloc(size)))
		print_err(99);
	memset(new,0,size);
	if ( (new_element = (elem_t *) malloc(sizeof(struct elem_t))) == NULL ) 
		print_err(99);
	
    new_element->data = new;
    new_element->next_ptr = ptr_stack->top_ptr;
    ptr_stack->top_ptr = new_element;

	return new;
}

void * _calloc(size_t num, size_t size){
	void * new;
	elem_t * new_element;
	if(!(new = calloc(num, size)))
		print_err(99);

	if ( (new_element = (elem_t *) malloc(sizeof(struct elem_t))) == NULL ) 
        print_err(99);

    new_element->data = new;
    new_element->next_ptr = ptr_stack->top_ptr;
    ptr_stack->top_ptr = new_element;

	return new;
}

void clear_all(){
	if(!ptr_stack)
		return;
	
	elem_t *tmp = ptr_stack->top_ptr;
    
    for(; tmp != NULL; tmp = ptr_stack->top_ptr){
      	free(tmp->data);
      	ptr_stack->top_ptr = ptr_stack->top_ptr->next_ptr;
      	free(tmp);	
    }
}