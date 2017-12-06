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
 * @brief      Determines if token type is arithmetic operator.
 *
 * @param[in]  token_type  The token type
 *
 * @return     True if token type is arithmetic operator, False otherwise.
 */
bool is_arithmetic_opr (int token_type);

/**
 * @brief      Determines if token type is logic operator.
 *
 * @param[in]  token_type  The token type
 *
 * @return     True if token type is logic operator, False otherwise.
 */
bool is_logic_opr (int token_type);

/**
 * @brief      Determines if token type is operand.
 *
 * @param[in]  token_type  The token type
 *
 * @return     True if token type is operand, False otherwise.
 */
bool is_operand (int token_type);

/**
 * @brief      Determines if token type is name of builtin function.
 *
 * @param[in]  token_type  The token type
 *
 * @return     True if token type is name of builtin function, False otherwise.
 */
bool is_builtin_function (int token_type);

/**
 * @brief      Determines if token type is ID of users function.
 *
 * @param      act_token     The actual token
 * @param      act_function  The actual function, in which the compiler is located
 *
 * @return     True if token type is ID of users function, False otherwise.
 */
bool is_users_function (token_t *act_token, function_t *act_function);

/**
 * @brief      Determines if token type is number type (integer/double or number constant).
 *
 * @param[in]  type  The token type
 *
 * @return     True if token type is number, False otherwise.
 */
bool is_number (int type);

/**
 * @brief      According to the precedence table select relevant operation. Accorrding to intersection
 *             entry token and token on the top of stack in the precedence taable, to be able entry token 
 *             shift on the stack or reduced expression on the stack. This function is utilizing when 
 *             expression is transforming from infix to postfix.
 *
 * @param      entry_token  The actual token (coordinates y in the precedence table)
 * @param      stack_token  The token on the top stack (coordinates x in the precedence table)
 *
 * @return     True if selected operation is shift on the stack, False if selected operation is reducing expression. 
 */
bool shift_to_stack (token_t *entry_token, token_t *stack_token);

/**
 * @brief      Checking the permissible construction (syntax control) predominantly when calling the function.
 *             Too checks right count of params calling function. If the construction is not right or count of params
 *             does not match, error is reported (syntax or semantic).
 *
 * @param[in]  type_token    The type token for control
 * @param[in]  count_params  The count of params calling function
 */
void control_token (int type_token, int count_params);

/**
 * @brief      Checking the permissible construction (syntax control) predominantly when processing expressions.   
 *             Checks right sequence tokens (operators and operand) that are admitted according to the precedence table.
 *             If the construction is not right, syntax error in reported.
 *
 * @param      act_token   The actual token
 * @param      prev_token  The previous token
 * @param      set_logic   The set logic is a sign that mark using logic operator in actual expression.
 */
void correct_expr (token_t *act_token, int *prev_token, bool *set_logic);

/**
 * @brief      Empties the temporary stack (operators_stack) and adds element from him to the output stack with postfix
 *             expression. Empties terminates when on the top of stack will be left rounded bracket or stack will be empty.
 *             In the first case, will be left rounded bracket too adds to the output stack.
 *
 * @param      operators_stack  The temporary operators stack
 * @param      output_stack     The output stack with postfix expression
 */
void do_until_left_bracket (stack_t *operators_stack, stack_t *output_stack);

/**
 * @brief      According to element on the top of operators stack and actual token, respectively according to their
 *             intersection in precedence table performs appropriate one operation. 
 *             In the case that operators stack is empty, then actual token will be add to the operators stack. 
 *             As well as in the case that element on the top operators stack was left rounded bracket, or on the
 *             intersection in the table is located sign of shift to the stack.
 *             In the other case will be operators stack empties and relocation to the output stack. This emptying
 *             terminates when operators stack will be empty or on the intersetion in the table will not be sign of
 *             reducing.
 *
 * @param      operators_stack  The temporary operators stack
 * @param      output_stack     The output stack with postfix expression
 * @param      act_token        The actual token
 * @param      stack_token      The stack token on the top of operators stack
 */
void do_operation (stack_t *operators_stack, stack_t *output_stack, token_t *act_token, token_t *stack_token);

/**
 * @brief      Secures syntax and semantic control when builtin function is calling. Some instruction 
 *             for target code are creating in this function, and other are creating in function, which are calling
 *             from this function.
 *
 * @param      function      The actual token, which contains name of builtin function in the begin.
 * @param      act_function  The actual function in which the compiler is located
 * @param      l_value       The left value in assignment, in which expression will be assigned to
 */
