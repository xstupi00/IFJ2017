#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "scanner.h"
#include "error.h"
#include "expr.h"
#include "stack.h"

bool is_arithmetic_opr (int token_type);
bool is_logic_opr (int token_type);
bool is_operand (int token_type);
bool shift_to_stack (T_Token *entry_token, T_Token *stack_token);
void correct_expr (T_Token *act_token, int *prev_token, bool *set_logic);
void do_until_left_bracket (T_Stack *operators_stack, T_Stack *output_stack);
void do_operation (T_Stack *operators_stack, T_Stack *output_stack, T_Token *act_token, T_Token *stack_token);
void infix_to_postfix ();
T_Token *copy_token (T_Token *act_token);

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

T_Token* copy_token(T_Token *act_token) {

    T_Token *new_token = NULL;
    if ( (new_token = malloc(sizeof(T_Token))) == NULL )
        print_err(99);

    new_token->str = act_token->str;
    new_token->type = act_token->type;

    return new_token;
}

bool shift_to_stack (T_Token *entry_token, T_Token *stack_token) {

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

void correct_expr (T_Token *act_token, int *prev_token, bool *set_logic) {

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

void do_until_left_bracket (T_Stack *operators_stack, T_Stack *output_stack) {

    T_Token *stack_token;

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

void do_operation (T_Stack *operators_stack, T_Stack *output_stack, T_Token *act_token, T_Token *stack_token) {

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

    T_Token *act_token;
    T_Token *stack_token;
    unsigned count_of_bracket = 0;
    int sum_count = 1;
    bool logic_on = false;
 
    T_Stack *output_stack;
    if ( (output_stack = (T_Stack *) malloc(sizeof(T_Stack))) == NULL )
        print_err(99);
    S_Init(output_stack);

    T_Stack *infix_stack;
    if ( (infix_stack = (T_Stack *) malloc(sizeof(T_Stack))) == NULL )
        print_err(99);
    S_Init(infix_stack);

    act_token = getToken();
    //printf("First token is: %d\n", act_token->type);
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

void expression () {

    infix_to_postfix();
}