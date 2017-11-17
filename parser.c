#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "parser.h"
#include "scanner.h"
#include "error.h"
#include "expr.h"
#include "symtable.h"
#include "semantic_control.h"

token_t *token;
bool is_scope;

//Declaration of state functions
bool PROG();	// :)
bool DECLARE_FUNCTION();	// :)
bool DEFINE_FUNCTION();		// :)	
bool MAIN_FUNCTION();		// :)
bool FUNCTION_ELEMENT();	// :)
bool ELEMENT_LIST();		// :)
bool STATEMENT();		// :|
bool VALUE();		// :)
bool ELSE_STATEMENT();	// :|
bool STAT_LIST();		// :|
bool EXPRESSION();		// :)
bool PARAM_LIST();	// :)
bool NEXT_PARAM();	// :)
bool PARAM();	// :)
bool DATA_TYPE();	// :)	 


void debug_p(char *c){
	fprintf(stdout, "parser: %s\n", c);
}

//Definition of rule functions
bool PROG(){
	debug_p("enter PROG");
	
	if(token->type == DECLARE){
		if(DECLARE_FUNCTION()){
			if(token->type == EOL){
				token = getToken();
				return PROG();
			}
		}	
	}
	else if(token->type == T_FUNCTION){
		if(DEFINE_FUNCTION()){
			if(token->type == EOL){
				token = getToken();
				return PROG();
			}
		}
	}
	else if(token->type == SCOPE){
		is_scope = true;
		if(MAIN_FUNCTION()){	
			while(token->type != END_OF_FILE){
				if(token->type != EOL)
					return false;
				token = getToken();
			}
			return true;
		} 
	}
	else if(token->type == EOL){
		token = getToken();
		return PROG();
	}
	else if(token->type == END_OF_FILE)
		return true;
	
	return false;
}

bool DECLARE_FUNCTION(){
	debug_p("enter DECLARE_FUNCTION");

	if(token->type == DECLARE){
		token = getToken();
		if(token->type == T_FUNCTION){
			token = getToken();
			if(token->type == ID){
				token = getToken();
				if(token->type == LEFT_R_BRACKET){
					token = getToken();
					if(PARAM_LIST()){
						if(token->type == AS){
							token = getToken();
							return DATA_TYPE();
						}
					}
				}
			}
		}
	}
	return false;
}

bool DEFINE_FUNCTION(){
	debug_p("enter DEFINE_FUNCTION");
	
	if(token->type == T_FUNCTION){
		token = getToken();
		if(token->type == ID){
			token = getToken();
			if(token->type == LEFT_R_BRACKET){
				token = getToken();
				if(PARAM_LIST()){
					if(token->type == AS){
						token = getToken();
						if(DATA_TYPE()){
							if(token->type == EOL){
								token = getToken();
								if(FUNCTION_ELEMENT()){
									if(token->type == T_FUNCTION){
										token = getToken();
										return true;
									}
								}
							}
						}	
					}
				}
			}
		}
	}
	return false;
}

bool MAIN_FUNCTION(){
	debug_p("enter MAIN_FUNCTION");

	if(token->type == SCOPE){
		token = getToken();
		if(token->type == EOL){
			token = getToken();
			if(FUNCTION_ELEMENT()){
				if(token->type == SCOPE){
					token = getToken();
					return true;
				}
			}
		}
	}
	return false;
}

bool FUNCTION_ELEMENT(){
	debug_p("enter FUNCTION_ELEMENT");
	if(token->type == END){
		token = getToken();
		return true;
	}
	else if(token->type == DIM){
		token = getToken();
		if(token->type == ID){
			token = getToken();
			if(token->type == AS){
				token = getToken();
				if(DATA_TYPE() && VALUE()){
					if(token->type == EOL){
						token = getToken();
						return FUNCTION_ELEMENT();
					}
				}
			}
		}
	}
	else if(token->type == EOL	 ||	token->type == ID     || token->type == PRINT || 
			token->type == INPUT || token->type == RETURN || token->type == DO	  ||
			token->type == IF){
		return ELEMENT_LIST();
	}
	return false;
}

bool ELEMENT_LIST(){
	debug_p("enter ELEMENT_LIST");
	if(	token->type == EOL	 ||	token->type == ID     || token->type == PRINT || 
		token->type == INPUT || token->type == RETURN || token->type == DO	  ||
		token->type == IF){
		return STATEMENT() && FUNCTION_ELEMENT();
	}
	return false;
}

