///////////////////////////////////////////////////////////////////////////////////
// School:      Brno University of Technology, Faculty of Information Technology //
// Course:      Formal Languages and Compilers                                   //
// Project:     IFJ17                                                            //
// Module:      Work with strings                                                //
// Authors:     Kristián Liščinský  (xlisci01)                                   //
//              Matúš Liščinský     (xlisci02)                                   //
//              Šimon Stupinský     (xstupi00)                                   //
//              Vladimír Marcin     (xmarci10)                                   //
///////////////////////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include <stdbool.h>
#include "strlib.h"
#include "error.h"
#include "clear.h"

/// replace malloc & realloc calls with our malloc & realloc wrapper
#define malloc(size) _malloc(size)
#define realloc(ptr, new_size, old_size) _realloc(ptr, new_size, old_size)

bool extendStr(string_t * str, size_t new_size){
    if( !(str->string =(char *)(realloc(str->string, sizeof(char)*new_size+1, str->capacity+1))) ) 
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