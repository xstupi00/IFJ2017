///////////////////////////////////////////////////////////////////////////////////
// School:      Brno University of Technology, Faculty of Information Technology //
// Course:      Formal Languages and Compilers                                   //
// Project:     IFJ17                                                            //
// Module:      Generator of code                                                //
// Authors:     Kristián Liščinský  (xlisci01)                                   //
//              Matúš Liščinský     (xlisci02)                                   //
//              Šimon Stupinský     (xstupi00)                                   //
//              Vladimír Marcin     (xmarci10)                                   //
///////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <math.h>
#include "generate.h"
#include "error.h"
#include "semantic_control.h"
#include "scanner.h"
#include "clear.h"

/// replace malloc & calloc calls with our malloc & calloc wrapper
#define malloc(size) _malloc(size)
#define calloc(num, size) _calloc(num, size)


list_t * list;

// generator of label names
char * gen_label_name(int i, char c){
	unsigned digits	= floor(log10(abs(i)))+1; /// getting count of digits
	char * name = (char *) malloc(digits*sizeof(int)+3); 
	if(!name)
		print_err(99);
	name[0] = '&'; /// first character of string will be '&'
	name[1] = c;  
	sprintf(name+2, "%d", i);
	return name;
}

void list_init()
{
	list = (list_t*) malloc(sizeof(list_t));
	if(!list)
		print_err(99);
	list->First=NULL;
	list->Last=NULL;
}


variable_t * create_var(char *str1, bool constant){
	variable_t * tmp = init_variable();
	tmp->data.str = (char*)malloc(sizeof(char)*(strlen(str1)+2));
 	if(!tmp->data.str)
 		print_err(99);
	strcpy(tmp->data.str, str1);
	strcat(tmp->data.str, " ");
	tmp->constant = constant;
	return tmp;
}

void concat()
{

	variable_t * tmp1 = create_var("&C1 ", false);
	variable_t * tmp2 = create_var("&C2 ", false);	

	list_insert("CREATEFRAME ",NULL, NULL, NULL);
	list_insert("PUSHFRAME ",NULL, NULL, NULL);
	list_insert("DEFVAR ", tmp1, NULL, NULL);
	list_insert("DEFVAR ", tmp2, NULL, NULL);

	list_insert("POPS ", tmp1, NULL, NULL);
	list_insert("POPS ", tmp2, NULL, NULL);

	list_insert("CONCAT ",tmp2, tmp2, tmp1);
	list_insert("PUSHS ",tmp2, NULL, NULL);
	list_insert("POPFRAME ",NULL, NULL, NULL);

}

/// function to create & initialize instruction 
instruction_t * instr_init (){
	instruction_t * new =  (instruction_t *)malloc(sizeof(instruction_t));
	if(!new)
		print_err(99);
	new->instr_name = (char *) calloc(15,sizeof(char));
	new->op1 = new->op2 = new->op3 = NULL;
	new->next = NULL;

	return new;
}

/// function to create new variable same as src variable
variable_t * copy_variable(variable_t * src){
	variable_t * new = init_variable();

	new->data_type = src->data_type;
	switch(new->data_type){
		case INTEGER: new->data.i = src->data.i;break;
		case DOUBLE: new->data.d  = src->data.d;break;
		default: 	if(src->data.str){
						new->data.str = (char *) malloc(sizeof(char)*(strlen(src->data.str)+1));
 					 	if(!new->data.str)
 					 		print_err(99);
 					 	strcpy(new->data.str, src->data.str);
					}break;
	}
	new->constant = src->constant;
	return new;
}

void list_insert(char * instr, variable_t * par1, variable_t * par2, variable_t * par3 )
{
	instruction_t * new = instr_init();
	strcpy(new->instr_name,instr);
	if(par1)
		new->op1 = copy_variable(par1);
	else new->op1 = NULL;

	if(par2)
		new->op2 = copy_variable(par2);
	else new->op2 = NULL;

	if(par3)
		new->op3 = copy_variable(par3);
	else new->op3 = NULL;

	if(list){
		if(list->First == NULL)
			list->First=list->Last=new;
		else {
			list->Last->next=new;
			list->Last=new;
		}
	}
}

void process_string (char * orig_string){

	if (!orig_string)
		return;

	for(unsigned i = 0; orig_string[i]!='\0' ; i++){
		if(isalpha(orig_string[i]) || isdigit(orig_string[i]))
			printf("%c",orig_string[i]);		
		else
			printf("\\%03d", (int)((unsigned char)orig_string[i]));			
	}	
	printf(" ");
}


