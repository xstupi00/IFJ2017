#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "scanner.h"
#include "error.h"
#include "expr.h"
#include "stack.h"
#include "symtable.h"
#include "semantic_control.h"
#include "generate.h"

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
void infix_to_postfix (function_t *act_function, variable_t *l_value);
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
    //printf("is_users_foo %s\n", search_function->key);

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
    if ( (new_token = malloc(sizeof(token_t))) == NULL )
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

int check_return_type(int *operator, variable_t *operand_1, variable_t *operand_2) {

    /*if ( *operator == ADD && operand_1->data_type == TEXT && operand_2->data_type == TEXT ) 
        return TEXT;
    else if ( ((operand_1->data_type == DOUBLE_NUMBER || operand_2->data_type == DOUBLE_NUMBER) &&
                types_control(operand_1->data_type, operand_2->data_type) && *operator != INT_DIV) ||
                (types_control(operand_1->data_type, operand_2->data_type) && *operator == DIV) )
        return DOUBLE_NUMBER;
    else if ( (operand_1->data_type == INT_NUMBER && operand_2->data_type == INT_NUMBER) ||
                ( (types_control(operand_1->data_type, operand_2->data_type) && (*operator == INT_DIV)) ) ) 
        return INT_NUMBER; */
    
    if ( !is_number(operand_1->data_type) && !is_number(operand_2->data_type) && (*operator == ADD || is_logic_opr(*operator)) )
        return STRING;
    else if ( (is_number(operand_1->data_type) || is_number(operand_2->data_type)) && types_control(operand_1->data_type, operand_2->data_type) ) {
        if ( (((operand_1->data_type != INT_NUMBER && operand_1->data_type != INTEGER) || 
             (operand_2->data_type != INT_NUMBER && operand_2->data_type != INTEGER)) &&
             *operator != INT_DIV) || *operator == DIV )
             return DOUBLE;
        return INTEGER;
    }
    //else if ( !is_number(operand_1->data_type) && !is_number(operand_2->data_type) && is_logic_opr(*operator) )
    //    return INTEGER;
    else
        print_err(4);

    return 0;
}

void control_postfix (stack_t *postfix_stack, function_t *act_function, variable_t *l_value) {

    stack_t *output_stack;
    if ( (output_stack = (stack_t *) malloc(sizeof(stack_t))) == NULL )
        print_err(99);
    S_Init(output_stack);

    token_t *act_token;
    variable_t *operand_1;
    variable_t *operand_2;
    variable_t *operand;
    int ret_type;
    int prev_type = -1;
    bool is_string = false;

    variable_t *new_var = init_variable();   
    new_var->data.i = -1;

    while ( !S_Empty(postfix_stack) ) {
        act_token = S_Top(postfix_stack);
        S_Pop(postfix_stack);
        if ( is_operand(act_token->type) ) {
            //printf("i have token %d\t%s\n", act_token->type, act_token->str->string);          
            operand = find_var(act_token, act_function);
            S_Push(output_stack, operand);
            ret_type = operand->data_type;
        }
        else {
            operand_1 = S_Top(output_stack);
            S_Pop(output_stack);
            operand_2 = S_Top(output_stack);
            S_Pop(output_stack);
            
            ret_type = check_return_type(&act_token->type, operand_1, operand_2);
            //printf("types is: %d\n", ret_type);
            if ( prev_type != ret_type && prev_type != -1 ) {
                if ( operand_2->data.i != -1 )
                    retype(operand_2);
                else
                    retype(operand_1);
                }

            if ( operand_2->data.i != -1 ) {
                if ( !is_number(operand_2->data_type) && operand_2->data.str == NULL ) {
                    operand_2->data.str = (char *)malloc(1);
                    strcpy(operand_2->data.str,"");
                }
                if ( operand_2->data_type == STRING )
                    is_string = true;
                /*if ( !operand_2->constant )
                    operand_2 = create_var(operand_2->data.str);*/
                list_insert("PUSHS ", operand_2, NULL, NULL);
                if ( operand_2 != NULL && operand_2->data_type != ret_type)  
                    retype(operand_2);
            }
            if ( operand_1->data.i != -1 ) {
                if ( !is_number(operand_1->data_type) && operand_1->data.str == NULL ) {
                    operand_1->data.str = (char *)malloc(1);
                    strcpy(operand_1->data.str,"");
                }
                if ( operand_1->data_type == STRING )
                    is_string &= 1;
                /*if ( !operand_1->constant )
                    operand_1 = create_var(operand_1->data.str);*/
                list_insert("PUSHS ", operand_1, NULL, NULL);
                if ( operand_1 != NULL && operand_1->data_type != ret_type)
                    retype(operand_1);
            }
            if ( is_logic_opr(act_token->type) )  
                ret_type = INTEGER;
            //printf("EXECUTE: %d\n",act_token->type);
            switch(act_token->type) {
                case ADD: if ( is_string ) {concat();break;}
                        list_insert("ADDS ", NULL, NULL, NULL);break;
                case SUB: list_insert("SUBS ", NULL, NULL, NULL);break;
                case MUL: list_insert("MULS ", NULL, NULL, NULL);break;
                case DIV: list_insert("DIVS ", NULL, NULL, NULL);break;
                case INT_DIV: list_insert("DIVS ", NULL, NULL, NULL);break;
                case LESS: list_insert("LTS ", NULL, NULL, NULL);break;
                case GREATER: list_insert("GTS ", NULL, NULL, NULL);break;
                case ASSIGNMENT_EQ: list_insert("EQS ", NULL, NULL, NULL);break;
                case LESS_EQ: list_insert("GTS ", NULL, NULL, NULL);
                        list_insert("NOTS ", NULL, NULL, NULL);break;
                case GREATER_EQ: list_insert("LTS ", NULL, NULL, NULL);
                        list_insert("NOTS ", NULL, NULL, NULL);break;
                case NEQ:list_insert("EQS ", NULL, NULL, NULL);
                        list_insert("NOTS ", NULL, NULL, NULL);break;
            }
            prev_type = ret_type;
            new_var->data_type = ret_type;
            S_Push(output_stack, new_var);
        }
    }
    if ( !S_Empty(output_stack) ) {
        operand_1 = S_Top(output_stack);
        new_var->data_type = operand_1->data_type;
        if ( operand_1->data.i != -1) {
            if ( !is_number(operand_1->data_type) && operand_1->data.str == NULL ){
                operand_1->data.str = (char *)malloc(1);
                strcpy(operand_1->data.str,"");
            }
            /*if ( !operand_1->constant )
                operand_1 = create_var(operand_1->data.str);*/
            list_insert("PUSHS ", operand_1, NULL, NULL);
        }
    }
    //printf("types is: %d\n", ret_type);
    if ( l_value != NULL) {
        //printf("return type is: %d\nl_value type is: %d\n", ret_type, l_value->data_type);
        types_control(ret_type, l_value->data_type); 
        if ( ret_type != l_value->data_type ) {
            //printf("iam here---%d\n", new_var->data_type);
            retype(new_var);
        }
    }
}

