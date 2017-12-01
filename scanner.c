///////////////////////////////////////////////////////////////////////////////////
// School:      Brno University of Technology, Faculty of Information Technology //
// Course:      Formal Languages and Compilers                                   //
// Project:     IFJ17                                                            //
// Module:      Lexical analysis                                                 //
// Authors:     Kristián Liščinský  (xlisci01)                                   //
//              Matúš Liščinský     (xlisci02)                                   //
//              Šimon Stupinský     (xstupi00)                                   //
//              Vladimír Marcin     (xmarci10)                                   //
///////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include "scanner.h"
#include "error.h"
#include "strlib.h"
#include "clear.h"

#define malloc(size) _malloc(size)

token_t * token;
bool unget;

void initToken(){
    token = (token_t*) malloc(sizeof(struct token_t));
    if(!token){
        print_err(99);
    }       
    token->str = strInit(STR_INIT); // alokacia pamate pre string
    
}

void ungetToken(){
    unget=true;
}
//funkcia na uloženie načítaného tokena do tokenovej štruktúry
token_t *saveToken(int type, bool string){
    if(!token){
        print_err(99);
        return NULL;
    }   
    if(!string)
        token->str->string[0]='\0'; 
    token->type = type; //priradí typ
    token->str->length = strlen(token->str->string);
    //printf("token : %s %d\n",token->str->string,token->type);
    return token;
}

