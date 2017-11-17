#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "scanner.h"
#include "error.h"
#include "expr.h"
#include "stack.h"
#include "symtable.h"
#include "semantic_control.h"

bool is_arithmetic_opr (int token_type);
bool is_logic_opr (int token_type);
bool is_operand (int token_type);
bool is_builtin_function (int token_type);
bool shift_to_stack (token_t *entry_token, token_t *stack_token);
void correct_expr (token_t *act_token, int *prev_token, bool *set_logic);
void do_until_left_bracket (stack_t *operators_stack, stack_t *output_stack);
void do_operation (stack_t *operators_stack, stack_t *output_stack, token_t *act_token, token_t *stack_token);
void builtin_function (token_t *function, function_t *act_function, variable_t *l_value);
variable_t *find_var (token_t *find_token, function_t *act_function);
int types_control (int expect_type, int real_type);
void infix_to_postfix ();
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
        //ungetToken(); 
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

token_t* copy_token(token_t *act_token) {

    token_t *new_token = NULL;
    if ( (new_token = malloc(sizeof(token_t))) == NULL )
        print_err(99);

    new_token->str = act_token->str;
    new_token->type = act_token->type;

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

    if ( is_arithmetic_opr(*prev_token) || *prev_token == LEFT_R_BRACKET 
        || *prev_token == FIRST_TOKEN || is_logic_opr(*prev_token) ) 
    {
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

    token_t *stack_token;

    while ( !S_Empty(operators_stack) ) {
        stack_token = copy_token(S_Top(operators_stack));
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

void infix_to_postfix () {

    token_t *act_token;
    token_t *stack_token;
    unsigned count_of_bracket = 0;
    int sum_count = 1;
    bool logic_on = false;
 
    stack_t *output_stack;
    if ( (output_stack = (stack_t *) malloc(sizeof(stack_t))) == NULL )
        print_err(99);
    S_Init(output_stack);

    stack_t *infix_stack;
    if ( (infix_stack = (stack_t *) malloc(sizeof(stack_t))) == NULL )
        print_err(99);
    S_Init(infix_stack);

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
        stack_token = copy_token(S_Top(infix_stack));
		S_Push(output_stack, stack_token);
		S_Pop(infix_stack);
    }
    S_Copy(infix_stack, output_stack);
    S_Destroy(output_stack);

    if ( count_of_bracket || sum_count )
        print_err(2);
    ungetToken();

    //S_Print(infix_stack);
}

void control_token (int type_token) {

    token_t *act_token = getToken();

    if ( act_token->type != type_token ) {
        if ( type_token == LEFT_R_BRACKET && act_token->type == RIGHT_R_BRACKET )           // i want "(" but token is ")" -> length)
            print_err(2);
        else if ( (type_token != RIGHT_R_BRACKET && act_token->type == RIGHT_R_BRACKET) ||  // token is ")" but i dont want some ")"
                  (type_token != COMMA && act_token->type == COMMA) ) {                     // token is "," but i dont want ","
                act_token = getToken();
                if ( act_token->type == RIGHT_R_BRACKET )                                   // case for (param,)
                    print_err(2);
                else if ( is_operand(act_token->type) )                                     // case for (param,param) instead of (param)
                    print_err(4);
                ungetToken();
        }
        print_err(2);
    }
}

variable_t *store_constant (token_t *const_token) {

    htab_item_t *is_find = htab_find(const_symtable, const_token->str->string);

    if ( !is_find ) {
        variable_t *new_constant = (variable_t *) malloc(sizeof(variable_t));    
        new_constant->data_type = const_token->type;
        if ( new_constant->data_type == INT_NUMBER )
            new_constant->data.i = strtol(const_token->str->string, NULL, 10);
        else if ( new_constant->data_type == DOUBLE_NUMBER )
            new_constant->data.d = strtod(const_token->str->string, NULL);
        else if ( new_constant->data_type == TEXT )
            new_constant->data.str = const_token->str->string;
        is_find = htab_insert(const_symtable, const_token->str->string); 
        is_find->is_function = 0;
        is_find->data.var = new_constant;
    }
    else 
        free(const_token->str);   

    printf("type is(from store_const): %d\n", is_find->data.var->data_type);

    return is_find->data.var;
}

variable_t *find_var (token_t *find_token, function_t *act_function) {

    if ( find_token->type == ID ) {
        htab_item_t *is_found = htab_find(act_function->local_symtable, find_token->str->string);
        if ( !is_found ) {
            print_err(3);
        }
        return is_found->data.var;
    }
    else if ( find_token->type >= INT_NUMBER && find_token->type <= TEXT )
        return store_constant(find_token); 

    return NULL;
}

int types_control (int expect_type, int real_type) {

    if ( ((expect_type == INT_NUMBER || expect_type == DOUBLE_NUMBER) && real_type == TEXT) ||
         ((real_type == INT_NUMBER || real_type == DOUBLE_NUMBER) && expect_type == TEXT) )
        
        print_err(4);

    return 1;
}

variable_t *next_params (function_t *act_function, int expect_type) {

    token_t *act_token = getToken();

    if ( !is_operand(act_token->type) ) {
        if ( act_token->type == RIGHT_R_BRACKET )
            print_err(4);
        print_err(2);
    }
    variable_t *act_param = find_var(act_token, act_function);
    types_control(expect_type, act_param->data_type);

    return act_param;
}

void builtin_function (token_t *function, function_t *act_function, variable_t *l_value) {

    int function_name = function->type;

    control_token(LEFT_R_BRACKET);

    if ( function_name == LENGTH ) {
        variable_t *param = next_params(act_function, TEXT);
        printf("obsah: %s\n",param->data.str);
    }
    else if ( function_name == SUBSTR ) {
        variable_t *param_1 = next_params(act_function, TEXT);
        printf("obsah: %s\n",param_1->data.str);
        control_token(COMMA);
        variable_t *param_2 = next_params(act_function, INT_NUMBER);
        printf("obsah: %d\n",param_2->data.i);
        control_token(COMMA);
        variable_t *param_3 = next_params(act_function, TEXT);
        printf("obsah: %s\n",param_3->data.str);
    }
    else if ( function_name == ASC ) {
        variable_t *param_1  = next_params(act_function, TEXT);
        printf("obsah: %s\n",param_1->data.str);
        control_token(COMMA);
        variable_t *param_2 = next_params(act_function, INT_NUMBER);
        printf("obsah: %d\n",param_2->data.i);
    }
    else {
        variable_t *param = next_params(act_function, INT_NUMBER);
        printf("obsah: %d\n",param->data.i);
    }

    control_token(RIGHT_R_BRACKET);
    types_control(act_function->return_type, l_value->data_type);
}

int decode_type (char type) {

    if ( type == 'i' )
        return INT_NUMBER;
    else if ( type == 'd' )
        return DOUBLE_NUMBER;
    else // !!!
        return TEXT;
}

void users_function (token_t *act_token, function_t *act_function, variable_t *l_value) {

    htab_item_t *search_function = htab_find(global_symtable, act_token->str->string);
    function_t *users_function = search_function->data.fun;

    control_token(LEFT_R_BRACKET);
    
    int count_of_params = users_function->params->length;
    for( int i = 0; i <= count_of_params; i++ ) {
        int type_param = decode_type(users_function->params->string[i]);
        variable_t *param = next_params(act_function, type_param);
        if ( i != count_of_params )
            control_token(COMMA);
        printf("obsah: %s\n",param->data.str);   
    }

    control_token(RIGHT_R_BRACKET);
    types_control(act_function->return_type, l_value->data_type);
}

void expression (function_t *act_function, variable_t *l_value) {

    token_t *act_token = getToken();

    if ( is_builtin_function(act_token->type) ) 
        builtin_function(act_token, act_function, l_value);
    else if ( is_users_function(act_token, act_function) )
        users_function(act_token, act_function, l_value);
    else
        infix_to_postfix();
}