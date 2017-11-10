#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "parser.h"
#include "scanner.h"
#include "error.h"

T_Token *token;

//Declaration of state function
bool PROG();
bool DECLARE_FUNCTION();
bool DEFINE_FUNCTION();
bool MAIN_FUNCTION();
bool FUNCTION_ELEMENT();
bool ELEMENT_LIST();
bool STATEMENT();
bool VALUE();
bool ELSE_STATEMENT();
bool STAT_LIST();
bool EXPRESSION();
bool CALL_ASSIGN();
bool PARAM_VALUE();
bool NEXT_PARAM_VALUE();
bool PARAM_LIST();
bool NEXT_PARAM();
bool PARAM();
bool DATA_TYPE();


//Definition of rule function
bool PROG(){
	if(token->type == DECLARE){
		//simulation of 1st rule
		if(!DECLARE_FUNCTION()){
			return false;
		}
		token = GetToken();
		if(token->type == EOL){
			token = GetToken();
			return PROG();
		}
		//syntax error
		else{
			return false;
		}	
	}
	else if(token->type == T_FUNCTION){
		//simulation of 2nd rule
		if(!DEFINE_FUNCTION()){
			return false;
		}
		token = GetToken();
		if(token->type == EOL){
			token = GetToken();
			return PROG();
		}
		//syntax error
		else{
			return false;
		}
	}
	else if(token->type == SCOPE){
		//simulation of 3rd rule
		return MAIN_FUNCTION();
	}
	else if(token->type == EOL){
		//simulation of 41. rule
		return PROG();
	}
	else if(token->type == END_OF_FILE){
		//simulation of 4th rule
		return true;
	}
	//syntax error
	return false;
}

bool DECLARE_FUNCTION(){
	return false;
}

bool DEFINE_FUNCTION(){
	return false;
}

bool MAIN_FUNCTION(){
	return false;
}

bool FUNCTION_ELEMENT(){
	return false;
}

bool ELEMENT_LIST(){
	return false;
}

bool STATEMENT(){
	return false;
}

bool VALUE(){
	return false;
}

bool ELSE_STATEMENT(){
	return false;
}

bool STAT_LIST(){
	return false;
}

bool EXPRESSION(){
	return false;
}

bool CALL_ASSIGN(){
	return false;
}

bool PARAM_VALUE(){
	return false;
}

bool NEXT_PARAM_VALUE(){
	return false;
}

bool PARAM_LIST(){
	return false;
}

bool NEXT_PARAM(){
	return false;
}

bool PARAM(){
	return false;
}

bool DATA_TYPE(){
	return false;
}

void parse(){
	token = GetToken();
	//syntax error
	if(!PROG()){
		print_err(2);
	}
}
