//////////////////////////////////////////////////////////////////////////
//      Vysoké Učení Technické, Fakulta Informačních Technologií        //
//      Předmět Formální jazyky a překladače                            //
//      Projekt  IFJ17							//
//      Lexikálna analýza                                               //
//      Autoři: Kristián Liščinský  (xlisci01)                          //
//              Matúš Liščinský     (xlisci02)		                //
//		Šimon Stupinský	    (xstupi00)				//
//		Vladimír Marcin	    (xmarci10)				//
//////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include "scanner.h"
#include "error.h"
#include "strlib.h"


T_Token * initToken(){
    T_Token *token = (T_Token*) malloc(sizeof(struct T_Token));
    if(!token){
        print_err(99);
        return NULL;
    }   
    
    token->str = strInit(); // alokacia pamate pre string
    //(T_string*) malloc(sizeof(T_string)*(STR_INIT + 1)); //alokácia stringu na uloženie hodnoty tokenu
    if(!token->str){
        print_err(99);
        return NULL;
    }   
    return token;
}


//funkcia na uloženie načítaného tokena do tokenovej štruktúry
T_Token *saveToken(T_Token * token, int type, T_string * t_str, bool full){
    
    if(!token){
        print_err(99);
        return NULL;
    }   
    if(full){
        if(!t_str){
            print_err(99);
            return NULL;
        }   
        unsigned length = strlen(t_str->string);
        if(length > token->str->capacity)
        {
            if(!(extendStr(token->str,length)))
            {
                print_err(99);
                return NULL;
            }   
        }
        strcpy(token->str->string, t_str->string); //priradí hodnotu tokenu do retazca
        t_str->string[0]='\0';

    }
    else 
        token->str->string[0]='\0'; 
    token->type = type; //priradí typ

    return token;
}