void length_of_str(variable_t * l_value){

	list_insert("CREATEFRAME ",NULL, NULL, NULL);
	list_insert("PUSHFRAME ",NULL, NULL, NULL);
	variable_t * tmp = create_var("RETURN ", false);

	list_insert("DEFVAR ",tmp, NULL, NULL);
	variable_t * str = create_var("PRINT ", false);
	list_insert("DEFVAR ",str, NULL, NULL);
	list_insert("POPS ", str, NULL, NULL);

	list_insert("STRLEN ",tmp, str, NULL);

	list_insert("PUSHS ",tmp, NULL, NULL);

	if(l_value && l_value->data_type == DOUBLE)
		list_insert("INT2FLOATS ",NULL, NULL, NULL);
	list_insert("POPFRAME ",NULL, NULL, NULL);

}


void retype(variable_t * var){
	if(var->data_type == INTEGER){
		list_insert("INT2FLOATS ",NULL, NULL, NULL);
	}
	else if(var->data_type == DOUBLE){
		list_insert("FLOAT2R2EINTS ",NULL, NULL, NULL);
	}
}

void substr(){
	list_insert("CREATEFRAME ",NULL, NULL, NULL);
	list_insert("PUSHFRAME ",NULL, NULL, NULL);
	variable_t * ret = create_var("RETURN ", false);
	variable_t * tmp = create_var("PRINT ", false);
	variable_t * jedna = create_var("INT@1", true);
	variable_t * nula = create_var("INT@0", true);
	variable_t * s = create_var("ASC ", false);
	variable_t * i = create_var("NEXT ", false);
	variable_t * n = create_var("SUBSTR ", false);
	variable_t * b = create_var("BOOLEAN ", false);
	variable_t * btrue = create_var("bool@true", true);


	static unsigned substr_counter;
	substr_counter++;
	char *name = gen_label_name(substr_counter, 'S');
	variable_t * zero = create_var(name, true); 
	substr_counter++;
	name = gen_label_name(substr_counter, 'S');
	variable_t * end = create_var(name, true); 
	substr_counter++;
	name = gen_label_name(substr_counter, 'S');
	variable_t * change = create_var(name, true); 
	substr_counter++;
	name = gen_label_name(substr_counter, 'S');
	variable_t * normal = create_var(name, true); 

	list_insert("DEFVAR ",ret, NULL, NULL);
	list_insert("DEFVAR ",tmp, NULL, NULL);

	list_insert("DEFVAR ",s, NULL, NULL);
	list_insert("DEFVAR ",i, NULL, NULL);
	list_insert("DEFVAR ",n, NULL, NULL);
	list_insert("DEFVAR ",b, NULL, NULL);

	
	variable_t * var = create_var("STRING@", true);
	list_insert("MOVE ", ret, var, NULL);

	list_insert("POPS ", n, NULL, NULL);
	list_insert("POPS ", i, NULL, NULL);
	list_insert("POPS ", s, NULL, NULL);

	list_insert("JUMPIFEQ ", end, n, nula);
	// condition jump
	list_insert("STRLEN ", tmp, s, NULL);
	list_insert("JUMPIFEQ ", zero ,tmp, nula);
	list_insert("GT ", b, i, nula);
	list_insert("NOT ", b, b, NULL);
	list_insert("JUMPIFEQ ", zero, b, btrue);

	list_insert("SUB ", tmp, tmp, i);
	list_insert("GT ", b, n, tmp);
	list_insert("JUMPIFEQ ", change ,b, btrue);
	
	list_insert("LT ",b ,n, nula);
	list_insert("JUMPIFEQ ", change ,b, btrue);
	
	list_insert("JUMP ", normal, NULL, NULL);
	// jump normal 

	//label change
	list_insert("LABEL ", change, NULL, NULL);
	list_insert("MOVE ", n, tmp, NULL);
	list_insert("ADD ", n, n, jedna);

	// label normal
	
	list_insert("LABEL ", normal, NULL, NULL);
	list_insert("SUB ", i, i, jedna);

	substr_counter++;
	name = gen_label_name(substr_counter, 'S');
	var = create_var(name, true);
	list_insert("LABEL ", var, NULL, NULL);
	list_insert("GETCHAR ",tmp, s, i);
	list_insert("CONCAT ", ret, ret, tmp);
	list_insert("SUB ", n, n, jedna);
	list_insert("ADD ", i, i, jedna);
	list_insert("JUMPIFNEQ ", var, n, nula);
	list_insert("JUMP ", end, NULL, NULL);
	// JUMP END

	// LABEL NULA
	list_insert("LABEL ", zero, NULL, NULL);
	var = create_var("STRING@", true);
	list_insert("MOVE ", ret, var, NULL);

	// LABEL END
	list_insert("LABEL ", end, NULL, NULL);
	list_insert("PUSHS ",ret, NULL, NULL);
	list_insert("POPFRAME ",NULL, NULL, NULL);
}

