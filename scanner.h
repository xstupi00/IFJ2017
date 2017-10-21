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
#define T_TRUE				34

#define NUMBER				35	// 4
#define DOUBL 				36	// 3,14
#define TEXT 				37	// abc

#define ADD					38	// +
#define SUB 				39	// -
#define MUL					40	// *

#define DIV					41	// /
#define ASSIGNMENT_EQ		42	// =

#define NEQ 				43	// <>
#define GREATER				44	// >
#define GREATER_EQ	 		45	// >=
#define LESS 				46	// <
#define LESS_EQ 			47	// <=
#define DOT 				48	// .
#define BACKSLASH			49	// /
#define END_OF_FILE			50	// EOF
#define COMMA				51	// ,
//#define LEFT_C_BRACKET  	52  // {
//#define RIGHT_C_BRACKET 	53  // }
#define LEFT_R_BRACKET  	54  // (
#define RIGHT_R_BRACKET 	55 	// )
#define SEMICOLON 			56	// ;  
#define ID 					57	// int x;
#define EOL 				58  // EOL
#define INT_DIV				59  // celociselne delenie 

/*typedef enum{
	LEX_OK,
	LEX_ERR,
	ALLOC_ERR
}T_Err;
*/
typedef struct T_Token{
	T_string * str;
	int type;
}T_Token;

#define VARIABLE 	1
#define FUNCTION	2
#define FUN_CALL	3

T_Token *getToken(T_Token * token, T_string * t_str);
T_Token *saveToken(T_Token * token, int type, T_string * string, bool full);

#endif 