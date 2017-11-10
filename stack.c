#include "stack.h"
#include "error.h"

void S_Init (T_Stack *s) {

    s->top_ptr = NULL;
}

int S_Push (T_Stack *s, void* d) {

    S_Elem *new_element = NULL;
    if ( (new_element = (S_Elem *) malloc(sizeof(void *))) == NULL ) 
        print_err(99);

    new_element->data = d;
    new_element->next_ptr = s->top_ptr;
    s->top_ptr = new_element;

    return 0;
}

void S_Pop (T_Stack *s) {
    
    S_Elem *tmp_element;

    if ( s->top_ptr != NULL ) {
        tmp_element = s->top_ptr;
        s->top_ptr = s->top_ptr->next_ptr;
        free(tmp_element->data);
        free(tmp_element);
    }
}

void* S_Top (T_Stack *s) {

    if ( s->top_ptr != NULL )
        return s->top_ptr->data;
    else
        return NULL;
}

int S_Empty (T_Stack *s) {

    return ( s->top_ptr != NULL ? 0 : -1 );
}

// debug functions
void S_Print (T_Stack *s) {

    S_Elem *tmp = s->top_ptr;
    while ( s->top_ptr != NULL ) {
        printf ("%d\n", *((int*)s->top_ptr->data));
        s->top_ptr = s->top_ptr->next_ptr;
    }
    s->top_ptr = tmp;
}

int main() {
    
    T_Stack *s = (T_Stack *) malloc(sizeof(T_Stack));
    S_Init(s);

    for ( int i = 0; i < 20; i++) {
        int *x = malloc(sizeof(int));
        *x = i;
        S_Push(s, x);
    }

    S_Print(s);
    int j = 0;
    while ( !S_Empty(s) ) {
        S_Pop(s);
        printf("%d\n",j++);
    }

    S_Push(s,&j);
    int *f = (int *)S_Top(s);
    printf("%d\n",*f);

    return 0;
}