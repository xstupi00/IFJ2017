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
#include "strlib.h"

token_t *token;
string_t *current_function_name;
string_t *current_variable_name;

//Declaration of non-terminal functions
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
bool PARAM_LIST();	
bool NEXT_PARAM();	
bool PARAM();	
bool DATA_TYPE();		 


void debug_p(char *c){
	fprintf(stdout, "parser: %s\n", c);
}

//Definition of non-terminal functions
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
		//check that all declared functions are defined
		check_function_definitions();
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
	
	return false;
}

bool DECLARE_FUNCTION(){
	debug_p("enter DECLARE_FUNCTION");

	function_t *current_function = init_function();
	current_function->defined = false;

	if(token->type == DECLARE){
		token = getToken();
		if(token->type == T_FUNCTION){
			token = getToken();
			if(token->type == ID){
				//save name of current fucntion
				store_current_function_name(token);
				token = getToken();
				if(token->type == LEFT_R_BRACKET){
					token = getToken();
					if(PARAM_LIST(current_function)){
						if(token->type == AS){
							token = getToken();
							if(DATA_TYPE(&(current_function->return_type))){
								store_fun_in_symtable(current_function,current_function_name->string);
								return true;
							}
						}
					}
				}
			}
			else if(token->type == LENGTH || token->type == SUBSTR || 
					token->type == ASC || token->type == CHR){
				//redefinition of builtin function
				print_err(3);
			}
		}
	}
	return false;
}

