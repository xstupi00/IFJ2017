#include <stdio.h>
#include "scanner.h"
#include "strlib.h"
#include "error.h"

int main(){
    //FILE *fp = fopen("S.TXT","r");
    
    initToken();
    T_Token * token;
    int i=0;
    for(i=0; i<80; i++){
        token = getToken();
        printf("%d\t", token->type);
        printf("%s\n", token->str->string);
    }

    free(token->str->string);
    free(token->str);
    free(token);
    
    /*for(int i = 0; i < 5; i++){
        T_Token *tok = getToken();
        printf("%d\t", tok->type);
        printf("%s\n", tok->str);
    }*/

}