bool STATEMENT(){
	debug_p("enter STATEMENT");
	
	if(token->type == EOL){
		token = getToken();
		return true;
	}
	else if(token->type == ID){
		token = getToken();
		if(token->type == ASSIGNMENT_EQ){
			expression();
			token = getToken();
			if(token->type == EOL){
				token = getToken();
				return true;
			}
		}
	}
	else if(token->type == PRINT){
		//if PRINT command doesn't contain any expression
		token = getToken();
		if(token->type == SEMICOLON){
			return false;
		}
		ungetToken();

		expression();
		token = getToken();
		if(token->type == SEMICOLON){
			token = getToken();
			return EXPRESSION();
		}

	}
	else if(token->type == INPUT){
		token = getToken();
		if(token->type == ID){
			token = getToken();
			if(token->type == EOL){
				token = getToken();
				return true;
			}
		}
	}
	else if(token->type == RETURN){
		if(is_scope) return false;
		expression();
		token = getToken();
		if(token->type == EOL){
			token = getToken();
			return true;
		}
	}
	else if(token->type == DO){
		token = getToken();
		if(token->type == WHILE){
			expression();
			token = getToken();
			if(token->type == EOL){
				token = getToken();
				if(STAT_LIST()){
					token = getToken();
					if(token->type == EOL){
						token = getToken();
						return true;
					}
				}
			}
		}
	}
	else if(token->type == IF){
		expression();
		token = getToken();
		if(token->type == THEN){
			token = getToken();
			if(token->type == EOL){
				token = getToken();
				if(STAT_LIST() && ELSE_STATEMENT()){
					token = getToken();
					if(token->type == IF){
						token = getToken();
						if(token->type == EOL){
							token = getToken();
							return true;
						}
					}	
				}
			}
		}	
	}
	return false;
}

bool STAT_LIST(){
	debug_p("enter STAT_LIST");

	if(	token->type == EOL	 ||	token->type == ID     || token->type == PRINT || 
		token->type == INPUT || token->type == RETURN || token->type == DO	  ||
		token->type == IF){
		return STATEMENT() && STAT_LIST();
	}
	else if(token->type == LOOP || token->type == ELSE || token->type == END){
		return true;
	}
	return false;
}

bool ELSE_STATEMENT(){
	debug_p("enter ELSE_STATEMENT");
	
	if(token->type == END){
		return true;
	}
	else if(token->type == ELSE){
		token = getToken();
		if(token->type == EOL){
			return STAT_LIST();
		}
	}
	return false;
}

bool VALUE(){
	debug_p("enter VALUE");
	if(token->type == EOL){
		return true;
	}
	else if(token->type == ASSIGNMENT_EQ){
		expression();
		token = getToken();
		return true;
	}
	return false;
}

bool EXPRESSION(){
	debug_p("enter EXPRESSION");
	
	if(token->type == EOL){
		token = getToken();
		return true;
	}
	else{
		ungetToken();
		expression();
		token = getToken();
		if(token->type == SEMICOLON){
			token = getToken();
			return EXPRESSION();
		}
	}
	return false;
}

bool PARAM_LIST(){
	debug_p("enter PARAM_LIST");
	if(token->type == ID){
		return PARAM() && NEXT_PARAM();
	}
	else if(token->type == RIGHT_R_BRACKET){
		token = getToken();
		return true;
	}
	return false;
}

bool NEXT_PARAM(){
	debug_p("enter NEXT_PARAM");
	if(token->type == COMMA){
		token = getToken();
		return PARAM() && NEXT_PARAM();
	}
	else if(token->type == RIGHT_R_BRACKET){
		token = getToken();
		return true;
	}
	return false;
}

bool PARAM(){
	debug_p("enter PARAM");
	if(token->type == ID){
		token = getToken();
		if(token->type == AS){
			token = getToken();
			return DATA_TYPE();
		}
	}
	return false;
}

bool DATA_TYPE(){
	debug_p("enter DATA_TYPE");
	if(token->type == INTEGER){
		token = getToken();
		return true;
	}
	else if(token->type == DOUBLE){
		token = getToken();
		return true;
	}
	else if(token->type == STRING){
		token = getToken();
		return true;
	}
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
