#ifndef STACK_H
#define STACK_H

typedef struct S_Elem {
    void **data;
    struct S_Elem *next_ptr;
} S_Elem;

typedef struct T_Stack {
    S_Elem *top_ptr;
} T_Stack;

void S_Init (T_Stack *s);
int S_Push (T_Stack *s, void* data);
void S_Pop (T_Stack *s);
void* S_Top (T_Stack *s);
int S_Empty (T_Stack *s);
void S_Print (T_Stack *s);
void S_Destroy (T_Stack *s);
void S_Copy (T_Stack *dst_stack, T_Stack *src_stack);

#endif // STACK_H