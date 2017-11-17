#ifndef STR_LIB_H
#define STR_LIB_H

#define STR_INIT 128

#include <stdlib.h>

typedef struct {
	char * string;
	size_t capacity; 
	size_t length;
}string_t;

string_t * strInit(size_t size);
bool extendStr(string_t * str, size_t new_size);


#endif