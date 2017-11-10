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


void debug_p(char *c){
	fprintf(stdout, "parser: %s\n", c);
}

//Definition of rule function
bool PROG(){
	debug_p("enter PROG");

	if(token->type == DECLARE){
		if(!DECLARE_FUNCTION())
			return false;
		token = getToken();
		if(token->type == EOL){
			token = getToken();
			return PROG();
		}else return false;	
	}
	else if(token->type == T_FUNCTION){
		if(!DEFINE_FUNCTION())
			return false;
		token = getToken();
		if(token->type == EOL){
			token = getToken();
			return PROG();
		}else return false;
	}
	else if(token->type == SCOPE)
		return MAIN_FUNCTION();

	else if(token->type == EOL)
		return PROG();
	
	else if(token->type == END_OF_FILE)
		return true;
	
	return false;
}

bool DECLARE_FUNCTION(){
	debug_p("enter DECLAERE_FUNCTION");
	if(token->type == DECLARE)
		token = getToken();
	else return false;
	
	if(token->type == T_FUNCTION)
		token = getToken();
	else return false;
	
	if(token->type == ID)
		token = getToken();
	else return false;

	if(token->type == LEFT_R_BRACKET)
		token = getToken();
	else return false;

	if(PARAM_LIST()){
		if(token->type == RIGHT_R_BRACKET)
			token = getToken();
		else return false;

		if(token->type == AS){
			token = getToken();
			return DATA_TYPE();
		}else return false;
	}
	else return false;
}

bool DEFINE_FUNCTION(){
	debug_p("enter DEFINE_FUNCTION");
	if(token->type == T_FUNCTION)
		token = getToken();
	else return false;
	
	if(token->type == ID)
		token = getToken();
	else return false;

	if(token->type == LEFT_R_BRACKET)
		token = getToken();
	else return false;

	if(PARAM_LIST()){
		if(token->type == RIGHT_R_BRACKET)
			token = getToken();
		else return false;

		if(token->type == AS)
			token = getToken();
		else return false;
		
		if(DATA_TYPE()){
			if(token->type == EOL)
				token = getToken();
			else return false;

			if(FUNCTION_ELEMENT()){
				if(token->type == END)
					token = getToken();					
				else return false;

				if(token->type == T_FUNCTION){
					token = getToken();
					return true;
				}else return false;

			}else return false;

		}else return false;

	}else return false;
}

bool MAIN_FUNCTION(){
	debug_p("enter MAIN_FUNCTION");
	if(token->type == SCOPE)
		token = getToken();
	else return false;

	if(token->type == EOL)
		token = getToken();
	else return false;

	//TODO look at function_element->epsilon rule
	//where i have to ask for that
	if(FUNCTION_ELEMENT()){
		if(token->type == END)
			token = getToken();
		else return false;
		
		if(token->type == SCOPE){
			token = getToken();
			return true;
		}else return false;

	}else return false;
}

bool FUNCTION_ELEMENT(){
	debug_p("enter FUNCTION_ELEMENT");
	return false;
}

bool ELEMENT_LIST(){
	debug_p("enter ELEMENT_LIST");
	return false;
}

bool STATEMENT(){
	debug_p("enter STATEMENT");
	return false;
}

bool VALUE(){
	debug_p("enter VALUE");
	return false;
}

bool ELSE_STATEMENT(){
	debug_p("enter ELSE_STATEMENT");
	return false;
}

bool STAT_LIST(){
	debug_p("enter STAT_LIST");
	return false;
}

bool EXPRESSION(){
	debug_p("enter EXPRESSION");
	return false;
}

bool CALL_ASSIGN(){
	debug_p("enter CALL_ASSIGN");
	return false;
}

bool PARAM_VALUE(){
	debug_p("enter PARAM_VALUE");
	return false;
}

bool NEXT_PARAM_VALUE(){
	debug_p("enter NEXT_PARAM_VALUE");
	return false;
}

bool PARAM_LIST(){
	debug_p("enter PARAM_LIST");
	return false;
}

bool NEXT_PARAM(){
	debug_p("enter NEXT_PARAM");
	return false;
}

bool PARAM(){
	debug_p("enter PARAM");
	return false;
}

bool DATA_TYPE(){
	debug_p("enter DATA_TYPE");
	return false;
}

void parse(){
	debug_p("***START***");
	token = getToken();
	//syntax error
	if(!PROG()){
		print_err(2);
	}
	debug_p("***END***");
}