void infix_to_postfix (function_t *act_function, variable_t *l_value) {

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
            //printf("in postfix: %d\n", stack_token->type);
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
    S_Destroy(output_stack);

    if ( count_of_bracket || sum_count )
        print_err(2);
    ungetToken();

    control_postfix(infix_stack, act_function, l_value);
    //S_Print(infix_stack);
}

void control_token (int type_token) {

    token_t *act_token = getToken();

    //printf("first type is: %d\tsecond type is: %d\n", type_token, act_token->type);

    if ( act_token->type != type_token ) {
        if ( type_token == LEFT_R_BRACKET && act_token->type == RIGHT_R_BRACKET )           // i want "(" but token is ")" -> length)
            print_err(2);
        else if ( (type_token != RIGHT_R_BRACKET && act_token->type == RIGHT_R_BRACKET) ||  // token is ")" but i dont want some ")"
                  (type_token != COMMA && act_token->type == COMMA) ) {                     // token is "," but i dont want ","
                act_token = getToken();
                if ( act_token->type == RIGHT_R_BRACKET )                                  // case for (param,)
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
            new_constant->data.str = malloc(const_token->str->capacity);
            if(!new_constant->data.str)
                print_err(99);
            strcpy(new_constant->data.str, const_token->str->string);
            //new_constant->data.str = const_token->str->string;
        }
        new_constant->constant = true;
        is_find = htab_insert(const_symtable, const_token->str->string); 
        if(!is_find)
            print_err(99);
        is_find->is_function = 0;
        is_find->data.var = new_constant;
    }
    //else  
        //free(const_token->str);   

    return is_find->data.var;
}

variable_t *find_var (token_t *find_token, function_t *act_function) {

    if ( find_token->type == ID ) {
        htab_item_t *is_found = htab_find(act_function->local_symtable, find_token->str->string);
        if ( !is_found ) {
            print_err(3);
        }
        //free(find_token->str);
        //token_t *tmp = copy_token(find_token);
        //is_found->data.var->data.str = tmp->str->string;
        return is_found->data.var;
    }
    else if ( find_token->type >= INT_NUMBER && find_token->type <= TEXT )
        return store_constant(find_token); 

    return NULL;
}


