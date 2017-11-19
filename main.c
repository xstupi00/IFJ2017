#include <stdio.h>
#include "scanner.h"
#include "strlib.h"
#include "error.h"
#include "stack.h"
#include "parser.h"
#include "expr.h"
#include "generate.h"
int main(){
    //FILE *fp = fopen("S.TXT","r");
    //T_Token *token;
    initToken();
    init_global_symtables();

    //int i=0;
    //for(i=0; i<5; i++){
    //    token = getToken();
    //    printf("%d\t", token->type);
    //    printf("%s\n", token->str->string);
    //}
    //token = getToken();
    //expression();
       /* printf("%d\t", token->type);
       // printf("%s\n", token->str->string);
        printf("%d\n", token_is_operator(token));
    //ungetToken();   
    token = getToken();
        printf("%d\t", token->type);
        //printf("%s\n", token->str->string);
        printf("%d\n", token_is_operator(token));
    token = getToken();
        printf("%d\t", token->type);
        //printf("%s\n", token->str->string);
        printf("%d\n", token_is_operator(token));
    free(token->str->string);
    free(token->str);
    free(token);*/
    
    /*for(int i = 0; i < 5; i++){
        T_Token *tok = getToken();
        printf("%d\t", tok->type);
        printf("%s\n", tok->str);
    }*/

    /* T_Stack *s = (T_Stack *) malloc(sizeof(T_Stack));
    S_Init(s);

    for ( int i = 0; i < 20; i++) {
        int *x = malloc(sizeof(int));
        *x = i;
        S_Push(s, x);
    }

    S_Print(s);
    int j = 0;
    while ( !S_Empty(s) ) {
        S_Pop(s);
        printf("%d\n",j++);
    }

    S_Push(s,&j);
    int *f = (int *)S_Top(s);
    printf("%d\n",*f); */
    
    /*list_init();
    expression(NULL, NULL);
    printf(".IFJcode17\n");
    printf("CREATEFRAME\n");
    printf("PUSHFRAME\n");

    printf("DEFVAR LF@VAR\n");
    
    for(instruction_t * tmp = list->First; tmp!=NULL; tmp=tmp->next){
        printf("%s",tmp->instr_name); 
        if(tmp->op1){
            switch (tmp->op1->data_type){
                case INTEGER:printf("int@%d",tmp->op1->data.i);break;
                case DOUBLE:printf("float@%g",tmp->op1->data.d);break;
                case STRING:printf("string@");
                            process_string(tmp->op1->data.str); break;
                default: if(tmp->op1->data.str)printf("%s", tmp->op1->data.str);break;
            }
        }
       
        if(tmp->op2){
            switch (tmp->op2->data_type){
                case INTEGER:printf("int@%d",tmp->op2->data.i);break;
                case DOUBLE:printf("float@%g",tmp->op2->data.d); break;
                case STRING:printf("string@");
                            process_string(tmp->op2->data.str); break;
                default: if(tmp->op2->data.str)printf("%s", tmp->op2->data.str);break;
               }
        }
        if(tmp->op3)
            switch (tmp->op3->data_type){
                case INTEGER:printf("int@%d",tmp->op3->data.i);break;
                case DOUBLE:printf("float@%g",tmp->op3->data.d); break;
                case STRING:printf("string@");
                            process_string(tmp->op3->data.str); break;
                default: if(tmp->op3->data.str)printf("%s", tmp->op3->data.str);break;

        }
        printf("\n");
    }
    printf("WRITE LF@CON\n");*/
    variable_t * str = create_var(998);
    printf("%s\n", str->data.str);


}
