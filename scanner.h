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
 
typedef enum {

	AS,
	DECLARE,
	DIM,
	DO,
	DOUBLE,
	ELSE,
	END,
	T_FUNCTION,
	IF,
	INPUT,	
	INTEGER,
	LENGTH,
	SUBSTR,
	ASC,
	CHR,
	LOOP,
	PRINT,
	RETURN,
	SCOPE,
	STRING,	
	THEN,
	WHILE,
	AND,
	BOOLEAN,
	CONTINUE,
	ELSEIF,
	EXIT,
	T_FALSE,
	FOR,
	NEXT,
	NOT,
	OR,
	SHARED,	
	STATIC,
	ADD,
	SUB,
	MUL,
	DIV,
	INT_DIV = 40,
	LESS,
	GREATER,
	LESS_EQ,
	GREATER_EQ,
	ASSIGNMENT_EQ,
	NEQ,
	ID,
	INT_NUMBER,
	DOUBLE_NUMBER,
	TEXT,
	BACKSLASH,
	END_OF_FILE,
	COMMA,
	LEFT_R_BRACKET,
	RIGHT_R_BRACKET,
	SEMICOLON,
	T_TRUE,
	EOL,
	DOT

} T_Tokens;

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