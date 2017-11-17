#ifndef STACK_H
#define STACK_H

typedef struct elem_t {
    void **data;
    struct elem_t *next_ptr;
} elem_t;

typedef struct stack_t {
    elem_t *top_ptr;
} stack_t;

void S_Init (stack_t *s);
int S_Push (stack_t *s, void* data);
void S_Pop (stack_t *s);
void* S_Top (stack_t *s);
int S_Empty (stack_t *s);
void S_Print (stack_t *s);
void S_Destroy (stack_t *s);
void S_Copy (stack_t *dst_stack, stack_t *src_stack);

#endif // STACK_H