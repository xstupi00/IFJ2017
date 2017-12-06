///////////////////////////////////////////////////////////////////////////////////
// School:      Brno University of Technology, Faculty of Information Technology //
// Course:      Formal Languages and Compilers                                   //
// Project:     IFJ17                                                            //
// Module:      Syntax analysis (Recursive descent)                              //
// Authors:     Kristián Liščinský  (xlisci01)                                   //
//              Matúš Liščinský     (xlisci02)                                   //
//              Šimon Stupinský     (xstupi00)                                   //
//              Vladimír Marcin     (xmarci10)                                   //
///////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "parser.h"
#include "scanner.h"
#include "error.h"
#include "expr.h"
#include "semantic_control.h"
#include "strlib.h"
#include "generate.h"
#include "stack.h"

token_t *token;
string_t *current_function_name;
string_t *current_variable_name;
stack_t * label_stack;

/// DECLARATION OF NON-TERMINALS FUNCTIONS
bool PROG();
bool DECLARE_FUNCTION();	
bool DEFINE_FUNCTION();			
bool MAIN_FUNCTION();		
bool FUNCTION_ELEMENT(function_t *);	
bool ELEMENT_LIST(function_t *);		
bool STATEMENT(function_t *);		
bool VALUE(function_t *, variable_t *);		
bool ELSE_BRANCH(function_t *);	
bool STAT_LIST(function_t *);		
bool EXP_TO_PRINT(function_t *);		
bool PARAM_LIST(function_t *);	
bool NEXT_PARAM(function_t *, stack_t *);	
bool PARAM(function_t *, stack_t *);	
bool DATA_TYPE(int *);		 


void debug_p(char *c){
	(void)c;
#ifdef DEBUG
	fprintf(stdout, "parser: %s\n", c);
#endif
}

/// DEFINITIONS OF NON-TERMINALS FUNCTIONS
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
		/// check that all declared functions are defined
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
	
	/// syntax error
	return false;
}

bool DECLARE_FUNCTION(){
	debug_p("enter DECLARE_FUNCTION");

	/// create new function declaration
	function_t *current_function = init_function();
	current_function->defined = false;

	if(token->type == DECLARE){
		token = getToken();
		if(token->type == T_FUNCTION){
			token = getToken();
			if(token->type == ID){
				
				/// save name of current fucntion
				store_current_function_name(token);
				
				token = getToken();
				if(token->type == LEFT_R_BRACKET){
					token = getToken();
					if(PARAM_LIST(current_function)){
						if(token->type == AS){
							token = getToken();
							if(DATA_TYPE(&(current_function->return_type))){
								
								/// store function declaration into global symbol table
								store_fun_in_symtable(current_function,current_function_name->string);
								return true;
							}
						}
					}
				}
			}
		}
	}
	/// syntax error
	return false;
}