void builtin_function (token_t *function, function_t *act_function, variable_t *l_value);

/**
 * @brief      Secures selection appropriate one activity according the actual obtaining token. 
 *             Kind of activity is selected on the basis of resolution of the species
 *             operands, operators and left or right rounded bracket. The result of
 *             sequence of these operations is eventual postfix expression.
 *             In conclusion underway syntax and semantic control some aspects acceptable expressions.
 *
 * @param      act_function  The actual function in which the compiler is located
 * @param      l_value       The left value in assignment, in which function is calling
 */
void infix_to_postfix (function_t *act_function, variable_t *l_value);

/**
 * @brief      Empties postfix stack, which contains postfix expression and terminates when stack is empty.
 *             In the case that from the top of stack was obtaining operand, is operand add to temporary stack and waiting to 
 *             arrival competent operators.
 *             In the case that from the top of stack was obtaining operator, from the temporary stack are gained two operands
 *             and according to type of operators is generating appropriate one instruction.
 *             During the empties postfix stack are performed semantic controls. 
 *
 * @param      postfix_stack  The postfix stack with postfix expression
 * @param      act_function   The actual function in which the compiler is located
 * @param      l_value        The left value in assignment, in which expression will be assigned to
 */
void postfix_to_instr (stack_t *postfix_stack, function_t *act_function, variable_t *l_value);

/**
 * @brief      Check returns type from competent operation with operator and two operands.
 *             In the case unauthorized combination of one of the operands with operator, or operands between themselves
 *             is semantic error in reported.
 * 
 * @param[in]  operator   The operator
 * @param      operand_1  The operand 1
 * @param      operand_2  The operand 2
 *
 * @return     Returns evaluated data type (integer/double/string) or terminates with error.
 */
int check_return_type(int operator, variable_t *operand_1, variable_t *operand_2);

/**
 * @brief      Function search calling function in global symtable and secures syntax and semantic 
 *             control. Some instruction for target code are creating in this function, and other 
 *             are creating in other functions in modul parser.
 *
 * @param      act_token     The actual token which contains ID of users function in the begin.
 * @param      act_function  The actual function in which the compiler is located
 * @param      l_value       The left value in assignment, in which function is calling
 *
 * @return     Returns return type of calling function which will be assigned to left side.
 */
int users_function (token_t *act_token, function_t *act_function, variable_t *l_value);

/**
 * @brief      Check admissible types also in the case of allowed conversion.
 *             At incompatibility both types is semantic error in reported.
 *
 * @param[in]  expect_type  The expect data type
 * @param[in]  real_type    The real data type
 *
 * @return     Return 1, in case that types is allowed or terminates with error.
 */
int types_control (int expect_type, int real_type);

/**
 * @brief      Decoding data type on the basis of agreed regulations encryption
 *
 * @param[in]  type  The coding data type
 *
 * @return     Return encoding data type (integer/double/string)
 */
int decode_type (char type);

/**
 * @brief      In first case, that token type is ID, secures searching ID in the local 
 *             symtable of actual function. If ID not founded is semantic error in reported.
 *             Second case, that token type is constant (int/double/str), secures calling other function
 *             to store this constant to local symtable actual function.
 *
 * @param      find_token    The actual token
 * @param      act_function  The actual function in which the compiler is located
 *
 * @return     Returns pointer on variable to the local symtable
 */
variable_t *find_var (token_t *find_token, function_t *act_function);

/**
 * @brief      Stores a constant to the local symtable of actual function with relevant parameters.
 *
 * @param      const_token  The token for store to the symtable
 *
 * @return     Returns pointer on new created variable in the local symbtable
 */
variable_t *store_constant (token_t *const_token);

/**
 * @brief      Secures searching operand in local symtable of actual function calling other function
 *             and calling function, which control admissible data types.
 *
 * @param      act_function  The actual function in which the compiler is located
 * @param[in]  expect_type   The expect type searched variable
 *
 * @return     Returns pointer on searching variable to the local symtable
 */
variable_t *next_params (function_t *act_function, int expect_type);

