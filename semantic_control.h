#ifndef SEM_CONTROL
#define SEM_CONTROL

#include <stdbool.h>
#include "symtable.h"
#include "strlib.h"
#include "scanner.h"
#include "stack.h"

/// the size of array which contains types of function's parameters
#define PARAM_COUNT 10

extern htab_t *global_symtable;         ///< global symbol table which contains program's functions
extern htab_t *const_symtable;          ///< global table which contains constants
extern string_t *current_function_name; ///< name of function that is processed
extern string_t *current_variable_name; ///< name of variable that is processed
extern stack_t *label_stack;            ///< stack for generate labels

/**
 * @brief      this structure represents program's variable
 */
typedef struct variable_t{
    int data_type;      ///< data type of variable (4-double, 10-integer, 19-string)
    union{      
        int i;         
        double d;       
        char *str;      
    }data;              ///< in case that variable is constant we store its data
    bool constant;      ///< flag indicates that variable is constant
}variable_t;

/**
 * @brief      this structure represents program's function
 */
typedef struct function_t{
    int return_type;                ///< return type of function (4-double, 10-integer, 19-string)
    bool defined;                   ///< flag indicates that functions is defined 
    struct htab_t *local_symtable;  ///< function's local symbol table which contains it's variables
    string_t *params;               ///< array which contains types and number of function's parameters
    variable_t *return_var;         ///< return variable of function
}function_t;

/**
 * @brief      the function creates and initialize all global structures
 */
void init_globals();

/**
 * @brief      the function creates new program's variable and init it
 * @return     the function returns pointer to newly created variable
 */
variable_t *init_variable();

/**
 * @brief      the function creates new program's function and init it
 * @return     the function returns pointer to newly created function
 */
function_t *init_function();

/**
 * @brief      the function stores function in global symbol table.
 *             The function also check if function 'f' is not a redefinition
 *             or redeclaration and in case that 'f' is a definition and 
 *             declaration is in the symtable checks return type, types of
 *             parameters and number of parameters.
 *
 * @param      f       function which we want to add to symbol table
 * @param[in]  f_name  name of function 'f'
 */
void store_fun_in_symtable(function_t *f,const char *f_name);

/**
 * @brief      the function stores variable in local symtable of function 'f'.
 *             The function also check if doesn't exist variable with same name in
 *             local symbol table of function 'f' and also in global symbol table. 
 *
 * @param      f       function in which variable 'v' is defined
 * @param      v       variable wich we want to store in local symtable of function 'f'
 * @param[in]  v_name  name of variable 'v'
 */
void store_var_in_symtable(function_t *f,variable_t *v,const char *v_name);

/**
 * @brief      the function checks if all of declared function have a definition.
 *             If some of declared function doesn't have a definition it is a semantic error.
 */
void check_function_definitions();

/**
 * @brief      the function searches variable with name 'key' in table 'symtable'
 *
 * @param      symtable  table in which function searches variable
 * @param[in]  key       name of searched variable
 *
 * @return     function returns pointer to found variable or NULL pointer in case that 
 *             table doesn't contain variable with name 'key'
 */
variable_t *find_variable(struct htab_t* symtable, const char *key);

/**
 * @brief      the function codes an integer number to character which mean data type
 *             ('i'-integer, 'd'-double, 's'-string)
 *
 * @param[in]  type     integer that is coded to char
 *
 * @return     function returns character which means datatype or 'e' character which means error
 */
char code_param_type(int type);

/**
 * @brief      the function stores the name of function that is processed
 *
 * @param      token  token structure that contains a function name
 */
void store_current_variable_name(struct token_t *token);

/**
 * @brief      the function stores the name of variable that is processed
 *
 * @param      token  token structure that contains a variable name
 */
void store_current_function_name(struct token_t *token);

#endif