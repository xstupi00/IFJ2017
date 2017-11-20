#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include "generate.h"
#include "error.h"
#include "symtable.h"
#include "semantic_control.h"
#include "scanner.h"
#define LENGTH 120

list_t * list;

void list_init()
{
	list = (list_t*) malloc(sizeof(list_t));
	if(!list)
		print_err(99);
	list->First=NULL;
	list->Last=NULL;
}


variable_t * create_var(char *str1, char * str2){
	variable_t * tmp = init_variable();
	unsigned length_of_instr = (str2)? strlen(str2):0;
	tmp->data.str = (char*)malloc(sizeof(char)*(length_of_instr+strlen(str1)+1));
 		if(!tmp->data.str)
 		 	print_err(99);
 	strcpy(tmp->data.str, (str2)? str2:"");
	strcat(tmp->data.str, str1);
	strcat(tmp->data.str, " ");
	//tmp->constant = true;
	return tmp;
}

/*variable_t * create_var(char *string){
	variable_t * tmp = init_variable();
	tmp->data.str = (char*)malloc(sizeof(char)*(4+strlen(string)));
 		if(!tmp->data.str)
 		 	print_err(99);
	strcpy(tmp->data.str, "LF@");
	strcat(tmp->data.str, string);
	return tmp;
}
*/
void concat()
{
	variable_t * tmp1 = create_var("NEXT ", "LF@");
	variable_t * tmp2 = create_var("SUBSTR ", "LF@");
	variable_t * tmp3 = create_var("ASC ", "LF@");
	
	list_insert("POPS ", tmp1, NULL, NULL);
	list_insert("POPS ", tmp2, NULL, NULL);

	list_insert("CONCAT ",tmp3, tmp2, tmp1);
	list_insert("PUSHS ",tmp3, NULL, NULL);

	/*free(tmp->data.str);
	free(tmp);
*/

}
instruction_t * instr_init (){
	instruction_t * new =  (instruction_t *)malloc(sizeof(instruction_t));
	if(!new)
		print_err(99);
	memset(new->instr_name, '\0', sizeof(new->instr_name));

	new->op1 = init_variable();
	new->op2 = init_variable();
	new->op3 = init_variable();
	new->next = NULL;

	return new;
}

variable_t * copy_variable(variable_t * src){
	variable_t * new = init_variable();

	new->data_type = src->data_type;
	switch(new->data_type){
		case INTEGER: new->data.i = src->data.i;break;
		case DOUBLE: new->data.d  = src->data.d;break;
		default: new->data.str = (char *) malloc(sizeof(char)*strlen(src->data.str));
 					 if(!new->data.str)
 					 	print_err(99);
 					 strcpy(new->data.str, src->data.str);break;
	}
	//new->constant = src->constant;
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

	for(unsigned i = 0; orig_string[i]!='\0' ; i++){
		if(isalpha(orig_string[i]) || isdigit(orig_string[i]))
			printf("%c",orig_string[i]);		
		
		else{
			int znak =(int)orig_string[i];
			printf("\\%03u", znak);
		}
	}	
	printf(" ");
}


void length_of_str(variable_t * var){

	list_insert("CREATEFRAME ",NULL, NULL, NULL);
	list_insert("PUSHFRAME ",NULL, NULL, NULL);
	variable_t * tmp = create_var("RETURN ", "LF@");
	/*
	variable_t * tmp = init_variable();
	tmp->data.str = (char *) malloc(8);
 		if(!tmp->data.str)
 		 	print_err(99);
	strcpy(tmp->data.str,"LF@RETURN ");*/

	//tmp->data_type=STRING;
	list_insert("DEFVAR ",tmp, NULL, NULL);

	list_insert("STRLEN ",tmp, var, NULL);
	list_insert("PUSHS ",tmp, NULL, NULL);
	free(tmp->data.str);
	free(tmp);
}


void retype(variable_t * var){
	if(var->data_type == INTEGER){
		list_insert("INT2FLOATS ",NULL, NULL, NULL);
	}
	else if(var->data_type == DOUBLE){
		list_insert("FLOAT2INTS ",NULL, NULL, NULL);
	}
}

void substr(variable_t * s, variable_t * i, variable_t * n){

	list_insert("CREATEFRAME ",NULL, NULL, NULL);
	list_insert("PUSHFRAME ",NULL, NULL, NULL);
	variable_t * ret = create_var("RETURN ", "LF@");
	variable_t * tmp = create_var("PRINT ", "LF@");
	variable_t * jedna = create_var("INT@1", NULL);
	variable_t * nula = create_var("INT@0", NULL);
	/*
	variable_t * tmp = init_variable();
	tmp->data.str = (char *) malloc(8);
 		if(!tmp->data.str)
 		 	print_err(99);
	strcpy(tmp->data.str,"LF@RETURN ");*/

	//tmp->data_type=STRING;
	list_insert("DEFVAR ",ret, NULL, NULL);
	list_insert("DEFVAR ",tmp, NULL, NULL);
	list_insert("DEFVAR ",jedna, NULL, NULL);
	list_insert("DEFVAR ",nula, NULL, NULL);
	
	variable_t * var = create_var("STRING@", NULL);
	list_insert("MOVE ", ret, var, NULL);

	list_insert("POPS ", n, NULL, NULL);
	list_insert("POPS ", i, NULL, NULL);
	list_insert("POPS ", s, NULL, NULL);

	i->data.d-=1; // ma to indexovat od 1

	var = create_var("FOR ", NULL);
	list_insert("LABEL ", var, NULL, NULL);
	list_insert("GETCHAR ",tmp, s, i);
	list_insert("CONCAT ", ret, ret, tmp);
	list_insert("SUB ", n, n, jedna);
	list_insert("ADD ", i, i, jedna);
	list_insert("JUMPIFNEQ ", var, n, nula);
	

	list_insert("PUSHS ",ret, NULL, NULL);
	
	// free	
	free(ret->data.str);
	free(ret);
	free(tmp->data.str);
	free(tmp);
	free(jedna->data.str);
	free(jedna);
	free(nula->data.str);
	free(nula);

}
/*
DEFVAR LF@RET // ret
DEFVAR LF@STR // s
DEFVAR LF@TMP // tmp
DEFVAR LF@COUNTER // n
DEFVAR LF@POS  // i

MOVE LF@STR STRING@ABCDEFGHIJ # ABCDEFGHIJ-string
MOVE LF@COUNTER INT@4
MOVE LF@POS INT@2
MOVE LF@RET STRING@

LABEL FOR
GETCHAR LF@TMP LF@STR LF@POS
CONCAT LF@RET LF@RET LF@TMP
SUB LF@COUNTER LF@COUNTER INT@1
ADD LF@POS LF@POS INT@1
JUMPIFNEQ FOR LF@COUNTER INT@0

WRITE LF@RET
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
