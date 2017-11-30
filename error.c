///////////////////////////////////////////////////////////////////////////////////
// School:      Brno University of Technology, Faculty of Information Technology //
// Course:      Formal Languages and Compilers                                   //
// Project:     IFJ17                                                            //
// Module:      Error states	                                                 //
// Authors:     Kristián Liščinský  (xlisci01)                                   //
//              Matúš Liščinský     (xlisci02)                                   //
//              Šimon Stupinský     (xstupi00)                                   //
//              Vladimír Marcin     (xmarci10)                                   //
///////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include "error.h"

void print_err(int id)
{
	switch(id){
		case 1: fprintf(stderr,"Error in the program associated with lexical analysis\n"); break;
		case 2: fprintf(stderr,"Error in the program associated with syntax analysis\n"); break;
		case 3: fprintf(stderr,"Semantic error in the program\n"); break;
		case 4: fprintf(stderr,"Semantic type compatibility error\n"); break;
		case 6: fprintf(stderr,"Others semantic error\n"); break;
		case 99: fprintf(stderr,"Internal error of compiler\n"); break;
	}
	exit(id);
}