int types_control (int expect_type, int real_type) {

    //if ( ((expect_type == INT_NUMBER || expect_type == DOUBLE_NUMBER) && real_type == TEXT) ||
    //     ((real_type == INT_NUMBER || real_type == DOUBLE_NUMBER) && expect_type == TEXT) )

    if ( (is_number(expect_type) && !is_number(real_type)) ||
          (is_number(real_type) && !is_number(expect_type) ) ) 
        print_err(4);

    return 1;
}

variable_t *next_params (function_t *act_function, int expect_type) {

    token_t *act_token = getToken();
    //printf("token %d\n", act_token->type);

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
        variable_t *param = next_params(act_function, STRING);
        control_token(RIGHT_R_BRACKET);
        types_control(INTEGER, l_value->data_type);
        //if ( !param->constant )
                //param = create_var(param->data.str);
        list_insert("PUSHS ", param, NULL, NULL);
        length_of_str(l_value);
        //printf("obsah: %d\n",param->data_type);
    }
    else if ( function_name == SUBSTR ) {
        variable_t *param_1 = next_params(act_function, STRING);
        //printf("obsah: %d\n",param_1->data_type);
        control_token(COMMA);
        variable_t *param_2 = next_params(act_function, INTEGER);
        //printf("obsah: %d\n",param_2->data_type);
        control_token(COMMA);
        variable_t *param_3 = next_params(act_function, INTEGER);
        //printf("obsah: %d\n",param_3->data_type);
        control_token(RIGHT_R_BRACKET);
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
        //printf("obsah: %d\n",param_1->data_type);
        control_token(COMMA);
        variable_t *param_2 = next_params(act_function, INTEGER);
        //printf("obsah: %d\n",param_2->data_type);
        control_token(RIGHT_R_BRACKET);
        types_control(INTEGER, l_value->data_type);
        list_insert("PUSHS ", param_1, NULL, NULL);
        list_insert("PUSHS ", param_2, NULL, NULL);
        if(param_2->data_type == DOUBLE)
            list_insert("FLOAT2R2EINTS ", NULL, NULL, NULL);
        asc(l_value);

    }
    else {
        variable_t *param = next_params(act_function, INTEGER);
        //printf("obsah: %d\n",param->data_type);
        control_token(RIGHT_R_BRACKET);
        types_control(STRING, l_value->data_type);
        list_insert("PUSHS ", param, NULL, NULL);
        if(param->data_type == DOUBLE)
            list_insert("FLOAT2R2EINTS ", NULL, NULL, NULL);
        chr();
    }

    //control_token(RIGHT_R_BRACKET);
    //types_control(act_function->return_type, l_value->data_type);
}

int decode_type (char type) {

    if ( type == 'i' )
        return INTEGER;
    else if ( type == 'd' )
        return DOUBLE;
    else // !!!
        return STRING;
}

int users_function (token_t *act_token, function_t *act_function, variable_t *l_value) {

    htab_item_t *search_function = htab_find(global_symtable, act_token->str->string);
    //printf("%s\n", act_token->str->string);

    function_t *users_function;
    variable_t *param;

    users_function = search_function->data.fun;

    control_token(LEFT_R_BRACKET);
    
    int count_of_params = users_function->params->length;
    //printf("params: %d\n", count_of_params);
    for( int i = 0; i < count_of_params; i++ ) {
        int type_param = decode_type(users_function->params->string[i]);
        param = next_params(act_function, type_param);
        if ( i != count_of_params-1 )
            control_token(COMMA);
        list_insert("PUSHS ", param, NULL, NULL);
        if ( param->data_type != type_param)
            retype(param);
        //printf("obsah: %d\n",param->data_type);   
    }

    control_token(RIGHT_R_BRACKET);
    //printf("%d\t%d\n", search_function->data.fun->return_type, l_value->data_type);
    types_control(search_function->data.fun->return_type, l_value->data_type);
    variable_t * tmp = create_var(search_function->key, true);
    list_insert("CALL ", tmp, NULL, NULL);
    return users_function->return_type;
}

void expression (function_t *act_function, variable_t *l_value) {
    token_t *act_token = getToken();
    if ( is_builtin_function(act_token->type) ) 
        builtin_function(act_token, act_function, l_value);
    else if ( is_users_function(act_token, act_function) ){
        int f_type = users_function(act_token, act_function, l_value);
        if(f_type != l_value->data_type ){
            if(f_type == INTEGER){
                list_insert("INT2FLOATS ",NULL, NULL, NULL);
            }
            else if(f_type == DOUBLE){
                list_insert("FLOAT2INTS ",NULL, NULL, NULL);
            } 
        }
    }
    else
        infix_to_postfix(act_function, l_value);
}