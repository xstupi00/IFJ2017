#include "strlib.h"

bool extendStr(T_string * str, size_t new_size){
    if( (str->string = (char *)(realloc(str->string, sizeof(char)*new_size+1))) == NULL) 
    	return false;
    str->capacity=new_size;
    return true;
}


T_string * strInit(){
	T_string * str = (T_string *) malloc(sizeof(T_string));
	if(!str)
		return NULL;
	str->string = (char *) malloc(sizeof(char)*STR_INIT+1);
	if(!(str->string))
		return NULL;
	str->capacity=STR_INIT;
	str->length=0;
	return str;
} 
