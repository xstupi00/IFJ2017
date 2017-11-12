#include "error.h"


void print_err(int id)
{
	switch(id){
		case 1: fprintf(stderr,"Chyba pri lexikalnej analyze\n");break;
		case 2: fprintf(stderr,"Chyba pri syntaktickej analyze\n");break;
		case 3: fprintf(stderr,"Sémantická chyba v programe\n");break;
		case 4: fprintf(stderr,"Sémantická chyba typovej kompatibility\n");break;
		case 6: fprintf(stderr,"Iná sémantická chyba\n");break;
		case 99: fprintf(stderr,"Interná chyba prekladača\n");break;
		default: fprintf(stderr,"ERR: volas funkciu so zlym parametrom\n"); // debug
	}
	exit(id);
}