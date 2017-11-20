#include <stdio.h>
#include "scanner.h"
#include "strlib.h"
#include "error.h"
#include "stack.h"
#include "parser.h"
#include "expr.h"
#include "generate.h"
int main(){

    initToken();
    init_global_symtables();
    list_init();
    parse();
   
    /*expression(NULL, NULL);*/
    printf(".IFJcode17\nJUMP SCOPE\n");
    /*printf("CREATEFRAME\n");
    printf("PUSHFRAME\n");

    printf("DEFVAR LF@VAR\n");
    */
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
    //printf("POPS LF@VAR\nWRITE LF@VAR\n");
    /*variable_t * str = create_var("prmenna", "TF@");
    printf("%s\n", str->data.str);
    */
}
