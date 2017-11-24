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

#define LENGTH 120

list_t * list;

char * gen_label_name(int i, char c){
	unsigned digits	= floor(log10(abs(i)))+1;
	char * name = (char *) malloc(digits*sizeof(int)+3);
	if(!name)
		print_err(99);
	name[0] = '&';
	name[1] = c;
	sprintf(name+2, "%d", i);
	return name;
}

void free_var(variable_t * v){
	if(v->data.str)
		free(v->data.str);
	if(v)
		free(v);
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
	//unsigned length_of_instr = (str2)? strlen(str2):0;
	tmp->data.str = (char*)malloc(sizeof(char)*(strlen(str1)+1));
 	if(!tmp->data.str)
 		print_err(99);
	strcpy(tmp->data.str, str1);
	strcat(tmp->data.str, " ");
	tmp->constant = constant;
	return tmp;
}

/*variable_t * create_var(char *string){
	variable_t * tmp = init_variable();
	tmp->data.str = (char*)malloc(sizeof(char)*(4+strlen(string)));
 		if(!tmp->data.str)
 		 	print_err(99);
	strcpy(tmp->data.str);
	strcat(tmp->data.str, string);
	return tmp;
}
*/
void concat()
{
	//static unsigned conc_counter;
	//conc_counter++;
	//char *name = gen_label_name(conc_counter, 'C');
	//variable_t * tmp1 = create_var(name, false); free(name);
	//conc_counter++;
	//name = gen_label_name(conc_counter, 'C');
	//variable_t * tmp2 = create_var(name, false); free(name);
	//conc_counter++;
	//name = gen_label_name(conc_counter, 'C');
	//variable_t * tmp3 = create_var(name, false); free(name);


	variable_t * tmp1 = create_var("&C1 ", false);
	variable_t * tmp2 = create_var("&C2 ", false);
	//variable_t * tmp3 = create_var("ASC ", false);
	

	list_insert("CREATEFRAME ",NULL, NULL, NULL);
	list_insert("PUSHFRAME ",NULL, NULL, NULL);
	list_insert("DEFVAR ", tmp1, NULL, NULL);
	list_insert("DEFVAR ", tmp2, NULL, NULL);
	//list_insert("DEFVAR ", tmp3, NULL, NULL);


	list_insert("POPS ", tmp1, NULL, NULL);
	list_insert("POPS ", tmp2, NULL, NULL);

	list_insert("CONCAT ",tmp2, tmp2, tmp1);
	list_insert("PUSHS ",tmp2, NULL, NULL);
	list_insert("POPFRAME ",NULL, NULL, NULL);


	free_var(tmp1);
	free_var(tmp2);
	//free_var(tmp3);
}
instruction_t * instr_init (){
	instruction_t * new =  (instruction_t *)malloc(sizeof(instruction_t));
	if(!new)
		print_err(99);
	new->instr_name = (char *) calloc(15,sizeof(char));
	//memset(new->instr_name, '\0', sizeof(new->instr_name));
	new->op1 = new->op2 = new->op3 = NULL;
	new->next = NULL;

	return new;
}

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
		
		else{
			//int znak = (int)orig_string[i];
			printf("\\%03d", (int)((unsigned char)orig_string[i]));			
		}
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
	//list_insert("MOVE  ", tmp, var, NULL);
	list_insert("POPS ", str, NULL, NULL);

	list_insert("STRLEN ",tmp, str, NULL);


	list_insert("PUSHS ",tmp, NULL, NULL);

	
	if(l_value && l_value->data_type == DOUBLE)
		list_insert("INT2FLOATS ",NULL, NULL, NULL);
	list_insert("POPFRAME ",NULL, NULL, NULL);
	free_var(tmp);
	free_var(str);
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
	variable_t * zero = create_var(name, true); free(name);
	substr_counter++;
	name = gen_label_name(substr_counter, 'S');
	variable_t * end = create_var(name, true); free(name);
	substr_counter++;
	name = gen_label_name(substr_counter, 'S');
	variable_t * change = create_var(name, true); free(name);
	substr_counter++;
	name = gen_label_name(substr_counter, 'S');
	variable_t * normal = create_var(name, true); free(name);

	/*
	variable_t * tmp = init_variable();
	tmp->data.str = (char *) malloc(8);
 		if(!tmp->data.str)
 		 	print_err(99);
	strcpy(tmp->data.str,"LF@RETURN ");*/

	//tmp->data_type=STRING;
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

	//list_insert("SUB ", i, i, jedna);
	//i->data.d-=1; // ma to indexovat od 1

	list_insert("JUMPIFEQ ", end, n, nula);
	// podmieneny skook
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
	// free	
	free_var(ret);
	free_var(tmp);
	free_var(jedna);
	free_var(nula);
	free_var(s);
	free_var(i);
	free_var(n);
	free_var(zero);
	free_var(change);
	free_var(normal);
	free_var(end);
	free_var(b);
	free_var(btrue);
	free_var(var);

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
variable_t * zero = create_var(name, true); free(name);
asc_counter++;
name = gen_label_name(asc_counter, 'A');
variable_t * end = create_var(name, true); free(name);
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

