///////////////////////////////////////////////////////////////////////////////////
// School:      Brno University of Technology, Faculty of Information Technology //
// Course:      Formal Languages and Compilers                                   //
// Project:     IFJ17                                                            //
// Module:      Heade file of stack                                              //
// Authors:     Kristián Liščinský  (xlisci01)                                   //
//              Matúš Liščinský     (xlisci02)                                   //
//              Šimon Stupinský     (xstupi00)                                   //
//              Vladimír Marcin     (xmarci10)                                   //
///////////////////////////////////////////////////////////////////////////////////

#ifndef STACK_H
#define STACK_H

/**
 * @brief      this structure represents item of stack
 */
typedef struct elem_t {
    void *data;					///< pointer to stack data
    struct elem_t *next_ptr;	///< pointer to next item
} elem_t;

/**
 * @brief      this structure represents stack
 */
typedef struct stack_t {
    elem_t *top_ptr;			///< pointer to the stack top
} stack_t;

/**
 * @brief      the function creates and initialize stack
 *
 * @return     function returns pointer to newly created stack
 */
stack_t* S_Init ();

/**
 * @brief      the function creates new stack item set data
 * 			   and insert it to the stack
 *
 * @param      s     the stack where we want to store new item
 * @param      data  the data to be stored
 */
void S_Push (stack_t *s, void* data);

/**
 * @brief      the function removes the item from top of the stack
 *
 * @param      s     the stack from which we want to remove the item
 */
void S_Pop (stack_t *s);

/**
 * @brief      the function returns item from top of the stack (don't remove it)
 *
 * @param      s     the stack from which we want to get the item
 *
 * @return     the function returns pointer to data of the item on top of the stack
 */
void* S_Top (stack_t *s);

/**
 * @brief      the function tells us if the stack is empty
 *
 * @param      s     pointer to stack
 *
 * @return     the function returns -1 if stack is empty and 0 if not
 */
int S_Empty (stack_t *s);

/**
 * @brief      the function prints the stack's items
 *
 * @param      s     pointer to stack
 */
void S_Print (stack_t *s);

/**
 * @brief      the function copies data from src_stack to dst_stack
 *
 * @param      dst_stack  the destination stack
 * @param      src_stack  the source stack
 */
void S_Copy (stack_t *dst_stack, stack_t *src_stack);

#endif // STACK_H