bool DEFINE_FUNCTION(){
	debug_p("enter DEFINE_FUNCTION");

	function_t *current_function = init_function();
	current_function->defined = true;
	
	if(token->type == T_FUNCTION){
		token = getToken();
		if(token->type == ID){
			//save name of current function
			store_current_function_name(token);
			token = getToken();
			if(token->type == LEFT_R_BRACKET){
				token = getToken();
				if(PARAM_LIST(current_function)){
					if(token->type == AS){
						token = getToken();
						if(DATA_TYPE(&(current_function->return_type))){
							if(token->type == EOL){
								token = getToken();
								if(FUNCTION_ELEMENT(current_function)){
									if(token->type == T_FUNCTION){
										store_fun_in_symtable(current_function,current_function_name->string);
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
		else if(token->type == LENGTH || token->type == SUBSTR || 
				token->type == ASC || token->type == CHR){
			//redefinition of built-in function
			print_err(3);
		}
	}
	return false;
}

bool MAIN_FUNCTION(){
	debug_p("enter MAIN_FUNCTION");

	function_t *current_function = init_function();
	current_function->defined = true;
	current_function->return_type = -1;
	free(current_function->return_var);
	current_function->return_var = NULL;
	free(current_function->params);
	current_function->params = NULL;
	strcpy(current_function_name->string,"scope");

	if(token->type == SCOPE){
		token = getToken();
		if(token->type == EOL){
			token = getToken();
			if(FUNCTION_ELEMENT(current_function)){
				if(token->type == SCOPE){
					token = getToken();
					return true;
				}
			}
		}
	}
	return false;
}

bool FUNCTION_ELEMENT(function_t *f){
	debug_p("enter FUNCTION_ELEMENT");
	if(token->type == END){
		token = getToken();
		return true;
	}
	else if(token->type == DIM){
		variable_t *current_variable = init_variable();
		token = getToken();
		if(token->type == ID){
			//save name of current variable 
			store_current_variable_name(token);
			token = getToken();
			if(token->type == AS){
				token = getToken();
				if(DATA_TYPE(&(current_variable->data_type)) && VALUE(f,current_variable)){
					if(token->type == EOL){
						token = getToken();
						return FUNCTION_ELEMENT(f);
					}
				}
			}
		}
		//name of var is equal to name of built-in function
		else if(token->type == LENGTH || token->type == SUBSTR || 
				token->type == ASC || token->type == CHR){
			print_err(3);
		}
	}
	else if(token->type == EOL	 ||	token->type == ID     || token->type == PRINT || 
			token->type == INPUT || token->type == RETURN || token->type == DO	  ||
			token->type == IF){
		return ELEMENT_LIST(f);
	}
	return false;
}

bool ELEMENT_LIST(function_t *f){
	debug_p("enter ELEMENT_LIST");
	if(	token->type == EOL	 ||	token->type == ID     || token->type == PRINT || 
		token->type == INPUT || token->type == RETURN || token->type == DO	  ||
		token->type == IF){
		return STATEMENT(f) && FUNCTION_ELEMENT(f);
	}
	return false;
}

bool STATEMENT(function_t *f){
	debug_p("enter STATEMENT");
	
	if(token->type == EOL){
		token = getToken();
		return true;
	}
	else if(token->type == ID){
		store_current_variable_name(token);	
		variable_t *var = find_variable(f->local_symtable,current_variable_name->string);
		//undefined variable
		if(!var)
			print_err(3);
		token = getToken();
		if(token->type == ASSIGNMENT_EQ){
		expression(f,var);
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

		expression(f,NULL);
		token = getToken();
		if(token->type == SEMICOLON){
			token = getToken();
			return EXPRESSION(f);
		}

	}
	else if(token->type == INPUT){
		token = getToken();
		if(token->type == ID){
			store_current_variable_name(token);
			variable_t *var = find_variable(f->local_symtable,current_variable_name->string);
			//undefinied variable
			if(!var)
				print_err(3);
			token = getToken();
			if(token->type == EOL){
				token = getToken();
				return true;
			}
		}
	}
	else if(token->type == RETURN){
		//TODO asking for name of current function instead is_scope flag
		if(!strcmp(current_function_name->string,"scope")) return false;
		f->return_var->data_type = f->return_type;
		expression(f,f->return_var);
		token = getToken();
		if(token->type == EOL){
			token = getToken();
			return true;
		}
	}
	else if(token->type == DO){
		token = getToken();
		if(token->type == WHILE){
			expression(f,NULL);
			token = getToken();
			if(token->type == EOL){
				token = getToken();
				if(STAT_LIST(f)){
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
		expression(f,NULL);
		token = getToken();
		if(token->type == THEN){
			token = getToken();
			if(token->type == EOL){
				token = getToken();
				if(STAT_LIST(f) && ELSE_STATEMENT(f)){
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

bool STAT_LIST(function_t *f){
	debug_p("enter STAT_LIST");

	if(	token->type == EOL	 ||	token->type == ID     || token->type == PRINT || 
		token->type == INPUT || token->type == RETURN || token->type == DO	  ||
		token->type == IF){
		return STATEMENT(f) && STAT_LIST(f);
	}
	else if(token->type == LOOP || token->type == ELSE || token->type == END){
		return true;
	}
	return false;
}

bool ELSE_STATEMENT(function_t *f){
	debug_p("enter ELSE_STATEMENT");
	
	if(token->type == END){
		return true;
	}
	else if(token->type == ELSE){
		token = getToken();
		if(token->type == EOL){
			return STAT_LIST(f);
		}
	}
	return false;
}

bool VALUE(function_t *f, variable_t *v){
	debug_p("enter VALUE");
	if(token->type == EOL){
		store_var_in_symtable(f,v,current_variable_name->string);
		return true;
	}
	else if(token->type == ASSIGNMENT_EQ){
		expression(f,v);
		store_var_in_symtable(f,v,current_variable_name->string);
		token = getToken();
		return true;
	}
	return false;
}

bool EXPRESSION(function_t *f){
	debug_p("enter EXPRESSION");
	
	if(token->type == EOL){
		token = getToken();
		return true;
	}
	else{
		ungetToken();
		expression(f,NULL);
		token = getToken();
		if(token->type == SEMICOLON){
			token = getToken();
			return EXPRESSION(f);
		}
	}
	return false;
}

bool PARAM_LIST(function_t *f){
	debug_p("enter PARAM_LIST");
	if(token->type == ID){
		return PARAM(f) && NEXT_PARAM(f);
	}
	else if(token->type == RIGHT_R_BRACKET){
		token = getToken();
		return true;
	}
	return false;
}

bool NEXT_PARAM(function_t *f){
	debug_p("enter NEXT_PARAM");
	if(token->type == COMMA){
		token = getToken();
		return PARAM(f) && NEXT_PARAM(f);
	}
	else if(token->type == RIGHT_R_BRACKET){
		token = getToken();
		return true;
	}
	return false;
}

bool PARAM(function_t *f){
	debug_p("enter PARAM");
	variable_t *current_variable = init_variable();
	if(token->type == ID){
		store_current_variable_name(token);
		token = getToken();
		if(token->type == AS){
			token = getToken();
			int type; //type of parameter		
			if(DATA_TYPE(&type)){
				if(f->params->capacity < (f->params->length+1))
					extendStr(f->params,(f->params->capacity*2));
				f->params->string[f->params->length++] = code_param_type(type);
				if(f->defined){
					//store param of define function into local symtable 
					//new variable in function mustn't have the same name
					current_variable->data_type = type;
					store_var_in_symtable(f,current_variable,current_variable_name->string);
				}	
				return true;
			}
		}
	}
	else if((token->type == LENGTH || token->type == SUBSTR || 
			 token->type == ASC || token->type == CHR) && f->defined){
		print_err(3);
	}
	return false;
}

bool DATA_TYPE(int *type){
	debug_p("enter DATA_TYPE");
	if(token->type == INTEGER){
		token = getToken();
		*type = INTEGER;
		return true;
	}
	else if(token->type == DOUBLE){
		token = getToken();
		*type = DOUBLE;
		return true;
	}
	else if(token->type == STRING){
		token = getToken();
		*type = STRING;
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
