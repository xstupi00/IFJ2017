#ifndef SCANNER_H
#define SCANNER_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include "scanner.h"
#include "error.h"
#include "strlib.h"
#include "stack.h"


typedef enum{

	S_START,
	S_IDENTIFIER,
	S_NUMBER,
	S_STRING_NUMBERS,
	S_DIV_COM,
	S_NOT_EQ,
	S_GR_EQ,
	S_LESS_EQ,
	S_STR

}T_States;

#define AS					0
#define ASC					1
#define DECLARE				2
#define DIM					3
#define	DO 					4
#define DOUBLE 				5
#define ELSE 				6
#define END					7
#define CHR					8
#define T_FUNCTION			9
#define IF					10
#define INPUT 				11
#define INTEGER 			12
#define LENGTH				13
#define LOOP 				14
#define PRINT 				15
#define RETURN 				16
#define	SCOPE				17
#define	STRING 				18
#define SUBSTR				19
#define THEN				20
#define WHILE				21

#define	AND					22
#define	BOOLEAN				23
#define	CONTINUE			24
#define	ELSEIF				25
#define	EXIT				26
#define T_FALSE				27
#define	FOR					28
#define	NEXT				29
#define NOT 				30
#define OR 					31
#define	SHARED				32
#define	STATIC				33
#define ADD					34 // +

#define SUB 				35	// -
#define MUL 				36	// *
#define DIV 				37	// /

#define INT_DIV				40	// celociselne delenie
#define LESS 				41	// <
#define GREATER				42	// >

#define LESS_EQ				43	// <=
#define GREATER_EQ			44	// >=

#define ASSIGNMENT_EQ 		45	// =
#define NEQ					46	// <>
#define ID 	 				47	// int x;
#define NUMBER  			48	// int
#define DOUBL 	 			49	// double
#define TEXT 				50	// string
#define BACKSLASH			51	// /
#define END_OF_FILE			52	// EOF
#define COMMA				53	// ,
//#define LEFT_C_BRACKET  	54  // {
//#define RIGHT_C_BRACKET 	55  // }
#define LEFT_R_BRACKET  	56  // (
#define RIGHT_R_BRACKET 	57 	// )
#define SEMICOLON 			58	// ;  
#define T_TRUE 				59	// 
#define EOL 				60  // EOL
#define DOT					61  // dot 

/*typedef enum{
	LEX_OK,
	LEX_ERR,
	ALLOC_ERR
}T_Err;
*/
typedef struct T_Token{
	int type;
	T_string * str;
}T_Token;

#define VARIABLE 	1
#define FUNCTION	2
#define FUN_CALL	3

void ungetToken();
void initToken();
T_Token *getToken();
T_Token *saveToken(int type, bool string);

#endif 