token_t *getToken(){

    if(unget){
        unget=false;
        return token;
    }
    
    int position = 0;           //index aktualneho charu
    //char c; cppcheck
    char hexa[4];
    hexa[3] = '\0';
    int escapovanie = 0;
    int pocet_cisel = 0;
    bool exponent = false;
    bool dot = false;
    states_t state = S_START;   //stav automatu

    while(/*((c = fgetc(stdin)) != EOF)+*/1){
        char c = fgetc(stdin);
        token->str->length=strlen(token->str->string);
  
        if(token->str->length+1 > token->str->capacity){
            extendStr(token->str, 2*token->str->capacity);
        }

        if(state != S_STR)
            c = tolower(c);
       
        switch(state){
            case S_START:
                if(c == '\n'){
                    return saveToken(EOL, false);    //EOL
                }
                else if(isspace(c)){
                    state = S_START;    //biely znak == pokračuj ďalej
                }
                else if(isalpha(c) || c == '_'){
                    state = S_IDENTIFIER;   //identifikátor, kľúčové slovo
                    token->str->string[position++] = c;
                }
                else if(isdigit(c)){
                    state = S_NUMBER;   //číslo
                    token->str->string[position++] = c;
                }
                else if(c == '('){
                    return saveToken(LEFT_R_BRACKET, false);     //ľavá okrúhla zátvorka
                }
                else if(c == ')'){
                    return saveToken(RIGHT_R_BRACKET, false);    //pravá okrúhla zátvorka
                }
                else if(c == ','){
                    return saveToken(COMMA, false);  //čiarka
                }
                
                else if(c == ';'){
                    return saveToken(SEMICOLON, false);  //bodkočiarka
                }
                else if(c == '*'){
                    return saveToken(MUL, false);    //krát
                }
                else if(c == '-'){
                    return saveToken(SUB, false);    //minus
                }
                else if(c == '+'){
                    return saveToken(ADD, false);    // plus
                }
                else if(c == '='){
                    return saveToken(ASSIGNMENT_EQ, false);   //porovnanie alebo priradenie
                }
                else if(c == '\\'){
                    return saveToken(INT_DIV, false);   //celociselne delenie
                }

                else if(c == '\''){
                    while ( ((c = fgetc(stdin)) != '\n') && (c != -1) );
                    if(c == -1){
                        return saveToken(END_OF_FILE, false);
                    }
                    else if (c=='\n')
                        return saveToken(EOL, false);    //EOL
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
                else if(c == -1){
                    return  saveToken(END_OF_FILE, false);   //koniec suboru
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

			        	token->str->string[position++] = c;
			        	state = S_STR;
			        }
			        //backslash znamená, že je zapnute escapovanie
			        else if(((c=='\x5C') && (escapovanie == 0))){

			        	escapovanie = 1;
			        	state = S_STR;
					}
					//ak je zapnute escapovanie, zapiseme do retazca backslash
					else if (((c == '\x5C') && (escapovanie == 1))){	//odescapeovaný backslash

						token->str->string[position++] = c;
						escapovanie = 0;
						state = S_STR;

			        }
			        //ak je zapnute escapovanie, zapíšeme do reťazca úvodzovky
			        else if(((c == '\x22') && (escapovanie == 1))){	//odescapeované úvodzovky  '\"'

			        	token->str->string[position++] = c;
						escapovanie = 0;
						state = S_STR;

			        }
			        //ak je zapnute escapovanie, zapíšeme do reťazca new line
			        else if (c == 'n' && escapovanie == 1){		//odescapeovaný

						c = '\x0A';
						token->str->string[position++] = c;
						escapovanie = 0;
						state = S_STR;
					}
					//ak je zapnute escapovanie, zapíšeme do reťazca tabulátor
			        else if(((c == 't') && (escapovanie == 1))){

						c = '\x09';
						token->str->string[position++] = c;
						escapovanie = 0;
						state = S_STR;

			        }
			        //ak je zapnuté escapovanie, znak môže byť zadaný pomocou oktalovej escape sekvencii, to riešime v stave S_STRING_NUMBERS
			        else if (((isdigit(c)) && (escapovanie == 1))){

						hexa[0] = c;
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

			        	token->str->string[position] = '\0';
						return saveToken(TEXT, true);
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
				 	pocet_cisel = 2;
					state = S_STRING_NUMBERS;
			      }
			     //ak sa jedná o tretie číslo, zapíšeme ho na tretiu pozíciu
			    else if(((isdigit(c)) && (pocet_cisel == 2))){
					hexa[2] = c;
			    	//pocet_cisel = 3;
					int number = atoi(hexa);
                    //printf("ASCII: %d\n", number);
					//číslo musí byť z intervalu <0,255>, ak nie je, tak to značí lexikálnu chybu
				    if(((number < 001) || (number > 255))){
				    	print_err(1);
						return NULL;
				    }

				    else{
						token->str->string[position++] = number;
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
                    return  saveToken(NEQ, false);   // !=
                }
                else{
                    print_err(1);  //lexikálna chyba
                    return NULL;
                }
            break;

            //greater, equal
            case S_GR_EQ:
                if(c == '='){
                    return  saveToken(GREATER_EQ, false);    // >=
                }
                else{
                    ungetc(c, stdin);
                    return  saveToken(GREATER, false);   //>
                }
            break;

            //less, equal
            case S_LESS_EQ:
                if(c == '='){
                    return  saveToken(LESS_EQ, false);   //<=
                }
                else if (c == '>')
                {
                    return saveToken(NEQ, false);     //<>
                }
                else{
                    ungetc(c, stdin);
                    return  saveToken(LESS, false);  //<
                }
            break;

           case S_DIV_COM:
                //jedná sa o blokový komentár
                if(c == '\''){
                    //načítavame do konca súboru
                    while((c  = fgetc(stdin)) != -1){
                        //keď príde / vyskakujeme z blokového komentára a ideme na počiatočný stav
                        if(c == '\''){
                            c = fgetc(stdin);
                            if(c == '/'){
                                //state = S_START;
                                break;
                            }
                            else if (c == '\'')
                                ungetc(c, stdin);
                        }
                    }
                    //koemntár do konca súboru
                    if(c == -1){
                        //return saveToken(END_OF_FILE, false);
                        print_err(1);
                    }
                    state = S_START;
                }
                else if(c == -1){
                    return saveToken(DIV, false);    //delenie, ako posledný znak, očakáva sa syntaktická chyba
                }
                else{
                    ungetc(c, stdin);
                    return saveToken(DIV, false);    //delenie
                }
            break;

            case S_NUMBER:
                if(isdigit(c)){
                    token->str->string[position++] = c;
                }
                //ak pride exponent
                else if((c == 'e') || (c == 'E')){
                    if( exponent == true){
                        print_err(1);  //2 exponenty v číslu vedú k lexikálnej chybe
                        return NULL;
                    }

                    exponent = true;
                    token->str->string[position++] = c;
                    c = fgetc(stdin);

                    if(isdigit(c)){
                        token->str->string[position++] = c;
                    }

                    // + alebo -
                    else if((c == '+') || (c == '-')){
                        token->str->string[position++] = c;
                        c = fgetc(stdin);

                        //za + alebo - musi nasledovat cislo, inak lexikálna chyba
                        if(isdigit(c)){
                            token->str->string[position++] = c;
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
                    token->str->string[position++] = c;
                    c = fgetc(stdin);

                    if(isdigit(c)){
                        token->str->string[position++] = c;
                    }

                    else{
                        print_err(1);
                        return NULL;
                    }
                }

                //znaky, ktoré nemôže číslo obsahovať
                else if ((c == '_') || isalpha(c)){
                    print_err(1);
                    return NULL;
                }

                else{
                    ungetc(c, stdin);
                    token->str->string[position] = '\0';
                    bool integer = true;
                    for(int i = 0; i < position; i++){
                        if(!(isdigit(token->str->string[i]))){
                            integer = false;
                        }
                    }

                    if(integer){
                        return saveToken(INT_NUMBER, true);   //číslo typu integer
                    }

                    else{
                        return saveToken(DOUBLE_NUMBER, true);    //číslo typu double
                    }
                }

            break;

            case S_IDENTIFIER:
                if(isalpha(c) || (c == '_') || isdigit(c)){
                    token->str->string[position++] = c;
                }

                else{
                    ungetc(c, stdin);
                    token->str->string[position] = '\0';

                    //porovnávame, či sa nejedná o kľúčové slová
                    if ((strcmp(token->str->string, "as")) == 0){
                        return saveToken(AS, false);
                    }
                    else if ((strcmp(token->str->string, "asc")) == 0){
                        return saveToken(ASC, false);
                    }
                    else if ((strcmp(token->str->string, "declare")) == 0){
                        return saveToken(DECLARE, false);
                    }
                    else if ((strcmp(token->str->string, "dim")) == 0){
                        return saveToken(DIM, false);
                    }
                    else if ((strcmp(token->str->string, "do")) == 0){
                        return saveToken(DO, false);
                    }
                    else if ((strcmp(token->str->string, "double")) == 0){
                        return saveToken(DOUBLE, false);
                    }
                    else if ((strcmp(token->str->string, "else")) == 0){
                        return saveToken(ELSE, false);
                    }
                    else if ((strcmp(token->str->string, "end")) == 0){
                        return saveToken(END, false);
                    }
                    else if ((strcmp(token->str->string, "chr")) == 0){
                        return saveToken(CHR, false);
                    }
                    else if ((strcmp(token->str->string, "function")) == 0){
                        return saveToken(T_FUNCTION, false);
                    }
                    else if ((strcmp(token->str->string, "if")) == 0){
                        return saveToken(IF, false);
                    }
                    else if ((strcmp(token->str->string, "input")) == 0){
                        return saveToken(INPUT, false);
                    }
                    else if ((strcmp(token->str->string, "integer")) == 0){
                        return saveToken(INTEGER, false);
                    }
                    else if ((strcmp(token->str->string, "length")) == 0){
                        return saveToken(LENGTH, false);
                    }
                    else if ((strcmp(token->str->string, "loop")) == 0){
                        return saveToken(LOOP, false);
                    }
                    else if ((strcmp(token->str->string, "print")) == 0){
                        return saveToken(PRINT, false);
                    }
                    else if ((strcmp(token->str->string, "return")) == 0){
                        return saveToken(RETURN, false);
                    }
                    else if ((strcmp(token->str->string, "scope")) == 0){
                        return saveToken(SCOPE, false);
                    }
                    else if ((strcmp(token->str->string, "string")) == 0){
                        return saveToken(STRING, false);
                    }
                    else if ((strcmp(token->str->string, "substr")) == 0){
                        return saveToken(SUBSTR, false);
                    }
                    else if ((strcmp(token->str->string, "then")) == 0){
                        return saveToken(THEN, false);
                    }
                    else if ((strcmp(token->str->string, "while")) == 0){
                        return saveToken(WHILE, false);
                    }
                    else if ((strcmp(token->str->string, "and")) == 0){
                        return saveToken(AND, false);
                    }
                    else if ((strcmp(token->str->string, "boolean")) == 0){
                        return saveToken(BOOLEAN, false);
                    }
                    else if ((strcmp(token->str->string, "continue")) == 0){
                        return saveToken(CONTINUE, false);
                    }
                    else if ((strcmp(token->str->string, "elseif")) == 0){
                        return saveToken(ELSEIF, false);
                    }
                    else if ((strcmp(token->str->string, "exit")) == 0){
                        return saveToken(EXIT, false);
                    }
                    else if ((strcmp(token->str->string, "false")) == 0){
                        return saveToken(T_FALSE, false);
                    }
                    else if ((strcmp(token->str->string, "for")) == 0){
                        return saveToken(FOR, false);
                    }
                    else if ((strcmp(token->str->string, "next")) == 0){
                        return saveToken(NEXT, false);
                    }
                    else if ((strcmp(token->str->string, "not")) == 0){
                        return saveToken(NOT, false);
                    }
                    else if ((strcmp(token->str->string, "or")) == 0){
                        return saveToken(OR, false);
                    }
                    else if ((strcmp(token->str->string, "shared")) == 0){
                        return saveToken(SHARED, false);
                    }
                    else if ((strcmp(token->str->string, "static")) == 0){
                        return saveToken(STATIC, false);
                    }
                    else if ((strcmp(token->str->string, "true")) == 0){
                        return saveToken(T_TRUE, false);
                    }
                    else{
                        return saveToken(ID, true);
                    }
                }
            break;

        }
    if ( c == -1 )
        print_err(1);
    }
}