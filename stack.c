///////////////////////////////////////////////////////////////////////////////////
// School:      Brno University of Technology, Faculty of Information Technology //
// Course:      Formal Languages and Compilers                                   //
// Project:     IFJ17                                                            //
// Module:      Stack                                                            //
// Authors:     Kristián Liščinský  (xlisci01)                                   //
//              Matúš Liščinský     (xlisci02)                                   //
//              Šimon Stupinský     (xstupi00)                                   //
//              Vladimír Marcin     (xmarci10)                                   //
///////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include "stack.h"
#include "error.h"
#include "clear.h"

/// replace malloc calls with our malloc wrapper
#define malloc(size) _malloc(size)

stack_t* S_Init () {
    stack_t* s;
    if( !(s = (stack_t*)malloc(sizeof(stack_t))) )
        print_err(99);
    s->top_ptr = NULL;
    return s;
}

void S_Push (stack_t *s, void* d) {

    elem_t *new_element; 
    if ( (new_element = (elem_t *) malloc(sizeof(struct elem_t))) == NULL ) 
        print_err(99);

    /// insert new element on top of stack
    new_element->data = d;
    new_element->next_ptr = s->top_ptr;
    s->top_ptr = new_element;
}

void S_Pop (stack_t *s) {

    if ( s->top_ptr != NULL ) {
        s->top_ptr = s->top_ptr->next_ptr;
    }
}

void* S_Top (stack_t *s) {

    if ( s->top_ptr != NULL )
        return s->top_ptr->data;
    else
        return NULL;
}

int S_Empty (stack_t *s) {

    return ( s->top_ptr != NULL ? 0 : -1 );
}

void S_Copy (stack_t *dst_stack, stack_t *src_stack) {

    while ( !S_Empty(src_stack) ) {
        S_Push(dst_stack, S_Top(src_stack));
        S_Pop(src_stack);    
    }
}

void S_Print (stack_t *s) {
    (void)s;
#ifdef DEBUG
    elem_t *tmp = s->top_ptr;
    while ( s->top_ptr != NULL ) {
        printf ("%d\n", *((int*)s->top_ptr->data));
        s->top_ptr = s->top_ptr->next_ptr;
    }
    s->top_ptr = tmp;
#endif
}