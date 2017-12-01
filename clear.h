#ifndef CLEAR_H
#define CLEAR_H

#include "stack.h"

extern stack_t * ptr_stack;	///<

/**
 * @brief      { function_description }
 */
void init_ptr_stack();

/**
 * @brief      { function_description }
 *
 * @param      ptr       The pointer
 * @param[in]  new_size  The new size
 * @param[in]  old_size  The old size
 *
 * @return     { description_of_the_return_value }
 */
void * _realloc(void *ptr, size_t new_size, size_t old_size);

/**
 * @brief      { function_description }
 *
 * @param[in]  size  The size
 *
 * @return     { description_of_the_return_value }
 */
void * _malloc(size_t size);

/**
 * @brief      { function_description }
 *
 * @param[in]  num   The number
 * @param[in]  size  The size
 *
 * @return     { description_of_the_return_value }
 */
void * _calloc(size_t num, size_t size);

/**
 * @brief      { function_description }
 */
void clear_all();


#endif