///////////////////////////////////////////////////////////////////////////////////
// School:      Brno University of Technology, Faculty of Information Technology //
// Course:      Formal Languages and Compilers                                   //
// Project:     IFJ17                                                            //
// Module:      Predence analysis                                                //
// Authors:     Kristián Liščinský  (xlisci01)                                   //
//              Matúš Liščinský     (xlisci02)                                   //
//              Šimon Stupinský     (xstupi00)                                   //
//              Vladimír Marcin     (xmarci10)                                   //
///////////////////////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "scanner.h"
#include "error.h"
#include "expr.h"
#include "stack.h"
#include "symtable.h"
#include "semantic_control.h"
#include "generate.h"
#include "strlib.h"
#include "clear.h"

#define malloc(size) _malloc(size)

/**
 * @brief      Determines if arithmetic operator.
 *
 * @param[in]  token_type  The token type
 *
 * @return     True if arithmetic operator, False otherwise.
 */
bool is_arithmetic_opr (int token_type);

/**
 * @brief      Determines if logic operator.
 *
 * @param[in]  token_type  The token type
 *
 * @return     True if logic operator, False otherwise.
 */
bool is_logic_opr (int token_type);

/**
 * @brief      Determines if operand.
 *
 * @param[in]  token_type  The token type
 *
 * @return     True if operand, False otherwise.
 */
bool is_operand (int token_type);

/**
 * @brief      Determines if builtin function.
 *
 * @param[in]  token_type  The token type
 *
 * @return     True if builtin function, False otherwise.
 */
bool is_builtin_function (int token_type);

/**
 * @brief      Determines if users function.
 *
 * @param      act_token     The act token
 * @param      act_function  The act function
 *
 * @return     True if users function, False otherwise.
 */
bool is_users_function (token_t *act_token, function_t *act_function);

/**
 * @brief      Determines if number.
 *
 * @param[in]  type  The type
 *
 * @return     True if number, False otherwise.
 */
bool is_number (int type);

/**
 * @brief      { function_description }
 *
 * @param      entry_token  The entry token
 * @param      stack_token  The stack token
 *
 * @return     { description_of_the_return_value }
 */
bool shift_to_stack (token_t *entry_token, token_t *stack_token);

/**
 * @brief      { function_description }
 *
 * @param[in]  type_token    The type token
 * @param[in]  count_params  The count parameters
 */
void control_token (int type_token, int count_params);

/**
 * @brief      { function_description }
 *
 * @param      act_token   The act token
 * @param      prev_token  The previous token
 * @param      set_logic   The set logic
 */
void correct_expr (token_t *act_token, int *prev_token, bool *set_logic);

/**
 * @brief      { function_description }
 *
 * @param      operators_stack  The operators stack
 * @param      output_stack     The output stack
 */
void do_until_left_bracket (stack_t *operators_stack, stack_t *output_stack);

/**
 * @brief      { function_description }
 *
 * @param      operators_stack  The operators stack
 * @param      output_stack     The output stack
 * @param      act_token        The act token
 * @param      stack_token      The stack token
 */
void do_operation (stack_t *operators_stack, stack_t *output_stack, token_t *act_token, token_t *stack_token);

/**
 * @brief      { function_description }
 *
 * @param      function      The function
 * @param      act_function  The act function
 * @param      l_value       The l value
 */
void builtin_function (token_t *function, function_t *act_function, variable_t *l_value);

/**
 * @brief      { function_description }
 *
 * @param      act_function  The act function
 * @param      l_value       The l value
 */
void infix_to_postfix (function_t *act_function, variable_t *l_value);

/**
 * @brief      { function_description }
 *
 * @param      postfix_stack  The postfix stack
 * @param      act_function   The act function
 * @param      l_value        The l value
 */
void control_postfix (stack_t *postfix_stack, function_t *act_function, variable_t *l_value);

/**
 * @brief      { function_description }
 *
 * @param[in]  <unnamed>  { parameter_description }
 * @param      operand_1  The operand 1
 * @param      operand_2  The operand 2
 *
 * @return     { description_of_the_return_value }
 */
int check_return_type(int operator, variable_t *operand_1, variable_t *operand_2);

/**
 * @brief      { function_description }
 *
 * @param      act_token     The act token
 * @param      act_function  The act function
 * @param      l_value       The l value
 *
 * @return     { description_of_the_return_value }
 */
