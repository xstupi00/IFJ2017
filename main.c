#include <stdio.h>
#include "scanner.h"
#include "strlib.h"
#include "error.h"
#include "stack.h"
#include "parser.h"

int main(){
    //FILE *fp = fopen("S.TXT","r");
    
    initToken();
    T_Token * token;
   // int i=0;
    /*for(i=0; i<80; i++){
        token = getToken();
        printf("%d\t", token->type);
        printf("%s\n", token->str->string);
    }*/
    token = getToken();
        printf("%d\t", token->type);
        printf("%s\n", token->str->string);
    ungetToken();    
    token = getToken();
        printf("%d\t", token->type);
        printf("%s\n", token->str->string);
    token = getToken();
        printf("%d\t", token->type);
        printf("%s\n", token->str->string);

    free(token->str->string);
    free(token->str);
    free(token);
    
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
}