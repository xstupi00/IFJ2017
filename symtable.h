#ifndef SYMTABLE_H
#define SYMTABLE_H

#include <stdbool.h>

/**
 * size of hash table 
 * http://www.orcca.on.ca/~yxie/courses/cs2210b-2011/htmls/extra/PlanetMath_%20goodhashtable.pdf
 */
#define SIZE_HTABLE 97

/**
 * @brief      this structure represents item of hash table
 */
typedef struct htab_listitem{
    char *key;                  ///< the key
    bool is_function;           ///< flag indicates that item is function 
    union{                      
        struct variable_t *var; 
        struct function_t *fun; 
    }data;                      ///< data of item contain pointer to structure which represents variable or function
    struct htab_listitem *next; ///< pointer to the next item
}htab_item_t;

/**
 * @brief      this structure represents hash table
 */
typedef struct htab_t{
    unsigned arr_size;     ///< size of hash table
    unsigned n;            ///< the number of items in the hash table
    htab_item_t *ptr[];    ///< (flexible array member) the array of pointers to items
}htab_t;

/**
 * @brief      hash function for strings
 *             http://www.cse.yorku.ca/~oz/hash.html <- sdbm variant
 *
 * @param[in]  key   the key
 *
 * @return     the function returns index to table
 */
unsigned hash_function(const char *key);

/**
 * @brief      the function creates the hash table and initialize it
 *
 * @param[in]  size  size of new hash table
 *
 * @return     the function returns pointer to the newly created table or
 *             NULL pointer, when it fails
 */
htab_t* htab_init(unsigned size);

/**
 * @brief      the function creates new item with the 'key' and store it to the 'table'
 *
 * @param      table  table for storing new item
 * @param[in]  key    key for new item
 *
 * @return     the function returns pointer to the newly created item
 */
htab_item_t* htab_insert(htab_t *table, const char *key);

/**
 * @brief      the function finds the item with the 'key' and returns pointer to it
 *
 * @param      table  table for looking the item
 * @param[in]  key    key of the wanted item
 *
 * @return     the function returns pointer to the item in hash table or 
 *             NULL pointer, when the table doesn't contain searched item
 */
htab_item_t* htab_find(htab_t *table, const char *key);

/**
 * @brief      the function calls function 'func' over every item in the 'table'
 *
 * @param      table  table over which items will be function 'func' called
 * @param[in]  func   function which will be called
 */
void htab_foreach(htab_t *table,void(*func)(char*,bool,void*));

/**
 * @brief      the function prints the one item of hash table
 *
 * @param      key   key of the item
 * @param[in]  f     flag indicates that item is function
 * @param      data  pointer to data of the item
 */
void htab_print(char *key,bool f, void* data);

#endif