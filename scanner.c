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
#include "errorprint.c"

#define STR_INIT 100

//funkcia na uloženie načítaného tokena do tokenovej štruktúry
T_Token *SaveToken(int type, char *string){
    T_Token *token = malloc(sizeof(struct T_Token));    //alokacia štruktúry tokenu
    if(token == NULL){
        errorPrint(99);     //nepodarila sa alokácia
        return NULL;
    }

    token->type = type; //priradí typ

    if(string != NULL){
        int length = strlen(string);
        token->str = malloc(sizeof(char)*(length + 1)); //alokácia stringu na uloženie hodnoty tokenu

        if(token->str == NULL){
        errorPrint(99); //nepodarila sa alokácia
        return NULL;
        }

        strcpy(token->str, string); //priradí hodnotu tokenu do retazca
        free(string);
    }

    return token;
}

T_Token *GetToken(FILE *source){

    int str_init = STR_INIT;    //velkost alokovania retazca
    int position = 0;           //index aktualneho charu
    char c;
    char hexa[4];
    hexa[3] = '\0';
    int escapovanie = 0;
    int pocet_cisel = 0;
    bool exponent = false;
    bool dot = false;
    T_States state = S_START;   //stav automatu

    char *string = calloc(1, sizeof(char) * str_init);
    if(string == NULL){
        errorPrint(99);     //nepodarila sa alokácia
        return NULL;
    }

    while((c = fgetc(source)) != EOF +1){
        if(string[STR_INIT-1] == '\0'){
            str_init += str_init;
            string = realloc(string, sizeof(char) * str_init);
            if(string == NULL){
                errorPrint(99);     //nepodarila sa realokácia
                return NULL;
            }
        }

        switch(state){
            case S_START:
                if(isspace(c)){
                    state = S_START;    //biely znak == pokračuj ďalej
                }
                else if(isalpha(c) || c == '_'){
                    state = S_IDENTIFIER;   //identifikátor, kľúčové slovo
                    string[position++] = c;
                }
                else if(isdigit(c)){
                    state = S_NUMBER;   //číslo
                    string[position++] = c;
                }
                /*else if(c == '{'){
                    return SaveToken(LEFT_C_BRACKET, NULL); //ľavá zložená zátvorka
                }
                else if(c == '}'){
                    return SaveToken(RIGHT_C_BRACKET, NULL);    //pravá zložená zátvorka
                }
                */
                else if(c == '('){
                    return SaveToken(LEFT_R_BRACKET, NULL);     //ľavá okrúhla zátvorka
                }
                else if(c == ')'){
                    return SaveToken(RIGHT_R_BRACKET, NULL);    //pravá okrúhla zátvorka
                }
                /*else if(c == ','){
                    return SaveToken(COMMA, NULL);  //čiarka
                }
                */
                else if(c == ';'){
                    return SaveToken(SEMICOLON, NULL);  //bodkočiarka
                }
                else if(c == '*'){
                    return SaveToken(MUL, NULL);    //krát
                }
                else if(c == '-'){
                    return SaveToken(SUB, NULL);    //minus
                }
                else if(c == '+'){
                    return SaveToken(ADD, NULL);    // plus
                }
                else if(c == '='){
                    return SaveToken(ASSIGNMENT_EQ, NULL);   //porovnanie alebo priradenie
                }

                /*else if(c == '.'){
                    return SaveToken(DOT, NULL);    //bodka
                }
                */

                else if(c == '\''){
                    while (((c = fgetc(source)) != '\n') && (c != EOF));
                    if(c == EOF){
                        return SaveToken(END_OF_FILE, NULL);
                    }
                    else{
                        state = 0;
                    }
                }
                else if(c == '/'){
                    state = S_DIV_COM;
                }
                else if(c == '!'){

                    c = fgetc(source);
                    if(c != '"'){
                    	errorPrint(1);
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
                    return  SaveToken(END_OF_FILE, NULL);   //koniec suboru
                }
                else{
                    errorPrint(1);  //LEX_ERR
                    return NULL;
                }
            break;


           	//jedna sa o retazec
           	case S_STR:
					//ked sa znak nerovna uvodzovkam, backslashu ani newline
					if(((c!='"') && (escapovanie == 0) && (c!='\x0A') && (c!='\x5C'))){

			        	string[position++] = c;
			        	state = S_STR;
			        }
			        //backslash znamená, že je zapnute escapovanie
			        else if(((c=='\x5C') && (escapovanie == 0))){

			        	escapovanie = 1;
			        	state = S_STR;
					}
					//ak je zapnute escapovanie, zapiseme do retazca backslash
					else if (((c == '\x5C') && (escapovanie == 1))){	//odescapeovaný backslash

						string[position++] = c;
						escapovanie = 0;
						state = S_STR;

			        }
			        //ak je zapnute escapovanie, zapíšeme do reťazca úvodzovky
			        else if(((c == '\x22') && (escapovanie == 1))){	//odescapeované úvodzovky  '\"'

			        	string[position++] = c;
						escapovanie = 0;
						state = S_STR;

			        }
			        //ak je zapnute escapovanie, zapíšeme do reťazca new line
			        else if (c == 'n' && escapovanie == 1){		//odescapeovaný

						c = '\x0A';
						string[position++] = c;
						escapovanie = 0;
						state = S_STR;
					}
					//ak je zapnute escapovanie, zapíšeme do reťazca tabulátor
			        else if(((c == 't') && (escapovanie == 1))){

						c = '\x09';
						string[position++] = c;
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

			         	errorPrint(1);
						return NULL;
			        }

			        //ak prídu uvodzvky a niesu odescapeovane, znaci to koniec retazca
			        else if (((c == '"') && (escapovanie == 0))){ // sme na konci retezce

			        	string[position] = '\0';
						return SaveToken(TEXT, string);
			        }
			        //všetko iné považujeme za lexikálnu chybu
			        else{

			        	errorPrint(1);
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
				    	errorPrint(1);
						return NULL;
				    }

				    else{
						string[position++] = number;
						pocet_cisel = 0;
						state = S_STR;
					}
				}

				else{
					errorPrint(1);
					return NULL;
				}

			break;

            //not equal or lex error
            case S_NOT_EQ:
                if(c == '='){
                    return  SaveToken(NEQ, NULL);   // !=
                }
                else{
                    errorPrint(1);  //lexikálna chyba
                    return NULL;
                }
            break;

            //greater, equal
            case S_GR_EQ:
                if(c == '='){
                    return  SaveToken(GREATER_EQ, NULL);    // >=
                }
                else{
                    ungetc(c, source);
                    return  SaveToken(GREATER, NULL);   //>
                }
            break;

            //less, equal
            case S_LESS_EQ:
                if(c == '='){
                    return  SaveToken(LESS_EQ, NULL);   //<=
                }
                else if (c == '>')
                {
                    return SaveToken(NEQ, NULL);     //<>
                }
                else{
                    ungetc(c, source);
                    return  SaveToken(LESS, NULL);  //<
                }
            break;

            case S_DIV_COM:
                //jedná sa o blokový komentár
                if(c == '/'){
                    //načítavame do konca súboru
                    while((c  = fgetc(source)) != EOF){
                        //keď príde / vyskakujeme z blokového komentára a ideme na počiatočný stav
                        if(c == '/'){
                            c = fgetc(source);
                            if(c == '/'){
                                state = S_START;
                                break;
                            }
                        }
                    }
                    //koemntár do konca súboru
                    if(c == EOF){
                        return SaveToken(END_OF_FILE, NULL);
                    }
                    state = S_START;
                }
                else if(c == EOF){
                    return SaveToken(DIV, NULL);    //delenie, ako posledný znak, očakáva sa syntaktická chyba
                }
                else{
                    ungetc(c, source);
                    return SaveToken(DIV, NULL);    //delenie
                }
            break;

            case S_NUMBER:
                if(isdigit(c)){
                    string[position++] = c;
                }
                //ak pride exponent
                else if((c == 'e') || (c == 'E')){
                    if( exponent == true){
                        errorPrint(1);  //2 exponenty v číslu vedú k lexikálnej chybe
                        return NULL;
                    }

                    exponent = true;
                    string[position++] = c;
                    c = fgetc(source);

                    if(isdigit(c)){
                        string[position++] = c;
                    }

                    // + alebo -
                    else if((c == '+') || (c == '-')){
                        string[position++] = c;
                        c = fgetc(source);

                        //za + alebo - musi nasledovat cislo, inak lexikálna chyba
                        if(isdigit(c)){
                            string[position++] = c;
                        }
                        else{
                            errorPrint(1);
                            return NULL;
                        }
                    }

                    else{
                        errorPrint(1);
                        return NULL;
                    }
                }

                //desatinné číslo
                else if(c == '.'){
                    if( dot == true){
                        errorPrint(1);  //číslo nemôže obsahovať 2 desatinné čiarky
                        return NULL;
                    }

                    dot = true;
                    string[position++] = c;
                    c = fgetc(source);

                    if(isdigit(c)){
                        string[position++] = c;
                    }

                    else{
                        errorPrint(1);
                        return NULL;
                    }
                }

                //znaky, ktoré nemôže číslo obsahovať
                else if ((c == '_') ||isalpha(c)){
                    errorPrint(1);
                    return NULL;
                }

                else{
                    ungetc(c, source);
                    string[position] = '\0';
                    bool integer = true;
                    for(int i = 0; i < position; i++){
                        if(!(isdigit(string[i]))){
                            integer = false;
                        }
                    }

                    if(integer){
                        return SaveToken(NUMBER, string);   //číslo typu integer
                    }

                    else{
                        return SaveToken(DOUBL, string);    //číslo typu double
                    }
                }

            break;

            case S_IDENTIFIER:
                if(isalpha(c) || (c == '_') || isdigit(c)){
                    string[position++] = c;
                }

                else{
                    ungetc(c, source);
                    string[position] = '\0';

                    //porovnávame, či sa nejedná o kľúčové slová
                    if ((strcmp(string, "as")) == 0){
                        return SaveToken(AS, NULL);
                    }
                    else if ((strcmp(string, "asc")) == 0){
                        return SaveToken(ASC, NULL);
                    }
                    else if ((strcmp(string, "declare")) == 0){
                        return SaveToken(DECLARE, NULL);
                    }
                    else if ((strcmp(string, "dim")) == 0){
                        return SaveToken(DIM, NULL);
                    }
                    else if ((strcmp(string, "do")) == 0){
                        return SaveToken(DO, NULL);
                    }
                    else if ((strcmp(string, "double")) == 0){
                        return SaveToken(DOUBLE, NULL);
                    }
                    else if ((strcmp(string, "else")) == 0){
                        return SaveToken(ELSE, NULL);
                    }
                    else if ((strcmp(string, "end")) == 0){
                        return SaveToken(END, NULL);
                    }
                    else if ((strcmp(string, "chr")) == 0){
                        return SaveToken(CHR, NULL);
                    }
                    else if ((strcmp(string, "function")) == 0){
                        return SaveToken(T_FUNCTION, NULL);
                    }
                    else if ((strcmp(string, "if")) == 0){
                        return SaveToken(IF, NULL);
                    }
                    else if ((strcmp(string, "input")) == 0){
                        return SaveToken(INPUT, NULL);
                    }
                    else if ((strcmp(string, "integer")) == 0){
                        return SaveToken(INTEGER, NULL);
                    }
                    else if ((strcmp(string, "length")) == 0){
                        return SaveToken(LENGTH, NULL);
                    }
                    else if ((strcmp(string, "loop")) == 0){
                        return SaveToken(LOOP, NULL);
                    }
                    else if ((strcmp(string, "print")) == 0){
                        return SaveToken(PRINT, NULL);
                    }
                    else if ((strcmp(string, "return")) == 0){
                        return SaveToken(RETURN, NULL);
                    }
                    else if ((strcmp(string, "scope")) == 0){
                        return SaveToken(SCOPE, NULL);
                    }
                    else if ((strcmp(string, "string")) == 0){
                        return SaveToken(STRING, NULL);
                    }
                    else if ((strcmp(string, "substr")) == 0){
                        return SaveToken(SUBSTR, NULL);
                    }
                    else if ((strcmp(string, "then")) == 0){
                        return SaveToken(THEN, NULL);
                    }
                    else if ((strcmp(string, "while")) == 0){
                        return SaveToken(WHILE, NULL);
                    }

                    else if ((strcmp(string, "and")) == 0){
                        return SaveToken(AND, NULL);
                    }
                    else if ((strcmp(string, "boolean")) == 0){
                        return SaveToken(BOOLEAN, NULL);
                    }
                    else if ((strcmp(string, "continue")) == 0){
                        return SaveToken(CONTINUE, NULL);
                    }
                    else if ((strcmp(string, "elseif")) == 0){
                        return SaveToken(ELSEIF, NULL);
                    }
                    else if ((strcmp(string, "exit")) == 0){
                        return SaveToken(EXIT, NULL);
                    }
                    else if ((strcmp(string, "false")) == 0){
                        return SaveToken(T_FALSE, NULL);
                    }
                    else if ((strcmp(string, "for")) == 0){
                        return SaveToken(FOR, NULL);
                    }
                    else if ((strcmp(string, "next")) == 0){
                        return SaveToken(NEXT, NULL);
                    }
                    else if ((strcmp(string, "not")) == 0){
                        return SaveToken(NOT, NULL);
                    }
                    else if ((strcmp(string, "or")) == 0){
                        return SaveToken(OR, NULL);
                    }
                    else if ((strcmp(string, "shared")) == 0){
                        return SaveToken(SHARED, NULL);
                    }
                    else if ((strcmp(string, "static")) == 0){
                        return SaveToken(STATIC, NULL);
                    }
                    else if ((strcmp(string, "true")) == 0){
                        return SaveToken(T_TRUE, NULL);
                    }
                    else{
                        return SaveToken(ID, string);
                    }
                }
            break;

        }
    }
    return SaveToken(END_OF_FILE, NULL);
}

int main(){
    FILE *fp = fopen("S.TXT","r");
    for(int i = 0; i < 5; i++){
        T_Token *tok = GetToken(fp);
        printf("%d\n", tok->type);
        printf("%s\n", tok->str);
       // printf("%s\n", tok->str);
    }
}
