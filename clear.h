///////////////////////////////////////////////////////////////////////////////////
// School:      Brno University of Technology, Faculty of Information Technology //
// Course:      Formal Languages and Compilers                                   //
// Project:     IFJ17                                                            //
// Module:      Header file of clear module (garbage collector)                  //
// Authors:     Kristián Liščinský  (xlisci01)                                   //
//              Matúš Liščinský     (xlisci02)                                   //
//              Šimon Stupinský     (xstupi00)                                   //
//              Vladimír Marcin     (xmarci10)                                   //
///////////////////////////////////////////////////////////////////////////////////


#ifndef CLEAR_H
#define CLEAR_H

#include "stack.h"

extern stack_t * ptr_stack;	///< stack of all pointers pointing to alllocated memory on heap

/**
 * @brief      Functions initialize stack of pointers
 */
void init_ptr_stack();

/**
 * @brief      Own function for realloc
 *
 * @param      ptr       Pointer to memory to be reallocated
 * @param[in]  new_size  New size of memory to be allocated
 * @param[in]  old_size  Old size of allocated memory
 *
 * @return     Pointer to reallocated memory 
 */
void * _realloc(void *ptr, size_t new_size, size_t old_size);

/**
 * @brief      Own function for malloc
 *
 * @param[in]  size  Size of memory to be allocated
 *
 * @return     Pointer to allocated memory
 */
void * _malloc(size_t size);

/**
 * @brief      Own function for calloc
 *
 * @param[in]  num   Number of elements to allocate
 * @param[in]  size  Size of each element
 *
 * @return     Pointer to allocated memory
 */
void * _calloc(size_t num, size_t size);

/**
 * @brief      Deallocate all allocated memory blocks except memory for ptr_stack
 */
void clear_all();


#endif