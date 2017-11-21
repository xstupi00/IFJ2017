#include <stdlib.h>
#include <stdbool.h>
#include "strlib.h"
#include "error.h"

bool extendStr(string_t * str, size_t new_size){
    if( !(str->string = (char *)(realloc(str->string, sizeof(char)*new_size+1))) ) 
		print_err(99);
    str->capacity=new_size;
    return true;
}


string_t * strInit(size_t size){
	string_t * str = (string_t *) malloc(sizeof(string_t));
	if(!str)
		print_err(99);
	str->string = (char *) malloc(sizeof(char) * size + 1);
	if(!(str->string))
		print_err(99);
	str->capacity = size;
	str->length = 0;
	return str;
} 

void free_string(string_t * s){
	if(s->string)
		free(s->string);
	if(s)
		free(s);
}