int users_function (token_t *act_token, function_t *act_function, variable_t *l_value);

/**
 * @brief      { function_description }
 *
 * @param[in]  expect_type  The expect type
 * @param[in]  real_type    The real type
 *
 * @return     { description_of_the_return_value }
 */
int types_control (int expect_type, int real_type);

/**
 * @brief      { function_description }
 *
 * @param[in]  type  The type
 *
 * @return     { description_of_the_return_value }
 */
int decode_type (char type);

/**
 * @brief      { function_description }
 *
 * @param      find_token    The find token
 * @param      act_function  The act function
 *
 * @return     { description_of_the_return_value }
 */
variable_t *find_var (token_t *find_token, function_t *act_function);

/**
 * @brief      Stores a constant.
 *
 * @param      const_token  The constant token
 *
 * @return     { description_of_the_return_value }
 */
variable_t *store_constant (token_t *const_token);

/**
 * @brief      { function_description }
 *
 * @param      act_function  The act function
 * @param[in]  expect_type   The expect type
 *
 * @return     { description_of_the_return_value }
 */
variable_t *next_params (function_t *act_function, int expect_type);

/**
 * @brief      { function_description }
 *
 * @param      act_token  The act token
 *
 * @return     { description_of_the_return_value }
 */
token_t *copy_token (token_t *act_token);

char precedence_table [SIZE_TABLE][SIZE_TABLE] = {
/*            +    -    *    /    (    )    \    <    >   <=   >=    =   <>   id   lit   $ */
/*  +  */   {'>', '>', '<', '<', '<', '>', '<', '>', '>', '>', '>', '>', '>', '<', '<', '>'},
/*  -  */   {'>', '>', '<', '<', '<', '>', '<', '>', '>', '>', '>', '>', '>', '<', '<', '>'},
/*  *  */   {'>', '>', '>', '>', '<', '>', '>', '>', '>', '>', '>', '>', '>', '<', '<', '>'},
/*  /  */   {'>', '>', '>', '>', '<', '>', '>', '>', '>', '>', '>', '>', '>', '<', '<', '>'}, 
/*  (  */   {'<', '<', '<', '<', '<', '=', '<', '<', '<', '<', '<', '<', '<', '<', '<', ' '},
/*  )  */   {'>', '>', '>', '>', ' ', '>', '>', '>', '>', '>', '>', '>', '>', ' ', ' ', '>'},
/*  \  */   {'>', '>', '<', '<', '<', '>', '>', '>', '>', '>', '>', '>', '>', '<', '<', '>'},
/*  <  */   {'<', '<', '<', '<', '<', '>', '<', ' ', ' ', ' ', ' ', ' ', ' ', '<', '<', '>'},    
/*  >  */   {'<', '<', '<', '<', '<', '>', '<', ' ', ' ', ' ', ' ', ' ', ' ', '<', '<', '>'},
/*  <= */   {'<', '<', '<', '<', '<', '>', '<', ' ', ' ', ' ', ' ', ' ', ' ', '<', '<', '>'},
/*  >= */   {'<', '<', '<', '<', '<', '>', '<', ' ', ' ', ' ', ' ', ' ', ' ', '<', '<', '>'},
/*  =  */   {'<', '<', '<', '<', '<', '>', '<', ' ', ' ', ' ', ' ', ' ', ' ', '<', '<', '>'},
/*  <> */   {'<', '<', '<', '<', '<', '>', '<', ' ', ' ', ' ', ' ', ' ', ' ', '<', '<', '>'},
/*  ID */   {'>', '>', '>', '>', ' ', '>', '>', '>', '>', '>', '>', '>', '>', ' ', ' ', '>'},
/* LIT */   {'>', '>', '>', '>', ' ', '>', '>', '>', '>', '>', '>', '>', '>', ' ', ' ', '>'},
/*  $  */   {'<', '<', '<', '<', '<', ' ', '<', '<', '<', '<', '<', '<', '<', '<', '<', ' '},
};

bool is_arithmetic_opr (int token_type) {

    if ( token_type >= ADD && token_type <= INT_DIV )
        return true;
    else    
        return false;
}

bool is_logic_opr (int token_type) {

    if ( token_type >= LESS && token_type <= NEQ )
        return true;
    else    
        return false;
}

bool is_operand (int token_type) {

    if ( token_type >= ID && token_type <= TEXT )
        return true;
    else   
        return false;
}