bool DEFINE_FUNCTION(){
	debug_p("enter DEFINE_FUNCTION");

	/// create new function definition 
	function_t *current_function = init_function();
	current_function->defined = true;
	
	if(token->type == T_FUNCTION){
		token = getToken();
		if(token->type == ID){
			
			/// save name of current function
			store_current_function_name(token);
			
			/// create LABEL with the same name as function
			variable_t * tmp = create_var(current_function_name->string, true);
			list_insert("LABEL ", tmp, NULL, NULL); 
			list_insert("CREATEFRAME ", NULL, NULL, NULL); /// create temporary frame
			list_insert("PUSHFRAME ", NULL, NULL, NULL); /// push frame on frame stack
			tmp = create_var("PRINT", false); /// create & define own variable
    		list_insert("DEFVAR ", tmp, NULL, NULL);

			token = getToken();
			if(token->type == LEFT_R_BRACKET){
				token = getToken();
				if(PARAM_LIST(current_function)){
					if(token->type == AS){
						token = getToken();
						if(DATA_TYPE(&(current_function->return_type))){
							
							/// store function definition in global symtable
							store_fun_in_symtable(current_function,current_function_name->string);
							if(token->type == EOL){
								token = getToken();
								if(FUNCTION_ELEMENT(current_function)){
									if(token->type == T_FUNCTION){
										
										/// default return value according to function type
										variable_t * type ;
										/// create instructions for default return 
										switch(current_function->return_type){
											case INTEGER: type = create_var("int@0", true); break; 
											case DOUBLE: type = create_var("float@0.0", true); break;
											case STRING: type = create_var("string@", true); break;
										}
										list_insert("PUSHS ", type, NULL, NULL);
										list_insert("POPFRAME ", NULL, NULL, NULL);
										list_insert("RETURN ", NULL, NULL, NULL); 
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
	/// syntax error
	return false;
}

bool MAIN_FUNCTION(){
	debug_p("enter MAIN_FUNCTION");

	/// create LABEL for scope, push created temporary frame on frame stack & define own auxiliary variable called PRINT
	variable_t * tmp = create_var("SCOPE", true);
	list_insert("LABEL ", tmp, NULL, NULL); 
	list_insert("CREATEFRAME ", NULL, NULL, NULL);
	list_insert("PUSHFRAME ", NULL, NULL, NULL);
	tmp = create_var("PRINT", false);
    list_insert("DEFVAR ", tmp, NULL, NULL);

	/// create scope function 
	function_t *current_function = init_function();
	current_function->defined = true;
	current_function->return_type = -1;
	strcpy(current_function_name->string,"scope");

	/// store scope in global symtable
	store_fun_in_symtable(current_function, current_function_name->string);

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
	/// syntax error
	return false;
}

bool FUNCTION_ELEMENT(function_t *f){
	debug_p("enter FUNCTION_ELEMENT");
	if(token->type == END){
		token = getToken();
		return true;
	}
	else if(token->type == DIM){
		/// create new variable
		variable_t *current_variable = init_variable();
		token = getToken();
		if(token->type == ID){
			/// save the name of current variable 
			store_current_variable_name(token);

			/// instruction for defining variable with the same name as local variable 
			variable_t * tmp = create_var(current_variable_name->string, false);
			list_insert("DEFVAR ", tmp, NULL, NULL);

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
	}
	else if(token->type == EOL	 ||	token->type == ID     || token->type == PRINT || 
			token->type == INPUT || token->type == RETURN || token->type == DO	  ||
			token->type == IF){
		return ELEMENT_LIST(f);
	}
	/// syntax error
	return false;
}

bool ELEMENT_LIST(function_t *f){
	debug_p("enter ELEMENT_LIST");
	if(	token->type == EOL	 ||	token->type == ID     || token->type == PRINT || 
		token->type == INPUT || token->type == RETURN || token->type == DO	  ||
		token->type == IF){
		return STATEMENT(f) && FUNCTION_ELEMENT(f);
	}
	/// syntax error
	return false;
}

bool STATEMENT(function_t *f){
	debug_p("enter STATEMENT");

	if(token->type == EOL){
		token = getToken();
		return true;
	}
	else if(token->type == ID){
		/// save the current variable name
		store_current_variable_name(token);	
		variable_t *var = find_variable(f->local_symtable,current_variable_name->string);
		/// undefined variable
		if(!var)
			print_err(3);
		
		token = getToken();
		if(token->type == ASSIGNMENT_EQ){
			
			/// processing expression
			expression(f,var);
			
			/// pops result to variable from data stack 
			/// (expression leaves result on data stack) 
			variable_t * tmp = create_var(current_variable_name->string, false);
			list_insert("POPS ", tmp, NULL, NULL);
		
			token = getToken();
			if(token->type == EOL){
				token = getToken();
				return true;
			}
		}
	}
	else if(token->type == PRINT){
		/// if PRINT command doesn't contain any expression
		token = getToken();
		if(token->type == SEMICOLON){
			return false;
		}
		ungetToken();

		variable_t *print_var = init_variable();
		expression(f,print_var);
		
		/// pops result to own variable from data stack & print it on output 
		variable_t * tmp = create_var("PRINT", false);
		list_insert("POPS ", tmp, NULL, NULL);
		list_insert("WRITE ", tmp, NULL, NULL);
		

		token = getToken();
		if(token->type == SEMICOLON){
			token = getToken();
			return EXP_TO_PRINT(f);
		}

	}
	else if(token->type == INPUT){
		token = getToken();
		if(token->type == ID){
			store_current_variable_name(token);
			variable_t *var = find_variable(f->local_symtable,current_variable_name->string);
			/// undefinied variable
			if(!var)
				print_err(3);
			
			/// finding out which data type is variable in which we store input
			/// printing string "? " as starting input
			/// read input from user & store it to variable with defined data type  
			variable_t * type;
			switch(var->data_type){
				case INTEGER: type = create_var(I, true); break;
				case DOUBLE: type = create_var(F, true); break;
				case STRING: type = create_var(S, true); break;
			}
			variable_t *user_input = create_var(current_variable_name->string, false);
			variable_t *input_symbol = create_var("string@?\\032", true);
			
			list_insert("WRITE ", input_symbol, NULL, NULL );
			list_insert("READ ", user_input, type, NULL);
			
			token = getToken();
			if(token->type == EOL){
				token = getToken();
				return true;
			}
		}
	}
	else if(token->type == RETURN){
		/// scope can't contain return statement
		if(!strcmp(current_function_name->string,"scope")) 
			return false;
		f->return_var->data_type = f->return_type;
		
		/// procesing expression
		expression(f,f->return_var); 
		/// pops frame from frame stack and return 
		list_insert("POPFRAME ", NULL, NULL, NULL);
		list_insert("RETURN ", NULL, NULL, NULL); 
		token = getToken();
		if(token->type == EOL){
			token = getToken();
			return true;
		}
	}
	else if(token->type == DO){
		token = getToken();
		if(token->type == WHILE){ 
			static int while_counter;
			/// creating uniq LABEL names using counter and char 'W' as while
			while_counter++;
			char * lab_name = gen_label_name(while_counter, 'W');
			variable_t * L1 = create_var(lab_name, true);
			list_insert("LABEL ", L1, NULL, NULL);
			
			while_counter++;
			lab_name = gen_label_name(while_counter, 'W');
			variable_t * L2 = create_var(lab_name, true);
			/// push LABEL names on label stack in correct order
			S_Push(label_stack, L2);
			S_Push(label_stack, L1);
			
			
			variable_t * bool_var = init_variable();
			bool_var->data_type = BOOLEAN;
			expression(f,bool_var);

			/// creating auxiliary variables, pops result of expr from data stack
			/// and jump if it's not true (notice operation NOT)  
			variable_t * result = create_var("PRINT ", false);
			variable_t * btrue = create_var("bool@true ", true);
			
			list_insert("POPS ", result, NULL, NULL);
			list_insert("NOT ", result, result, NULL);
			list_insert("JUMPIFEQ ", L2, result, btrue);
			
			token = getToken();
			if(token->type == EOL){
				token = getToken();
				if(STAT_LIST(f)){
					token = getToken();
					if(token->type == EOL){ 
						/// at the end of stat. list 
						/// getting LABEL names from top of label stack & add instruction for JUMP to that LABEL
						variable_t * L = S_Top(label_stack);
						S_Pop(label_stack);
						list_insert ("JUMP ", L, NULL, NULL);
						/// getting another LABEL from stack which starts right here
						L = S_Top(label_stack);
						S_Pop(label_stack);
						list_insert("LABEL ", L, NULL, NULL);
						token = getToken();
						return true;
					}
				}
			}
		}
	}
	else if(token->type == IF){
		static int if_counter;
		variable_t *bool_var = init_variable();

		bool_var->data_type = BOOLEAN;
		/// processing expression
		expression(f,bool_var); 
		
		/// creating uniq LABEL names using counter and char 'I' as if
		/// push created LABEL name on label stack
		/// creating auxiliary variables, pops result of expr from data stack
		/// and jump if it's not true 

		variable_t * result = create_var("PRINT ", false);
		variable_t * btrue = create_var("bool@true", true);

		list_insert("POPS ", result, NULL, NULL);
		list_insert("NOT ", result, result, NULL);
		if_counter ++;
		char * if_lab_name = gen_label_name(if_counter,'I');
		variable_t * L1 = create_var(if_lab_name,true);
		S_Push(label_stack, L1);
		list_insert("JUMPIFEQ ", L1, result, btrue); 

		token = getToken();
		if(token->type == THEN){
			token = getToken();
			if(token->type == EOL){
				token = getToken();
				if(STAT_LIST(f)){
					/// getting top LABEL name from stack & generate LABEL for it to the end 
					/// generate another LABEL, push it on stack 
					/// jump to that label 
					if_counter++;
					if_lab_name = gen_label_name(if_counter,'I');
					variable_t * L2 = create_var(if_lab_name,true); 
					variable_t * L1  = S_Top(label_stack);
					S_Pop(label_stack);
					S_Push(label_stack, L2);
					list_insert("JUMP ", L2, NULL, NULL);
					list_insert("LABEL ", L1, NULL, NULL); 
					
					if(ELSE_BRANCH(f)){
						token = getToken();
						if(token->type == IF){
							token = getToken();
							if(token->type == EOL){
								/// getting LABEL name from top of stack & generating label for it
								variable_t * L2 = S_Top(label_stack);
								S_Pop(label_stack);
								list_insert("LABEL ", L2, NULL, NULL); 
								
								token = getToken();
								return true;
							}
						}	
					}
				}
			}
		}	
	}
	/// syntax error
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

bool ELSE_BRANCH(function_t *f){
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
	/// syntax error
	return false;
}

bool VALUE(function_t *f, variable_t *v){
	debug_p("enter VALUE");
	if(token->type == EOL){
		/// store variable in local symtable of function 'f'
		store_var_in_symtable(f,v,current_variable_name->string);
		
		/// default assignment 0 | 0.0 | "" to newly defined variable according to type of variable
		variable_t * l_value = create_var(current_variable_name->string, false);
		variable_t * type ;
		switch(v->data_type){
			case INTEGER: type = create_var("int@0", true);break;
			case DOUBLE: type = create_var("float@0.0", true); break;
			case STRING: type = create_var("string@", true);break;
		}

		list_insert("MOVE ", l_value, type, NULL);
		return true;
	}
	else if(token->type == ASSIGNMENT_EQ){
		/// processing expression
		expression(f,v);

		/// store variable in local symtable of function 'f'
		store_var_in_symtable(f,v,current_variable_name->string);
		/// initialization of variable at declaration
		/// pops result to variable from data stack (expression pushed it on data stack)
		variable_t * l_value = create_var(current_variable_name->string, false);
		list_insert("POPS ", l_value, NULL, NULL);
		
		token = getToken();
		return true;
	}
	/// syntax error
	return false;
}

bool EXP_TO_PRINT(function_t *f){
	debug_p("enter EXPRESSION");
	
	if(token->type == EOL){
		token = getToken();
		return true;
	}
	else{
		ungetToken();
		variable_t *print_var = init_variable();
		expression(f,print_var);
		/// creating auxiliary variable 
		/// pops result to that variable and print variable to output
		variable_t * tmp = create_var("PRINT", false);
		list_insert("POPS ", tmp, NULL, NULL);
		list_insert("WRITE ", tmp, NULL, NULL);
		

		token = getToken();
		if(token->type == SEMICOLON){
			token = getToken();
			return EXP_TO_PRINT(f);
		}
	}
	/// syntax error
	return false;
}

bool PARAM_LIST(function_t *f){
	debug_p("enter PARAM_LIST");

	if(token->type == ID){
		stack_t * param_stack; /// creating param stack
		if(f->defined){
			param_stack = S_Init();
		}
		if(PARAM(f, param_stack) && NEXT_PARAM(f, param_stack)){
			if(f->defined){
				while(!S_Empty(param_stack)){
					/// pops all real parameters from stack to created variables 
			 	    variable_t * tmp = S_Top(param_stack);
			       	S_Pop(param_stack);
					list_insert("POPS ", tmp, NULL, NULL);
					
				}
			}	
			return true;
		}
	}
	else if(token->type == RIGHT_R_BRACKET){
		token = getToken();
		return true;
	}
	/// syntax error
	return false;
		
}

bool NEXT_PARAM(function_t *f, stack_t * param_stack ){
	debug_p("enter NEXT_PARAM");
	
	if(token->type == COMMA){
		token = getToken();
		return PARAM(f, param_stack) && NEXT_PARAM(f, param_stack);
	}
	else if(token->type == RIGHT_R_BRACKET){
		token = getToken();
		return true;
	}
	/// syntax error
	return false;
}

bool PARAM(function_t *f, stack_t * param_stack ){
	debug_p("enter PARAM");
	variable_t *current_variable = init_variable();
	if(token->type == ID){
		store_current_variable_name(token);
		if(f->defined){		
			/// defining variable with the same name as formal parameter 
			// and push it to param stack (for assigning in correct order)				
			variable_t * tmp = create_var(current_variable_name->string, false);
	    	list_insert("DEFVAR ", tmp, NULL, NULL);
			
			S_Push(param_stack, tmp);
		}
		token = getToken();
		if(token->type == AS){
			token = getToken();
			int type; 		
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
	/// syntax error
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
	/// syntax error
	return false;
}

void parse(){
	debug_p("***START***");
	token = getToken();
	/// syntax error
	if(!PROG()){
		print_err(2);
	}
	debug_p("***END***");
}
