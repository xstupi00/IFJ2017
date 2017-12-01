#ifndef STR_LIB_H
#define STR_LIB_H

#define STR_INIT 128

/**
 * @brief      { item_description }
 */
typedef struct string_t{
	char * string;		///<
	size_t capacity; 	///<
	size_t length;		///<
}string_t;

/**
 * @brief      { function_description }
 *
 * @param[in]  size  The size
 *
 * @return     { description_of_the_return_value }
 */
string_t * strInit(size_t size);

/**
 * @brief      { function_description }
 *
 * @param      str       The string
 * @param[in]  new_size  The new size
 *
 * @return     { description_of_the_return_value }
 */
bool extendStr(string_t * str, size_t new_size);

#endif