bool is_builtin_function (int token_type) {

    if ( token_type >= LENGTH && token_type <= CHR )
        return true;
    else {
        return false;
    }
}

bool is_users_function (token_t *act_token, function_t *act_function) {

    htab_item_t *search_function = htab_find(global_symtable, act_token->str->string);

    if ( !search_function ) {
        find_var(act_token, act_function);
        ungetToken();
        return false;
    }

    return true;
}

bool is_number (int type) {

    if ( type == INT_NUMBER || type == INTEGER || type == DOUBLE || type == DOUBLE_NUMBER )
        return true;
    else
        return false;
}

token_t* copy_token(token_t *act_token) {

    token_t *new_token;
    if ( !(new_token = malloc(sizeof(token_t))) )
        print_err(99);

    new_token->str = strInit(strlen(act_token->str->string)) ;

    new_token->type = act_token->type;
    if ( act_token->str->string )
        strcpy(new_token->str->string, act_token->str->string);
    new_token->str->length = act_token->str->length;

    return new_token;
}

bool shift_to_stack (token_t *entry_token, token_t *stack_token) {

    unsigned index_y = entry_token->type - ADD;
    unsigned index_x = stack_token->type - ADD;

    if ( stack_token->type == LEFT_R_BRACKET ) 
        index_x = 4;
    else if ( stack_token->type == RIGHT_R_BRACKET )
        index_x = 5;

    if ( precedence_table[index_x][index_y] == '<')
        return true;
    else 
        return false;
}

void correct_expr (token_t *act_token, int *prev_token, bool *set_logic) {

    if ( is_arithmetic_opr(*prev_token) || *prev_token == LEFT_R_BRACKET || *prev_token == FIRST_TOKEN || is_logic_opr(*prev_token) ) {
        if ( !is_operand(act_token->type) && act_token->type != LEFT_R_BRACKET )
            print_err(2);
    }
    else if ( is_operand(*prev_token) || *prev_token == RIGHT_R_BRACKET ) {
        if ( is_logic_opr(act_token->type) ) {
            if ( *set_logic )
                print_err(2);
        }
        else if ( !is_arithmetic_opr(act_token->type) && act_token->type != RIGHT_R_BRACKET ) {
            print_err(2);
        }
    }

    *prev_token = act_token->type;
    if ( is_logic_opr(*prev_token) )
        *set_logic = true;
}

void do_until_left_bracket (stack_t *operators_stack, stack_t *output_stack) {

    while ( !S_Empty(operators_stack) ) {
        token_t * stack_token = copy_token(S_Top(operators_stack));
        if ( stack_token->type != LEFT_R_BRACKET ) {
            S_Push(output_stack, stack_token);
            S_Pop(operators_stack);    
        }
        else {
            S_Pop(operators_stack);
            break;
        }
    }
}

void do_operation (stack_t *operators_stack, stack_t *output_stack, token_t *act_token, token_t *stack_token) {

    if ( S_Empty(operators_stack) || shift_to_stack(act_token, stack_token) || stack_token->type == LEFT_R_BRACKET ) {
        stack_token = copy_token(act_token);
        S_Push(operators_stack, stack_token);
    }
    else {
        while ( !S_Empty(operators_stack) && !shift_to_stack(act_token, stack_token) ) {
            stack_token = copy_token(stack_token);
            S_Push(output_stack, stack_token);
            S_Pop(operators_stack);
            stack_token = S_Top(operators_stack);
            }
        stack_token = copy_token(act_token);
        S_Push(operators_stack, stack_token);
    }
}

int check_return_type(int operator, variable_t *operand_1, variable_t *operand_2) {
    
    if ( !is_number(operand_1->data_type) && !is_number(operand_2->data_type) && (operator == ADD || is_logic_opr(operator)) )
        return STRING;
    else if ( (is_number(operand_1->data_type) || is_number(operand_2->data_type)) && types_control(operand_1->data_type, operand_2->data_type) ) {
        if ( operator == INT_DIV )
            return INTEGER;
        else if ( ((operand_1->data_type != INT_NUMBER && operand_1->data_type != INTEGER) || 
             (operand_2->data_type != INT_NUMBER && operand_2->data_type != INTEGER)) ||
             operator == DIV )
             return DOUBLE;
        return INTEGER;
    }
    else
        print_err(4);

    return 0;
}