void asc (variable_t * l_value){
	list_insert("CREATEFRAME ",NULL, NULL, NULL);
	list_insert("PUSHFRAME ",NULL, NULL, NULL);
	variable_t * tmp = create_var("RETURN ", false);
	variable_t * s = create_var("STRING ", false);
	variable_t * b = create_var("BOOLEAN ", false);
	variable_t * i = create_var("NEXT ", false);
	variable_t * jedna = create_var("INT@1", true);
	variable_t * nula = create_var("INT@0", true);
	variable_t * btrue = create_var("bool@true", true);

	static unsigned asc_counter;
	asc_counter++;
	char *name = gen_label_name(asc_counter, 'A');
	variable_t * zero = create_var(name, true);
	asc_counter++;
	name = gen_label_name(asc_counter, 'A');
	variable_t * end = create_var(name, true);
	list_insert("DEFVAR ",tmp, NULL, NULL);
	list_insert("DEFVAR ",s, NULL, NULL);
	list_insert("DEFVAR ",b, NULL, NULL);
	list_insert("DEFVAR ",i, NULL, NULL);

	list_insert("POPS ", i, NULL, NULL);
	list_insert("POPS ", s, NULL, NULL);

	list_insert("GT ", b, i, nula);
	list_insert("NOT ", b, b, NULL);
	list_insert("JUMPIFEQ ", zero, b, btrue);

	list_insert("SUB ", i, i, jedna);
	list_insert("STRLEN ", tmp, s, NULL);
	list_insert("LT ", b, i, tmp);
	list_insert("NOT ", b, b, NULL);
	list_insert("JUMPIFEQ ", zero, b, btrue);
	list_insert("STRI2INT ", tmp, s, i);
	list_insert("JUMP ", end, NULL, NULL);
	list_insert("LABEL ", zero, NULL, NULL);
	list_insert("MOVE ", tmp, nula, NULL );
	list_insert("LABEL ", end, NULL, NULL);
	list_insert("PUSHS ", tmp, NULL, NULL);


	if(l_value && l_value->data_type == DOUBLE)
		list_insert("INT2FLOATS ",NULL, NULL, NULL);
	list_insert("POPFRAME ",NULL, NULL, NULL);

}

void chr(){

	list_insert("CREATEFRAME ",NULL, NULL, NULL);
	list_insert("PUSHFRAME ",NULL, NULL, NULL);
	list_insert("INT2CHARS", NULL, NULL, NULL);
	list_insert("POPFRAME ",NULL, NULL, NULL);

}

void print_list(){
	printf(".IFJcode17\nJUMP SCOPE\n");

	for(instruction_t * tmp = list->First; tmp!=NULL; tmp=tmp->next){
        if (tmp->instr_name) printf("%s",tmp->instr_name);  /// print instruction name
        if(tmp->op1){
            if(tmp->op1->constant){ /// print var as constant 
                switch (tmp->op1->data_type){
                    case INTEGER:printf("int@%d",tmp->op1->data.i);break;
                    case DOUBLE:printf("float@%g",tmp->op1->data.d);break;
                    case STRING:printf("string@");
                                process_string(tmp->op1->data.str); break;
                    default: if(tmp->op1->data.str)printf("%s", tmp->op1->data.str);break;  /// print only stored string as constant
                }
            }
            else if(tmp->op1->data.str) printf("LF@%s ", tmp->op1->data.str); /// print name of variable this way: LF@var_name
        }
       
        if(tmp->op2){
            if(tmp->op2->constant){
                switch (tmp->op2->data_type){
                    case INTEGER:printf("int@%d",tmp->op2->data.i);break;
                    case DOUBLE:printf("float@%g",tmp->op2->data.d);break;
                    case STRING:printf("string@");
                                process_string(tmp->op2->data.str); break;
                    default: if(tmp->op2->data.str)printf("%s", tmp->op2->data.str);break;
                }
            }
            else  if(tmp->op2->data.str) printf("LF@%s ", tmp->op2->data.str);
        }
        if(tmp->op3){
            if(tmp->op3->constant){
                switch (tmp->op3->data_type){
                    case INTEGER:printf("int@%d",tmp->op3->data.i);break;
                    case DOUBLE:printf("float@%g",tmp->op3->data.d);break;
                    case STRING:printf("string@");
                                process_string(tmp->op3->data.str); break;
                    default: if(tmp->op3->data.str)printf("%s", tmp->op3->data.str);break;
                }
            }
            else if(tmp->op3->data.str)  printf("LF@%s ", tmp->op3->data.str);
        }
        printf("\n");
    }
}