T_Token *getToken(T_Token * token, T_string * t_str){

    //T_string * t_str = strInit();

    int position = 0;           //index aktualneho charu
    char c;
    char hexa[4];
    hexa[3] = '\0';
    int escapovanie = 0;
    int pocet_cisel = 0;
    bool exponent = false;
    bool dot = false;
    T_States state = S_START;   //stav automatu

    //char *t_str = (char *) calloc(1, sizeof(char) * STR_INIT+1);
    if(t_str == NULL){
        print_err(99);     //nepodarila sa alokácia
        return NULL;
    }

    while(((c = fgetc(stdin)) != EOF) /*&& end!=false ? alebo ?+1*/){
        t_str->length=strlen(t_str->string);
      //  printf("dlzka :%lu\n",t_str->length);
       
        if(t_str->length+1 > t_str->capacity){
            if(!(extendStr(t_str, 2*t_str->capacity))) {
                print_err(99);
                return NULL;
            }
        }
        c = tolower(c);
        //printf("kapacita :%lu\n",t_str->capacity);
        
            //realloc(t_str->string, sizeof(char)*2*prev_size+1)
            //realloc(t_str->string, sizeof(char) * str_init);
            /*if(t_str->string == NULL){
                print_err(99);     //nepodarila sa realokácia
                return NULL;
            }
        }*/

        switch(state){
            case S_START:
                if(c == '\n'){
                    return saveToken(token, EOL, NULL, false);    //EOL
                }
                else if(isspace(c)){
                    state = S_START;    //biely znak == pokračuj ďalej
                }
                else if(isalpha(c) || c == '_'){
                    state = S_IDENTIFIER;   //identifikátor, kľúčové slovo
                    t_str->string[position++] = c;
                }
                else if(isdigit(c)){
                    state = S_NUMBER;   //číslo
                    t_str->string[position++] = c;
                }
                /*else if(c == '{'){
                    return saveToken(token, LEFT_C_BRACKET, NULL, false); //ľavá zložená zátvorka
                }
                else if(c == '}'){
                    return saveToken(token, RIGHT_C_BRACKET, NULL, false);    //pravá zložená zátvorka
                }
                */
                else if(c == '('){
                    return saveToken(token, LEFT_R_BRACKET, NULL, false);     //ľavá okrúhla zátvorka
                }
                else if(c == ')'){
                    return saveToken(token, RIGHT_R_BRACKET, NULL, false);    //pravá okrúhla zátvorka
                }
                else if(c == ','){
                    return saveToken(token, COMMA, NULL, false);  //čiarka
                }
                
                else if(c == ';'){
                    return saveToken(token, SEMICOLON, NULL, false);  //bodkočiarka
                }
                else if(c == '*'){
                    return saveToken(token, MUL, NULL, false);    //krát
                }
                else if(c == '-'){
                    return saveToken(token, SUB, NULL, false);    //minus
                }
                else if(c == '+'){
                    return saveToken(token, ADD, NULL, false);    // plus
                }
                else if(c == '='){
                    return saveToken(token, ASSIGNMENT_EQ, NULL, false);   //porovnanie alebo priradenie
                }
                else if(c == '\\'){
                    return saveToken(token, INT_DIV, NULL, false);   //celociselne delenie
                }
                /*else if(c == '.'){
                    return saveToken(token, DOT, NULL, false);    //bodka
                }
                */

                else if(c == '\''){
                    while (((c = fgetc(stdin)) != '\n') && (c != EOF));
                    if(c == EOF){
                        return saveToken(token, END_OF_FILE, NULL, false);
                    }
                    else if (c=='\n')
                        return saveToken(token, EOL, NULL, false);    //EOL
                    else{ 
                        state = 0;
                    }
                }
                else if(c == '/'){
                    state = S_DIV_COM;
                }
                else if(c == '!'){

                    c = fgetc(stdin);
                    if(c != '"'){
                        print_err(1);
                    }
                    else{
                        state = S_STR;
                    }

                }

                else if(c == '>'){
                    state = S_GR_EQ;    //väčší/väčší alebo rovný
                }
                else if(c == '<'){
                    state = S_LESS_EQ;  //menší/menší alebo rovný
                }
                
                /*else if(c == '"'){
                    state = S_STR;  //začiatok reťazca
                }*/

                else if(c == EOF){
                    return  saveToken(token, END_OF_FILE, NULL, false);   //koniec suboru
                }
                else{
                    print_err(1);  //LEX_ERR
                    return NULL;
                }
            break;


            //jedna sa o retazec
            case S_STR:
                    //ked sa znak nerovna uvodzovkam, backslashu ani newline
                    if(((c!='"') && (escapovanie == 0) && (c!='\x0A') && (c!='\x5C'))){

                        t_str->string[position++] = c;
                        state = S_STR;
                    }
                    //backslash znamená, že je zapnute escapovanie
                    else if(((c=='\x5C') && (escapovanie == 0))){

                        escapovanie = 1;
                        state = S_STR;
                    }
                    //ak je zapnute escapovanie, zapiseme do retazca backslash
                    else if (((c == '\x5C') && (escapovanie == 1))){    //odescapeovaný backslash

                        t_str->string[position++] = c;
                        escapovanie = 0;
                        state = S_STR;

                    }
                    //ak je zapnute escapovanie, zapíšeme do reťazca úvodzovky
                    else if(((c == '\x22') && (escapovanie == 1))){ //odescapeované úvodzovky  '\"'

                        t_str->string[position++] = c;
                        escapovanie = 0;
                        state = S_STR;

                    }
                    //ak je zapnute escapovanie, zapíšeme do reťazca new line
                    else if (c == 'n' && escapovanie == 1){     //odescapeovaný

                        c = '\x0A';
                        t_str->string[position++] = c;
                        escapovanie = 0;
                        state = S_STR;
                    }
                    //ak je zapnute escapovanie, zapíšeme do reťazca tabulátor
                    else if(((c == 't') && (escapovanie == 1))){

                        c = '\x09';
                        t_str->string[position++] = c;
                        escapovanie = 0;
                        state = S_STR;

                    }
                    //ak je zapnuté escapovanie, znak môže byť zadaný pomocou oktalovej escape sekvencii, to riešime v stave S_STRING_NUMBERS
                    else if (((isdigit(c)) && (escapovanie == 1))){

                        hexa[0] = c;
                        //printf("------ %c\n", hexa[0]);
                        pocet_cisel = 1;
                        escapovanie = 0;
                        state = S_STRING_NUMBERS;

                    }
                    //reťezec musí byť zapísaný na jednom riadku v programe
                    else if (((c == '\n') && (escapovanie == 0))){

                        print_err(1);
                        return NULL;
                    }

                    //ak prídu uvodzvky a niesu odescapeovane, znaci to koniec retazca
                    else if (((c == '"') && (escapovanie == 0))){ // sme na konci retezce

                        t_str->string[position] = '\0';
                        return saveToken(token, TEXT, t_str, true);
                    }
                    //všetko iné považujeme za lexikálnu chybu
                    else{

                        print_err(1);
                        return NULL;

                    }

                    break;

            //oktalova escape sekvencia
            case S_STRING_NUMBERS:

                //ak sa jedná o druhé číslo, zapíšeme ho na druhú pozíciu
                if(((isdigit(c)) && (pocet_cisel == 1))){
                    hexa[1] = c;
                    //printf("------ %c\n", hexa[1]);
                    pocet_cisel = 2;
                    state = S_STRING_NUMBERS;
                  }
                 //ak sa jedná o tretie číslo, zapíšeme ho na tretiu pozíciu
                else if(((isdigit(c)) && (pocet_cisel == 2))){
                    hexa[2] = c;
                    //printf("------ %c\n", hexa[2]);
                    pocet_cisel = 3;
                    int number = atoi(hexa);
                    //printf("______ %s\n", hexa);
                    //printf("_-_-_- %d\n", number);
                    //char a;
                    /*a = hexa[0] - '0';
                    a = a + hexa[1] - '0';
                    a = a + hexa[2] - '0';*/
                    //printf("aaaaaaaaaaaaa %d\n", number);

                    //číslo musí byť z intervalu <0,255>, ak nie je, tak to značí lexikálnu chybu
                    if(((number < 001) || (number > 255))){
                        print_err(1);
                        return NULL;
                    }

                    else{
                        t_str->string[position++] = number;
                        pocet_cisel = 0;
                        state = S_STR;
                    }
                }

                else{
                    print_err(1);
                    return NULL;
                }

            break;

            //not equal or lex error
            case S_NOT_EQ:
                if(c == '='){
                    return  saveToken(token, NEQ, NULL, false);   // !=
                }
                else{
                    print_err(1);  //lexikálna chyba
                    return NULL;
                }
            break;

            //greater, equal
            case S_GR_EQ:
                if(c == '='){
                    return  saveToken(token, GREATER_EQ, NULL, false);    // >=
                }
                else{
                    ungetc(c, stdin);
                    return  saveToken(token, GREATER, NULL, false);   //>
                }
            break;

            //less, equal
            case S_LESS_EQ:
                if(c == '='){
                    return  saveToken(token, LESS_EQ, NULL, false);   //<=
                }
                else if (c == '>')
                {
                    return saveToken(token, NEQ, NULL, false);     //<>
                }
                else{
                    ungetc(c, stdin);
                    return  saveToken(token, LESS, NULL, false);  //<
                }
            break;

           case S_DIV_COM:
                //jedná sa o blokový komentár
                if(c == '\''){
                    //načítavame do konca súboru
                    while((c  = fgetc(stdin)) != EOF){
                        //keď príde / vyskakujeme z blokového komentára a ideme na počiatočný stav
                        if(c == '\''){
                            c = fgetc(stdin);
                            if(c == '/'){
                                state = S_START;
                                break;
                            }
                        }
                    }
                    //koemntár do konca súboru
                    if(c == EOF){
                        return saveToken(token, END_OF_FILE, NULL, false);
                    }
                    state = S_START;
                }
                else if(c == EOF){
                    return saveToken(token, DIV, NULL, false);    //delenie, ako posledný znak, očakáva sa syntaktická chyba
                }
                else{
                    ungetc(c, stdin);
                    return saveToken(token, DIV, NULL, false);    //delenie
                }
            break;

            case S_NUMBER:
                if(isdigit(c)){
                    t_str->string[position++] = c;
                }
                //ak pride exponent
                else if((c == 'e') || (c == 'E')){
                    if( exponent == true){
                        print_err(1);  //2 exponenty v číslu vedú k lexikálnej chybe
                        return NULL;
                    }

                    exponent = true;
                    t_str->string[position++] = c;
                    c = fgetc(stdin);

                    if(isdigit(c)){
                        t_str->string[position++] = c;
                    }

                    // + alebo -
                    else if((c == '+') || (c == '-')){
                        t_str->string[position++] = c;
                        c = fgetc(stdin);

                        //za + alebo - musi nasledovat cislo, inak lexikálna chyba
                        if(isdigit(c)){
                            t_str->string[position++] = c;
                        }
                        else{
                            print_err(1);
                            return NULL;
                        }
                    }

                    else{
                        print_err(1);
                        return NULL;
                    }
                }

                //desatinné číslo
                else if(c == '.'){
                    if( dot == true){
                        print_err(1);  //číslo nemôže obsahovať 2 desatinné čiarky
                        return NULL;
                    }

                    dot = true;
                    t_str->string[position++] = c;
                    c = fgetc(stdin);

                    if(isdigit(c)){
                        t_str->string[position++] = c;
                    }

                    else{
                        print_err(1);
                        return NULL;
                    }
                }

                //znaky, ktoré nemôže číslo obsahovať
                else if ((c == '_') ||isalpha(c)){
                    print_err(1);
                    return NULL;
                }

                else{
                    ungetc(c, stdin);
                    t_str->string[position] = '\0';
                    bool integer = true;
                    for(int i = 0; i < position; i++){
                        if(!(isdigit(t_str->string[i]))){
                            integer = false;
                        }
                    }

                    if(integer){
                        return saveToken(token, NUMBER, t_str, true);   //číslo typu integer
                    }

                    else{
                        return saveToken(token, DOUBL, t_str, true);    //číslo typu double
                    }
                }

            break;

            case S_IDENTIFIER:
                if(isalpha(c) || (c == '_') || isdigit(c)){
                    t_str->string[position++] = c;
                }

                else{
                    ungetc(c, stdin);
                    t_str->string[position] = '\0';

                    //porovnávame, či sa nejedná o kľúčové slová
                    if ((strcmp(t_str->string, "as")) == 0){
                        return saveToken(token, AS, NULL, false);
                    }
                    else if ((strcmp(t_str->string, "asc")) == 0){
                        return saveToken(token, ASC, NULL, false);
                    }
                    else if ((strcmp(t_str->string, "declare")) == 0){
                        return saveToken(token, DECLARE, NULL, false);
                    }
                    else if ((strcmp(t_str->string, "dim")) == 0){
                        return saveToken(token, DIM, NULL, false);
                    }
                    else if ((strcmp(t_str->string, "do")) == 0){
                        return saveToken(token, DO, NULL, false);
                    }
                    else if ((strcmp(t_str->string, "double")) == 0){
                        return saveToken(token, DOUBLE, NULL, false);
                    }
                    else if ((strcmp(t_str->string, "else")) == 0){
                        return saveToken(token, ELSE, NULL, false);
                    }
                    else if ((strcmp(t_str->string, "end")) == 0){
                        return saveToken(token, END, NULL, false);
                    }
                    else if ((strcmp(t_str->string, "chr")) == 0){
                        return saveToken(token, CHR, NULL, false);
                    }
                    else if ((strcmp(t_str->string, "function")) == 0){
                        return saveToken(token, T_FUNCTION, NULL, false);
                    }
                    else if ((strcmp(t_str->string, "if")) == 0){
                        return saveToken(token, IF, NULL, false);
                    }
                    else if ((strcmp(t_str->string, "input")) == 0){
                        return saveToken(token, INPUT, NULL, false);
                    }
                    else if ((strcmp(t_str->string, "integer")) == 0){
                        return saveToken(token, INTEGER, NULL, false);
                    }
                    else if ((strcmp(t_str->string, "length")) == 0){
                        return saveToken(token, LENGTH, NULL, false);
                    }
                    else if ((strcmp(t_str->string, "loop")) == 0){
                        return saveToken(token, LOOP, NULL, false);
                    }
                    else if ((strcmp(t_str->string, "print")) == 0){
                        return saveToken(token, PRINT, NULL, false);
                    }
                    else if ((strcmp(t_str->string, "return")) == 0){
                        return saveToken(token, RETURN, NULL, false);
                    }
                    else if ((strcmp(t_str->string, "scope")) == 0){
                        return saveToken(token, SCOPE, NULL, false);
                    }
                    else if ((strcmp(t_str->string, "string")) == 0){
                        return saveToken(token, STRING, NULL, false);
                    }
                    else if ((strcmp(t_str->string, "substr")) == 0){
                        return saveToken(token, SUBSTR, NULL, false);
                    }
                    else if ((strcmp(t_str->string, "then")) == 0){
                        return saveToken(token, THEN, NULL, false);
                    }
                    else if ((strcmp(t_str->string, "while")) == 0){
                        return saveToken(token, WHILE, NULL, false);
                    }

                    else if ((strcmp(t_str->string, "and")) == 0){
                        return saveToken(token, AND, NULL, false);
                    }
                    else if ((strcmp(t_str->string, "boolean")) == 0){
                        return saveToken(token, BOOLEAN, NULL, false);
                    }
                    else if ((strcmp(t_str->string, "continue")) == 0){
                        return saveToken(token, CONTINUE, NULL, false);
                    }
                    else if ((strcmp(t_str->string, "elseif")) == 0){
                        return saveToken(token, ELSEIF, NULL, false);
                    }
                    else if ((strcmp(t_str->string, "exit")) == 0){
                        return saveToken(token, EXIT, NULL, false);
                    }
                    else if ((strcmp(t_str->string, "false")) == 0){
                        return saveToken(token, T_FALSE, NULL, false);
                    }
                    else if ((strcmp(t_str->string, "for")) == 0){
                        return saveToken(token, FOR, NULL, false);
                    }
                    else if ((strcmp(t_str->string, "next")) == 0){
                        return saveToken(token, NEXT, NULL, false);
                    }
                    else if ((strcmp(t_str->string, "not")) == 0){
                        return saveToken(token, NOT, NULL, false);
                    }
                    else if ((strcmp(t_str->string, "or")) == 0){
                        return saveToken(token, OR, NULL, false);
                    }
                    else if ((strcmp(t_str->string, "shared")) == 0){
                        return saveToken(token, SHARED, NULL, false);
                    }
                    else if ((strcmp(t_str->string, "static")) == 0){
                        return saveToken(token, STATIC, NULL, false);
                    }
                    else if ((strcmp(t_str->string, "true")) == 0){
                        return saveToken(token, T_TRUE, NULL, false);
                    }
                    else{
                        return saveToken(token, ID, t_str, true);
                    }
                }
            break;

        }
    }
    return saveToken(token, END_OF_FILE, NULL, false);
}

int main(){
    //FILE *fp = fopen("S.TXT","r");
    
    T_string * t_str = strInit();
    T_Token * new_t  = initToken();

    int i=0;
    for(i=0; i<80; i++){
        getToken(new_t, t_str);
        printf("%d\t", new_t->type);
        printf("%s\n", new_t->str->string);
    }

    free(t_str->string);
    free(t_str);
    free(new_t->str->string);
    free(new_t->str);
    free(new_t);
    
    /*for(int i = 0; i < 5; i++){
        T_Token *tok = getToken();
        printf("%d\t", tok->type);
        printf("%s\n", tok->str);
    }*/

}