void control_postfix (stack_t *postfix_stack, function_t *act_function, variable_t *l_value) {

    stack_t *output_stack = S_Init();

    variable_t *new_var = init_variable();   
    int ret_type;

    while ( !S_Empty(postfix_stack) ) {
        token_t * act_token = S_Top(postfix_stack);
        S_Pop(postfix_stack);
        if ( is_operand(act_token->type) ) {
            variable_t * operand = find_var(act_token, act_function);
            S_Push(output_stack, operand);
            if ( !is_number(operand->data_type) && operand->data.str == NULL ) {
                operand->data.str = (char *)malloc(1);
                strcpy(operand->data.str,"");
            }
            list_insert("PUSHS ", operand, NULL, NULL);
            ret_type = operand->data_type;
        }
        else {
            variable_t * operand_1 = S_Top(output_stack);
            S_Pop(output_stack);
            variable_t *operand_2 = S_Top(output_stack);
            S_Pop(output_stack);
            ret_type = check_return_type(act_token->type, operand_1, operand_2);
            if ( operand_1->data_type != ret_type )
                retype(operand_1);
            if ( operand_2->data_type != ret_type ) {
                variable_t * tmp = create_var("PRINT ", false);
                list_insert("POPS ", tmp, NULL, NULL);
                retype(operand_2);
                list_insert("PUSHS ", tmp, NULL, NULL);
            } 
            if ( is_logic_opr(act_token->type) )  
                ret_type = INTEGER;

            variable_t * tmp = create_var("PRINT ", false);
            switch(act_token->type) {
                case ADD:  
                            if ( operand_1->data_type == STRING && operand_2->data_type == STRING ) {
                                concat(); 
                                break;
                            }
                            list_insert("ADDS ", NULL, NULL, NULL); break;
                case SUB: 
                            list_insert("SUBS ", NULL, NULL, NULL); break;
                case MUL: 
                            list_insert("MULS ", NULL, NULL, NULL); break;
                case DIV: 
                            list_insert("DIVS ", NULL, NULL, NULL); break;
                case INT_DIV:
                            list_insert("POPS ", tmp, NULL, NULL);
                            list_insert("INT2FLOATS ", NULL, NULL, NULL);
                            list_insert("PUSHS ", tmp, NULL, NULL);
                            list_insert("INT2FLOATS ", NULL, NULL, NULL);
                            list_insert("DIVS ", NULL, NULL, NULL);
                            list_insert("FLOAT2INTS ", NULL, NULL, NULL); break; 
                case LESS: 
                            list_insert("LTS ", NULL, NULL, NULL); break;
                case GREATER: 
                            list_insert("GTS ", NULL, NULL, NULL); break;
                case ASSIGNMENT_EQ: 
                            list_insert("EQS ", NULL, NULL, NULL); break;
                case LESS_EQ: 
                            list_insert("GTS ", NULL, NULL, NULL);
                            list_insert("NOTS ", NULL, NULL, NULL); break;
                case GREATER_EQ: 
                            list_insert("LTS ", NULL, NULL, NULL);
                            list_insert("NOTS ", NULL, NULL, NULL); break;
                case NEQ:
                            list_insert("EQS ", NULL, NULL, NULL);
                            list_insert("NOTS ", NULL, NULL, NULL); break;
            }
            variable_t *stack_var = init_variable();
            stack_var->data_type = ret_type;
            S_Push(output_stack, stack_var);
            new_var = stack_var;
        }
    }
    if ( new_var->data_type == 0 )
        new_var->data_type = ret_type;

    if ( l_value ) {
        types_control(ret_type, l_value->data_type); 
        if ( ret_type != l_value->data_type )
            retype(new_var);
    }
    //S_Destroy(output_stack);
}

