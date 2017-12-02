#ifndef STR_LIB_H
#define STR_LIB_H

#define STR_INIT 128

/**
 * @brief      this structure represents string with metadata  
 */
typedef struct string_t{
	char * string;		///< string allocated on heap
	size_t capacity; 	///< allocated space for string
	size_t length;		///< actual length of string
}string_t;

/**
 * @brief      Function initializes string structure 
 *
 * @param[in]  size  Size of usable space
 *
 * @return     Pointer to initialized string structure
 */
string_t * strInit(size_t size);

/**
 * @brief      Function to extend string 
 *
 * @param      str       Pointer to string structure 
 * @param[in]  new_size  New size of usable space
 *
 * @return     Pointer to string structure with extended string
 */
bool extendStr(string_t * str, size_t new_size);

#endif