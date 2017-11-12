#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "scanner.h"
#include "error.h"
#include "expr.h"
#include "stack.h"

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

int is_arithmetic_opr (int token_type) {

    if ( token_type >= ADD && token_type <= INT_DIV )
        return 1;
    else    
        return 0;
}

int is_logic_opr (int token_type) {

    if ( token_type >= LESS && token_type <= NEQ )
        return 1;
    else    
        return 0;
}

int is_operand (int token_type) {

    if ( token_type >= ID && token_type <= TEXT )
        return 1;
    else   
        return 0;
}

T_Token* copy_token(T_Token *act_token) {

    T_Token *new_token = NULL;
    if ( (new_token = malloc(sizeof(T_Token))) == NULL )
        print_err(99);

    new_token->str = act_token->str;
    new_token->type = act_token->type;

    return new_token;
}

int shift_to_stack (T_Token *entry_token, T_Token *stack_token) {

    unsigned index_y = entry_token->type - ADD;
    unsigned index_x = stack_token->type - ADD;

    if ( stack_token->type == LEFT_R_BRACKET ) 
        index_x = 4;
    else if ( stack_token->type == RIGHT_R_BRACKET )
        index_x = 5;

    if ( precedence_table[index_x][index_y] == '<')
        return 1;
    else 
        return 0;
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

void infix_to_postfix () {

    T_Token *act_token;
    T_Token *stack_token;
    unsigned count_of_bracket = 0;
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
    int prev_token = FIRST_TOKEN;

    while ( act_token->type != SEMICOLON && act_token->type != EOL && 
            act_token->type != THEN && act_token->type != COMMA
            ) {

        correct_expr(act_token, &prev_token, &logic_on);
        if ( is_operand(act_token->type) ) {
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
            while ( !S_Empty(infix_stack) ) {
                stack_token = copy_token(S_Top(infix_stack));
                if ( stack_token->type != LEFT_R_BRACKET ) {
                    S_Push(output_stack, stack_token);
                    S_Pop(infix_stack);    
                }
                else {
                    S_Pop(infix_stack);
                    break;
                }
            }
        }
        else if ( is_arithmetic_opr(act_token->type) || is_logic_opr(act_token->type) ) {
            stack_token = S_Top(infix_stack);
            if ( S_Empty(infix_stack) || shift_to_stack(act_token, stack_token) || stack_token->type == LEFT_R_BRACKET ) {
                stack_token = copy_token(act_token);
                S_Push(infix_stack, stack_token);
            }
            else {
                while ( !S_Empty(infix_stack) && !shift_to_stack(act_token, stack_token) ) {
                    stack_token = copy_token(stack_token);
                    S_Push(output_stack, stack_token);
                    S_Pop(infix_stack);
                    stack_token = S_Top(infix_stack);
                }
                stack_token = copy_token(act_token);
                S_Push(infix_stack, stack_token);
            }
        }
        act_token = getToken();
    }

    while ( !S_Empty(infix_stack) ) {
        stack_token = copy_token(S_Top(infix_stack));
		S_Push(output_stack, stack_token);
		S_Pop(infix_stack);
    }
    S_Destroy(infix_stack);

    if ( count_of_bracket )
        print_err(2);
    ungetToken();

    S_Print(output_stack);
}

void expression () {

    infix_to_postfix();
}