void infix_to_postfix (function_t *act_function, variable_t *l_value) {

    token_t *act_token;
    token_t *stack_token;
    unsigned count_of_bracket = 0;
    int sum_count = 1;
    bool logic_on = false;
 
    stack_t *output_stack = S_Init();

    stack_t *infix_stack = S_Init();

    act_token = getToken();
    int prev_token = FIRST_TOKEN;

    while ( act_token->type != SEMICOLON && act_token->type != EOL && 
            act_token->type != THEN && act_token->type != COMMA
            ) {

        correct_expr(act_token, &prev_token, &logic_on);
        if ( is_operand(act_token->type) ) {
            sum_count--;
            stack_token = copy_token(act_token);
            S_Push(output_stack, stack_token);
        }
        else if ( act_token->type == LEFT_R_BRACKET ) {
            count_of_bracket++;
            stack_token = copy_token(act_token);
            S_Push(infix_stack, stack_token);
        }
        else if ( act_token->type == RIGHT_R_BRACKET ) {
            count_of_bracket--;
            do_until_left_bracket(infix_stack, output_stack);
        }
        else if ( is_arithmetic_opr(act_token->type) || is_logic_opr(act_token->type) ) {
            sum_count++;
            stack_token = S_Top(infix_stack);
            do_operation(infix_stack, output_stack, act_token, stack_token);
        }
        act_token = getToken();
    }

    while ( !S_Empty(infix_stack) ) {
		S_Push(output_stack, copy_token(S_Top(infix_stack)));
		S_Pop(infix_stack);
    }

    S_Copy(infix_stack, output_stack);
    //S_Destroy(output_stack);

    if ( count_of_bracket || sum_count )
        print_err(2);
    if ( l_value->data_type == BOOLEAN && !logic_on ) 
        print_err(4);
    if ( l_value->data_type != BOOLEAN && logic_on )
        print_err(6);
    ungetToken();

    if ( l_value->data_type == BOOLEAN || l_value->data_type == 0)
        l_value = NULL;   

    control_postfix(infix_stack, act_function, l_value);
    //S_Destroy(infix_stack);
}

void control_token (int type_token, int count_params) {

    token_t *act_token = getToken();

    if ( type_token == LEFT_R_BRACKET && act_token->type != LEFT_R_BRACKET)
        print_err(2);
    if ( (type_token != RIGHT_R_BRACKET && act_token->type == RIGHT_R_BRACKET) ||
         (type_token != COMMA && act_token->type == COMMA) ) {
            token_t *token = getToken();
            if ( (token->type == RIGHT_R_BRACKET || token->type == EOL) && !count_params )
                print_err(2);
            ungetToken(); 
        print_err(4);
    }
    if ( (type_token == RIGHT_R_BRACKET && is_operand(act_token->type)) ||
         type_token != act_token->type )
        print_err(2);
}

variable_t *store_constant (token_t *const_token) {

    htab_item_t *is_find = htab_find(const_symtable, const_token->str->string);

    if ( !is_find ) {
        variable_t *new_constant = init_variable();
        if ( const_token->type == INT_NUMBER ) {
            new_constant->data_type = INTEGER;
            new_constant->data.i = strtol(const_token->str->string, NULL, 10);
        }
        else if ( const_token->type == DOUBLE_NUMBER ) {
            new_constant->data_type = DOUBLE;
            new_constant->data.d = strtod(const_token->str->string, NULL);
        }
        else if ( const_token->type == TEXT ) {
            new_constant->data_type = STRING;
            new_constant->data.str = malloc((const_token->str->length)+1);
            if(!new_constant->data.str)
                print_err(99);
            strcpy(new_constant->data.str, const_token->str->string);
        }
        new_constant->constant = true;
        is_find = htab_insert(const_symtable, const_token->str->string); 
        if( !is_find )
            print_err(99);
        is_find->is_function = 0;
        is_find->data.var = new_constant;
    }

    return is_find->data.var;
}

variable_t *find_var (token_t *find_token, function_t *act_function) {

    if ( find_token->type == ID ) {
        htab_item_t *is_found = htab_find(act_function->local_symtable, find_token->str->string);
        if ( !is_found )
            print_err(3);
        return is_found->data.var;
    }
    else if ( find_token->type >= INT_NUMBER && find_token->type <= TEXT )
        return store_constant(find_token); 

    return NULL;
}


int types_control (int expect_type, int real_type) {

    if ( (is_number(expect_type) && !is_number(real_type)) || (is_number(real_type) && !is_number(expect_type) ) ) 
        print_err(4);

    return 1;
}

variable_t *next_params (function_t *act_function, int expect_type) {

    token_t *act_token = getToken();

    if ( !is_operand(act_token->type) ) 
        print_err(2);
    variable_t *act_param = find_var(act_token, act_function);
    types_control(expect_type, act_param->data_type);

    return act_param;
}