free_var(tmp);
free_var(s);
free_var(b);
free_var(i);
free_var(jedna);
free_var(nula);
free_var(btrue);
free_var(zero);
free_var(end);
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
        if (tmp->instr_name) printf("%s",tmp->instr_name); 
        if(tmp->op1){
            if(tmp->op1->constant){
                switch (tmp->op1->data_type){
                    case INTEGER:printf("int@%d",tmp->op1->data.i);break;
                    case DOUBLE:printf("float@%g",tmp->op1->data.d);break;
                    case STRING:printf("string@");
                                process_string(tmp->op1->data.str); break;
                    default: if(tmp->op1->data.str)printf("%s", tmp->op1->data.str);break;
                }
            }
            else if(tmp->op1->data.str) printf("LF@%s ", tmp->op1->data.str);
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

/*CREATEFRAME
DEFVAR TF@RETURN
DEFVAR TF@STRING
DEFVAR TF@INDEX
DEFVAR TF@BOOL
MOVE TF@STRING STRING@AHOJ
MOVE TF@INDEX INT@0

JUMPIFEQ NULA TF@INDEX INT@0

SUB TF@INDEX TF@INDEX INT@1
STRLEN TF@RETURN TF@STRING

LT TF@BOOL TF@INDEX TF@RETURN
NOT TF@BOOL TF@BOOL
JUMPIFEQ NULA TF@BOOL bool@true
STRI2INT TF@RETURN TF@STRING TF@INDEX

JUMP KONEC

LABEL NULA
MOVE TF@RETURN INT@0

LABEL KONEC
PUSHS TF@RETURN
WRITE TF@RETURN

*/



//int main(){

//char test_string [1000] = "Volam sa MAtus \nTOTO je text\n";
//a dnes by som rad povedal \nze mam rad vonu cerstvo upeceneho jablkoveho kolaca\n";

//length_of_str(list,test_string);
//substr(list, test_string, 1, 10);

// print TEXT
/*if(!(list_insert(list, ".IFJcode17\n"))){
	//print_err(99);
}
if(!(list_insert(list, "WRITE string@"))){
	//print_err(99);
}
str_t * retazec = process_string(test_string);
if(!(list_insert(list, retazec->string))){
	//print_err(99);
}*/

//for(str_t* tmp = list->First; tmp!=NULL; tmp=tmp->next)
	//fprintf(fw, "%s", tmp->string); 
//	printf("%s", tmp->string); 

//fprintf(fw,"\n");
//printf("\n");

// + free items of list (missing)
//free (list);
//free (retazec->string);
//free (retazec);
//}
