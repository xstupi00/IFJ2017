#ifndef SCANNER_H
#define SCANNER_H

#include "strlib.h"

//States of finite automaton
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

}states_t;

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

} tokens_t;

//structure of token
typedef struct token_t{
	int type;		//identification of token
	string_t * str;	//attribute of token
}token_t;

/**
 * @brief      Function to get previous token
 */
void ungetToken();

/**
 * @brief      Function to initialize structure of token
 */
void initToken();

/**
 * @brief      Function to get next token
 */
token_t *getToken();

/**
 * @brief      Function to save token
 *
 * @param      type 	identification of token
 * @param      string 	attribute of token
 */
token_t *saveToken(int type, bool string);

#endif 
