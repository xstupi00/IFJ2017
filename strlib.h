#ifndef STR_LIB_H
#define STR_LIB_H

#define STR_INIT 128

typedef struct {
	char * string;
	size_t capacity; 
	size_t length;
}T_string;

T_string * strInit();
bool extendStr(T_string * str, size_t new_size);


#endif