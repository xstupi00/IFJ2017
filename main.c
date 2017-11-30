///////////////////////////////////////////////////////////////////////////////////
// School:      Brno University of Technology, Faculty of Information Technology //
// Course:      Formal Languages and Compilers                                   //
// Project:     IFJ17                                                            //
// Module:      Main module                                                      //
// Authors:     Kristián Liščinský  (xlisci01)                                   //
//              Matúš Liščinský     (xlisci02)                                   //
//              Šimon Stupinský     (xstupi00)                                   //
//              Vladimír Marcin     (xmarci10)                                   //
///////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include "parser.h"
#include "generate.h"
#include "semantic_control.h"
#include "clear.h"


int main(){

    init_globals();
    parse();
    print_list();

    clear_all();
    free(ptr_stack);
    return 0;
}