#include <stdlib.h>
#include <stdbool.h>
#include "strlib.h"

bool extendStr(string_t * str, size_t new_size){
    if( (str->string = (char *)(realloc(str->string, sizeof(char)*new_size+1))) == NULL) 
    	return false;
    str->capacity=new_size;
    return true;
}


string_t * strInit(size_t size){
	string_t * str = (string_t *) malloc(sizeof(string_t));
	if(!str)
		return NULL;
	str->string = (char *) malloc(sizeof(char) * size + 1);
	if(!(str->string))
		return NULL;
	str->capacity = size;
	str->length = 0;
	return str;
} 
