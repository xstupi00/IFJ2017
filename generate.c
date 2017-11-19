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

variable_t * create_var(int i){
	char dig [4];
	for(int x = 2; x >= 0 ; x--) {
		dig[x]=(i%10)+'0';
		i/=10;
	}
	variable_t * tmp = init_variable();
	tmp->data.str = (char*)malloc(sizeof(char)*10);
 		if(!tmp->data.str)
 		 	print_err(99);
	strcpy(tmp->data.str, "LF@VAR");
	strcat(tmp->data.str, dig);
	return tmp;
}

void concat(variable_t * op1, variable_t * op2)
{
	variable_t * tmp = init_variable();
	tmp->data.str = (char *) malloc(8);
 		if(!tmp->data.str)
 		 	print_err(99);
	strcpy(tmp->data.str,"LF@CON ");
	//tmp->data_type=STRING;
	list_insert("DEFVAR ",tmp, NULL, NULL);

	list_insert("CONCAT ",tmp, op1, op2);
	free(tmp->data.str);
	free(tmp);


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
	if(list->Last){
		list->Last->next=new;
		list->Last=new;
	}
	else list->First=list->Last=new;

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
	variable_t * tmp = init_variable();
	tmp->data.str = (char *) malloc(8);
 		if(!tmp->data.str)
 		 	print_err(99);
	strcpy(tmp->data.str,"LF@RET ");
	//tmp->data_type=STRING;
	list_insert("DEFVAR ",tmp, NULL, NULL);

	list_insert("STRLEN ",tmp, var, NULL);
	free(tmp->data.str);
	free(tmp);
}

/*void push_param(list_t * list, int type, char * str)
{
	if(type == 48){	
		char in[strlen(str)+11];
		strcpy(in,"\nPUSHS int@");
		strcat(in,str);
		list_insert(list, in);
	}
	else if(type == 49){
		char db[strlen(str)+13];
		strcpy(db, "\nPUSHS float@");
		strcat(db,str);
		list_insert(list, db);
	}
	else {
		str_t * tmp = process_string(str);
		char st[strlen(tmp->string)+14];
		strcpy(st,"\nPUSHS string@");
		strcat(st,tmp->string);
		list_insert(list, st);
		free(tmp);			
	}
}

void pop_param(list_t * list, char * var_name){

	char intsr[strlen(var_name)+6];
	strcat(intsr,"POPS ");
	strcat(intsr,var_name);
	list_insert(list, intsr);
}

void create_symb(str_t * constant, int type){
	char * new_str = (char *)malloc(sizeof(char)*(strlen(constant->string)));
	if(!new_str)
		print_err(99);
	strcpy(new_str, constant->string);

	if(type == 48){	
		if(constant->capacity < (strlen(constant->string)+4))
			ext_str(constant,(strlen(constant->string)+4));
		memset(constant->string, '\0', sizeof(constant));
		strcpy(constant->string, "int@");
		
	}
	else if(type == 49){
		if(constant->capacity < (strlen(constant->string)+5))
			ext_str(constant,(strlen(constant->string)+5));
		memset(constant->string, '\0', sizeof(constant));
		strcpy(constant->string, "float@");
	}
	else {
		if(constant->capacity < (strlen(constant->string)+6))
			ext_str(constant,(strlen(constant->string)+6));
		memset(constant->string, '\0', sizeof(constant));
		strcpy(constant->string, "string@");
	}
	strcat(constant->string, new_str);
	free(new_str);
}


void substr(list_t * list, char *orig_string, int i, int n){
	list_insert(list, "CREATEFRAME\nPUSHFRAME\nDEFVAR LF@RET\nDEFVAR LF@STR\nDEFVAR LF@TMP\nDEFVAR LF@COUNTER\nDEFVAR LF@POS\nMOVE LF@RET STRING@\n");
	unsigned param_count = 3;
	char params[4]={'s','i','i'};
	char * par1 = orig_string;
	char * par2 = "4";
	char * par3 = "2"; 
	
	push_param(list, 50, par1);
	push_param(list, 48, par2);
	push_param(list, 48, par3);

	list_insert(list, "MOVE LF@RET STRING@\n");
	

	char stat1 [29] = "GETCHAR LF@TMP LF@STR INT@4\n";
	
	char stat2 [29] = "CONCAT LF@RET LF@RET LF@TMP\n";
	i=i-1; // indexovanie od 1
	for(int a = 0; a < n ; a++){
		stat1[26] = (i++)+'0';
		list_insert(list,stat1);
		list_insert(list, stat2);
	}
	if(!(list_insert(list, "PUSHS LF@RET"))){
		//print_err(99);
	}
	
}
*/
void retype(variable_t * var){
	if(var->data_type == INTEGER){
		list_insert("INT2FLOATS ",NULL, NULL, NULL);
	}
	else if(var->data_type == DOUBLE){
		list_insert("FLOAT2INTS ",NULL, NULL, NULL);
	}
}
/*
DEFVAR LF@RET
DEFVAR LF@STR
DEFVAR LF@TMP
DEFVAR LF@COUNTER
DEFVAR LF@POS

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

//FILE * fw = fopen("ppprog.code", "w");
//list_t *list = list_init();

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