/**
 * @brief      Secures creating new pointers to actual token. It's needed at add pointers on the stack.
 *
 * @param      act_token  The actual token
 *
 * @return     Returns pointer on the new created pointer with old content  
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

    /// searching in local symtable of actual fuction
    htab_item_t *search_function = htab_find(global_symtable, act_token->str->string);

    /// if we have not found, let's verify that ID of some variable
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

int types_control (int expect_type, int real_type) {

    if ( (is_number(expect_type) && !is_number(real_type)) || (is_number(real_type) && !is_number(expect_type) ) ) 
        print_err(4);

    return 1;
}

int decode_type (char type) {

    /// decoding under predefined rules
    if ( type == 'i' )
        return INTEGER;
    else if ( type == 'd' )
        return DOUBLE;
    else 
        return STRING;
}

token_t* copy_token(token_t *act_token) {

    token_t *new_token;
    if ( !(new_token = malloc(sizeof(token_t))) )
        print_err(99);

    /// initialization token string about old size of string
    new_token->str = strInit(strlen(act_token->str->string)) ;

    /// copy type, string and his length to the atributes of new token
    new_token->type = act_token->type;
    if ( act_token->str->string )
        strcpy(new_token->str->string, act_token->str->string);
    new_token->str->length = act_token->str->length;

    return new_token;
}

bool shift_to_stack (token_t *entry_token, token_t *stack_token) {

    /// calculate index to the precedence table
    unsigned index_y = entry_token->type - ADD; // coordinates y
    unsigned index_x = stack_token->type - ADD; // coordinates x

    /// special recalculating for both types brackets
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

    /// case after arithmetic/logic operator, left bracket or yet only on the begin of the expression
    if ( is_arithmetic_opr(*prev_token) || *prev_token == LEFT_R_BRACKET || *prev_token == FIRST_TOKEN || is_logic_opr(*prev_token) ) {
        if ( !is_operand(act_token->type) && act_token->type != LEFT_R_BRACKET ) /// have to follow operand or left bracket, else error
            print_err(2);
    }
    /// case after operand or right bracket
    else if ( is_operand(*prev_token) || *prev_token == RIGHT_R_BRACKET ) {
        if ( is_logic_opr(act_token->type) ) { /// follow logic operator
            if ( *set_logic )                  /// check presence of only one
                print_err(2);
        }
        /// else have to follow arithmetic operator or right bracket, else error
        else if ( !is_arithmetic_opr(act_token->type) && act_token->type != RIGHT_R_BRACKET ) 
            print_err(2);
    }

    *prev_token = act_token->type;
    if ( is_logic_opr(*prev_token) ) /// sets presence if logic operator in actual token
        *set_logic = true;
}

void do_until_left_bracket (stack_t *operators_stack, stack_t *output_stack) {

    while ( !S_Empty(operators_stack) ) {
        /// obtaining operands and operators from temporary stack and their relocating to the output stack
        token_t * stack_token = copy_token(S_Top(operators_stack));
        if ( stack_token->type != LEFT_R_BRACKET ) { /// check conditions for terminates before emptying all stack
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

    /// case for empty stack, marks in precedence table or left bracket
    if ( S_Empty(operators_stack) || shift_to_stack(act_token, stack_token) || stack_token->type == LEFT_R_BRACKET ) {
        stack_token = copy_token(act_token);
        S_Push(operators_stack, stack_token);
    }
    else {
        /// have to marks of reduction in the table and stack dont have to be empty
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

    /// concat or compare two strings    
    if ( !is_number(operand_1->data_type) && !is_number(operand_2->data_type) && (operator == ADD || is_logic_opr(operator)) )
        return STRING;
    /// both operand are number type
    else if ( (is_number(operand_1->data_type) || is_number(operand_2->data_type)) && types_control(operand_1->data_type, operand_2->data_type) ) {
        if ( operator == INT_DIV )  /// integer division
            return INTEGER;
        /// case if one of the operands is type of double or operator is division
        else if ( ((operand_1->data_type != INT_NUMBER && operand_1->data_type != INTEGER) || 
             (operand_2->data_type != INT_NUMBER && operand_2->data_type != INTEGER)) ||
             operator == DIV )
             return DOUBLE;
        /// for example addition two integer numbers
        return INTEGER;
    }
    else /// not permissible combination of types
        print_err(4);

    return 0;
}

void postfix_to_instr (stack_t *postfix_stack, function_t *act_function, variable_t *l_value) {

    stack_t *output_stack = S_Init();

    variable_t *new_var = init_variable();   
    int ret_type;

    while ( !S_Empty(postfix_stack) ) {
        token_t * act_token = S_Top(postfix_stack);
        S_Pop(postfix_stack);
        if ( is_operand(act_token->type) ) {
            variable_t * operand = find_var(act_token, act_function);
            S_Push(output_stack, operand);
            /// allocation for empty string, to allow access to it, but only at string type of operand
            if ( !is_number(operand->data_type) && operand->data.str == NULL ) { 
                operand->data.str = (char *)malloc(1);
                strcpy(operand->data.str,"");
            }
            list_insert("PUSHS ", operand, NULL, NULL); /// adds operand to the data stack of interpret
            ret_type = operand->data_type;              /// sets return type for case assignment, not expression
        }
        else {
            variable_t * operand_1 = S_Top(output_stack); /// obtaining fist operand
            S_Pop(output_stack);
            variable_t *operand_2 = S_Top(output_stack);  /// obtaining second operand
            S_Pop(output_stack);
            ret_type = check_return_type(act_token->type, operand_1, operand_2); /// check return type operation with these types
            if ( operand_1->data_type != ret_type )       /// retype first operand
                retype(operand_1);
            if ( operand_2->data_type != ret_type ) {     /// retype second operand, with the help pop and storing the first one
                variable_t * tmp = create_var("PRINT ", false);
                list_insert("POPS ", tmp, NULL, NULL);
                retype(operand_2);
                list_insert("PUSHS ", tmp, NULL, NULL);
            } 
            if ( is_logic_opr(act_token->type) )  /// sets type for case that logic operator was used
                ret_type = INTEGER;

            /// generating competent instruction based on type of operator
            variable_t * tmp = create_var("PRINT ", false);
            switch(act_token->type) {
                case ADD:  
                            if ( operand_1->data_type == STRING && operand_2->data_type == STRING ) { /// concat two strings
                                concat(); 
                                break;
                            }
                            /// classic addition
                            list_insert("ADDS ", NULL, NULL, NULL); break;
                case SUB: 
                            list_insert("SUBS ", NULL, NULL, NULL); break;
                case MUL: 
                            list_insert("MULS ", NULL, NULL, NULL); break;
                case DIV: 
                            list_insert("DIVS ", NULL, NULL, NULL); break;
                case INT_DIV:
                            /// secure calculating integer division
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
            stack_var->data_type = ret_type; /// auxiliary storing type of last result, for next calculation
            S_Push(output_stack, stack_var);
            new_var = stack_var;             /// storing last type of result, for the eventual ending
        }
    }
    /// case if it is only assignment
    if ( new_var->data_type == 0 )
        new_var->data_type = ret_type;

    /// check result types with type of left value
    if ( l_value ) {
        /// check permissible combination of types
        types_control(ret_type, l_value->data_type); 
        if ( ret_type != l_value->data_type )
            retype(new_var);
    }
}

void infix_to_postfix (function_t *act_function, variable_t *l_value) {

    token_t *act_token;
    token_t *stack_token;
    unsigned count_of_bracket = 0;
    /// auxiliary variable for right count operands and operator in expression
    int sum_count = 1;
    bool logic_on = false;
 
    stack_t *output_stack = S_Init();

    stack_t *infix_stack = S_Init();

    act_token = getToken();
    /// auxiliary constant, which means begin of the expression
    int prev_token = FIRST_TOKEN;

    /// marks, which means end of the expression
    while ( act_token->type != SEMICOLON && act_token->type != EOL && 
            act_token->type != THEN && act_token->type != COMMA
            ) {

        /// check the correctness of the actual sequence
        correct_expr(act_token, &prev_token, &logic_on);
        /// operand adds to the output stack stack with postfix expression
        if ( is_operand(act_token->type) ) {
            sum_count--;
            stack_token = copy_token(act_token);
            S_Push(output_stack, stack_token);
        }
        /// left bracket adds to the temporary stack
        else if ( act_token->type == LEFT_R_BRACKET ) {
            count_of_bracket++;
            stack_token = copy_token(act_token);
            S_Push(infix_stack, stack_token);
        }
        /// right bracket means reducing expression on the temporary stack until to left bracket
        else if ( act_token->type == RIGHT_R_BRACKET ) {
            count_of_bracket--;
            do_until_left_bracket(infix_stack, output_stack);
        }
        /// at the operator decides multiple factors in other function, which is calling
        else if ( is_arithmetic_opr(act_token->type) || is_logic_opr(act_token->type) ) {
            sum_count++;
            stack_token = S_Top(infix_stack);
            do_operation(infix_stack, output_stack, act_token, stack_token);
        }
        act_token = getToken();
    }

    /// relocating eventually hinges on the temporary stack
    while ( !S_Empty(infix_stack) ) {
		S_Push(output_stack, copy_token(S_Top(infix_stack)));
		S_Pop(infix_stack);
    }

    /// ensure the correct order
    S_Copy(infix_stack, output_stack);

    /// incorrect count of brackets or operands and operators
    if ( count_of_bracket || sum_count )
        print_err(2);
    /// auxiliary flag, when can be using logic operator
    if ( l_value->data_type == BOOLEAN && !logic_on ) 
        print_err(4);
    if ( l_value->data_type != BOOLEAN && logic_on )
        print_err(6);
    ungetToken();

    /// back sets value, if was only a flag
    if ( l_value->data_type == BOOLEAN || l_value->data_type == 0)
        l_value = NULL;   

    /// generating instruction
    postfix_to_instr(infix_stack, act_function, l_value);
}

void control_token (int type_token, int count_params) {

    token_t *act_token = getToken();

    /// check the correctness of the sequence, predominantly when function is calling 
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

    /// check that same constant yet not store in the table
    htab_item_t *is_find = htab_find(const_symtable, const_token->str->string);

    if ( !is_find ) {
        variable_t *new_constant = init_variable();
        /// store integer constant
        if ( const_token->type == INT_NUMBER ) {
            new_constant->data_type = INTEGER;
            new_constant->data.i = strtol(const_token->str->string, NULL, 10);
        }
        /// store double constant
        else if ( const_token->type == DOUBLE_NUMBER ) {
            new_constant->data_type = DOUBLE;
            new_constant->data.d = strtod(const_token->str->string, NULL);
        }
        /// store string constant and alloc new memory
        else if ( const_token->type == TEXT ) {
            new_constant->data_type = STRING;
            new_constant->data.str = malloc((const_token->str->length)+1);
            if(!new_constant->data.str)
                print_err(99);
            strcpy(new_constant->data.str, const_token->str->string);
        }
        new_constant->constant = true;
        /// find newly stored variable and sets of the relevant parameters
        is_find = htab_insert(const_symtable, const_token->str->string); 
        if( !is_find )
            print_err(99);
        is_find->is_function = 0;
        is_find->data.var = new_constant;
    }

    return is_find->data.var;
}

variable_t *find_var (token_t *find_token, function_t *act_function) {

    /// find variable in local symtable of actual function
    if ( find_token->type == ID ) {
        htab_item_t *is_found = htab_find(act_function->local_symtable, find_token->str->string);
        /// we did not find
        if ( !is_found )
            print_err(3);
        return is_found->data.var;
    }
    /// we are searching constant
    else if ( find_token->type >= INT_NUMBER && find_token->type <= TEXT )
        return store_constant(find_token); 

    return NULL;
}

variable_t *next_params (function_t *act_function, int expect_type) {

    token_t *act_token = getToken();

    /// parameter have to be operand
    if ( !is_operand(act_token->type) ) 
        print_err(2);
    variable_t *act_param = find_var(act_token, act_function); /// find variable in local symtable
    types_control(expect_type, act_param->data_type);          /// check expected types

    return act_param;
}

void builtin_function (token_t *function, function_t *act_function, variable_t *l_value) {

    int function_name = function->type;

    /// check correctness of sequence
    control_token(LEFT_R_BRACKET, 0);

    /// have to follow parameters
    token_t *act_token = getToken();
    if (act_token->type == RIGHT_R_BRACKET)
        print_err(4);
    else 
        ungetToken();

    if ( function_name == LENGTH ) {
        /// obtaining and control count, types of paramaters
        variable_t *param = next_params(act_function, STRING);
        control_token(RIGHT_R_BRACKET, 0);
        /// check compatibility of return types with left value
        if (l_value)          
            types_control(INTEGER, l_value->data_type);
        /// auxiliary store parameters to the data stack of interpret
        list_insert("PUSHS ", param, NULL, NULL);
        /// generating instruction
        length_of_str(l_value);
    }
    else if ( function_name == SUBSTR ) {
        /// obtaining and control count, types of paramaters
        variable_t *param_1 = next_params(act_function, STRING);
        control_token(COMMA, 2);
        variable_t *param_2 = next_params(act_function, INTEGER);
        control_token(COMMA, 1);
        variable_t *param_3 = next_params(act_function, INTEGER);
        control_token(RIGHT_R_BRACKET, 0);
        /// check compatibility of return types with left value
        if (l_value)
            types_control(STRING, l_value->data_type);
        /// auxiliary store parameters to the data stack of interpret
        list_insert("PUSHS ", param_1, NULL, NULL);
        list_insert("PUSHS ", param_2, NULL, NULL); 
        /// check possibly implicitly retyping
        if(param_2->data_type == DOUBLE)
            list_insert("FLOAT2R2EINTS ", NULL, NULL, NULL);
        list_insert("PUSHS ", param_3, NULL, NULL);
        if(param_3->data_type == DOUBLE)
            list_insert("FLOAT2R2EINTS ", NULL, NULL, NULL);
        /// generating instruction
        substr();
    }
    else if ( function_name == ASC ) {
        /// obtaining and control count, types of paramaters
        variable_t *param_1  = next_params(act_function, STRING);
        control_token(COMMA, 1);
        variable_t *param_2 = next_params(act_function, INTEGER);
        control_token(RIGHT_R_BRACKET, 0);
        /// check compatibility of return types with left value
        if (l_value)
            types_control(INTEGER, l_value->data_type);
        /// auxiliary store parameters to the data stack of interpret
        list_insert("PUSHS ", param_1, NULL, NULL);
        list_insert("PUSHS ", param_2, NULL, NULL);
        /// check possibly implicitly retyping
        if(param_2->data_type == DOUBLE)
            list_insert("FLOAT2R2EINTS ", NULL, NULL, NULL);
        /// generating instruction       
        asc(l_value);

    }
    else {
        /// obtaining and control count, types of paramaters
        variable_t *param = next_params(act_function, INTEGER);
        control_token(RIGHT_R_BRACKET,0);
        /// check compatibility of return types with left value
        if (l_value)
            types_control(STRING, l_value->data_type);
        /// auxiliary store parameters to the data stack of interpret
        list_insert("PUSHS ", param, NULL, NULL);
        /// check possibly implicitly retyping
        if(param->data_type == DOUBLE)
            list_insert("FLOAT2R2EINTS ", NULL, NULL, NULL);
        /// generating instruction
        chr();
    }
}

int users_function (token_t *act_token, function_t *act_function, variable_t *l_value) {

    /// find ID of function in global symtable
    htab_item_t *search_function = htab_find(global_symtable, act_token->str->string);
    function_t *users_function;
    
    users_function = search_function->data.fun;

    control_token(LEFT_R_BRACKET, 0);
    int count_of_params = users_function->params->length; /// obtaining count parameters of function

    /// if there are parameters, dont follow right bracker (bad count of params)
    if ( count_of_params ) {
        act_token = getToken();
        if (act_token->type == RIGHT_R_BRACKET)
            print_err(4);
        else 
            ungetToken();
    }

    for( int i = 0; i < count_of_params; i++ ) {
        /// decoding data types of parameters
        int type_param = decode_type(users_function->params->string[i]);
        /// obtaining and control types of paramaters
        variable_t * param = next_params(act_function, type_param);
        /// control correctness of sequence
        if ( i != count_of_params-1 )
            control_token(COMMA, count_of_params-i);
        /// auxiliary store parameter to the data stack of interpret
        list_insert("PUSHS ", param, NULL, NULL);
        /// check possibly implicitly retyping
        if ( param->data_type != type_param )
            retype(param);
    }
    control_token(RIGHT_R_BRACKET, 0);
    /// check compatibility of return types with left value
    if ( l_value )
        types_control(search_function->data.fun->return_type, l_value->data_type);

    variable_t * tmp = create_var(search_function->key, true);
    /// ensuring jump
    list_insert("CALL ", tmp, NULL, NULL);

    return users_function->return_type;
}

void expression (function_t *act_function, variable_t *l_value) {

    token_t *act_token = getToken();

    /// control whether it is calling builtin function
    if ( is_builtin_function(act_token->type) ) 
        builtin_function(act_token, act_function, l_value);
    /// control whether it is calling users function
    else if ( is_users_function(act_token, act_function) ) {
        int fun_type = users_function(act_token, act_function, l_value);
        /// check possibly implicitly retyping
        if(l_value && fun_type != l_value->data_type ) {
            if(fun_type == INTEGER)
                list_insert("INT2FLOATS ",NULL, NULL, NULL);
            else if(fun_type == DOUBLE)
                list_insert("FLOAT2INTS ",NULL, NULL, NULL);
        }
    }
    /// case for simple assignment or processing expressions
    else
        infix_to_postfix(act_function, l_value);
}