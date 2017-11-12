#ifndef EXPR_H
#define EXPR_H

#include "scanner.h"

#define SIZE_TABLE 16
#define FIRST_TOKEN 99

int is_arithmetic_opr (int token_type);
int is_logic_opr (int token_type);
int is_operand (int token_type);
int shift_to_stack (T_Token *entry_token, T_Token *stack_token);
void correct_expr (T_Token *act_token, int *prev_token, bool *set_logic);
void infix_to_postfix ();
T_Token *copy_token (T_Token *act_token);
void expression ();

#endif // EXPR_H