void builtin_function (token_t *function, function_t *act_function, variable_t *l_value) {

    int function_name = function->type;

    control_token(LEFT_R_BRACKET, 0);

    token_t *act_token = getToken();
    if (act_token->type == RIGHT_R_BRACKET)
        print_err(4);
    else 
        ungetToken();

    if ( function_name == LENGTH ) {
        variable_t *param = next_params(act_function, STRING);
        control_token(RIGHT_R_BRACKET, 0);
        if (l_value)
            types_control(INTEGER, l_value->data_type);
        list_insert("PUSHS ", param, NULL, NULL);
        length_of_str(l_value);
    }
    else if ( function_name == SUBSTR ) {
        variable_t *param_1 = next_params(act_function, STRING);
        control_token(COMMA, 2);
        variable_t *param_2 = next_params(act_function, INTEGER);
        control_token(COMMA, 1);
        variable_t *param_3 = next_params(act_function, INTEGER);
        control_token(RIGHT_R_BRACKET, 0);
        if (l_value)
            types_control(STRING, l_value->data_type);
        list_insert("PUSHS ", param_1, NULL, NULL);
        list_insert("PUSHS ", param_2, NULL, NULL);
        if(param_2->data_type == DOUBLE)
            list_insert("FLOAT2R2EINTS ", NULL, NULL, NULL);
        list_insert("PUSHS ", param_3, NULL, NULL);
        if(param_3->data_type == DOUBLE)
            list_insert("FLOAT2R2EINTS ", NULL, NULL, NULL);
        substr();
    }
    else if ( function_name == ASC ) {
        variable_t *param_1  = next_params(act_function, STRING);
        control_token(COMMA, 1);
        variable_t *param_2 = next_params(act_function, INTEGER);
        control_token(RIGHT_R_BRACKET, 0);
        if (l_value)
            types_control(INTEGER, l_value->data_type);
        list_insert("PUSHS ", param_1, NULL, NULL);
        list_insert("PUSHS ", param_2, NULL, NULL);
        if(param_2->data_type == DOUBLE)
            list_insert("FLOAT2R2EINTS ", NULL, NULL, NULL);
        asc(l_value);

    }
    else {
        variable_t *param = next_params(act_function, INTEGER);
        control_token(RIGHT_R_BRACKET,0);
        if (l_value)
            types_control(STRING, l_value->data_type);
        list_insert("PUSHS ", param, NULL, NULL);
        if(param->data_type == DOUBLE)
            list_insert("FLOAT2R2EINTS ", NULL, NULL, NULL);
        chr();
    }
}

int decode_type (char type) {

    if ( type == 'i' )
        return INTEGER;
    else if ( type == 'd' )
        return DOUBLE;
    else 
        return STRING;
}

int users_function (token_t *act_token, function_t *act_function, variable_t *l_value) {

    htab_item_t *search_function = htab_find(global_symtable, act_token->str->string);
    function_t *users_function;
    
    users_function = search_function->data.fun;

    control_token(LEFT_R_BRACKET, 0);
    int count_of_params = users_function->params->length;

    if ( count_of_params ) {
        act_token = getToken();
        if (act_token->type == RIGHT_R_BRACKET)
            print_err(4);
        else 
            ungetToken();
    }

    for( int i = 0; i < count_of_params; i++ ) {
        int type_param = decode_type(users_function->params->string[i]);
        variable_t * param = next_params(act_function, type_param);
        if ( i != count_of_params-1 )
            control_token(COMMA, count_of_params-i);
        list_insert("PUSHS ", param, NULL, NULL);
        if ( param->data_type != type_param )
            retype(param);
    }
    control_token(RIGHT_R_BRACKET, 0);
    if ( l_value )
        types_control(search_function->data.fun->return_type, l_value->data_type);

    variable_t * tmp = create_var(search_function->key, true);
    list_insert("CALL ", tmp, NULL, NULL);

    return users_function->return_type;
}

void expression (function_t *act_function, variable_t *l_value) {

    token_t *act_token = getToken();

    if ( is_builtin_function(act_token->type) ) 
        builtin_function(act_token, act_function, l_value);
    else if ( is_users_function(act_token, act_function) ) {
        int fun_type = users_function(act_token, act_function, l_value);
        if(l_value && fun_type != l_value->data_type ) {
            if(fun_type == INTEGER)
                list_insert("INT2FLOATS ",NULL, NULL, NULL);
            else if(fun_type == DOUBLE)
                list_insert("FLOAT2INTS ",NULL, NULL, NULL);
        }
    }
    else
        infix_to_postfix(act_function, l_value);
}