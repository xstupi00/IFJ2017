###################################################################################
## School:      Brno University of Technology, Faculty of Information Technology ##
## Course:      Formal Languages and Compilers                                   ##
## Project:     IFJ17                                                            ##
## Module:      Makefile                                                		 ##
## Authors:     Kristián Liščinský  (xlisci01)                                   ##
##              Matúš Liščinský     (xlisci02)                                   ##
##              Šimon Stupinský     (xstupi00)                                   ##
##              Vladimír Marcin     (xmarci10)                                   ##
###################################################################################
CC= gcc
CCFLAGS= -std=c99 -Wall -Wextra -pedantic
LDLIBS= -lm
EXECUTABLE= ifj17
REMOVE= rm -f
SRC= $(wildcard *.c) 
OBJ= $(SRC:%.c=%.o)

.PHONY: all debug test clean 

all: $(EXECUTABLE)

debug: CCFLAGS += -DDEBUG -g
debug: $(EXECUTABLE)

$(EXECUTABLE): $(OBJ)
	$(CC) $(CCFLAGS) $^ -o $@ $(LDLIBS)

error.o: error.c error.h clear.h
	$(CC) $(CCFLAGS) -c $< -o $@
expr.o: expr.c scanner.h error.h expr.h stack.h symtable.h semantic_control.h generate.h strlib.h clear.h
	$(CC) $(CCFLAGS) -c $< -o $@
generate.o: generate.c generate.h error.h semantic_control.h scanner.h clear.h
	$(CC) $(CCFLAGS) -c $< -o $@
main.o:	main.c parser.h generate.h semantic_control.h clear.h
	$(CC) $(CCFLAGS) -c $< -o $@
parser.o: parser.c parser.h scanner.h error.h expr.h semantic_control.h strlib.h generate.h stack.h
	$(CC) $(CCFLAGS) -c $< -o $@
scanner.o: scanner.c scanner.h error.h strlib.h clear.h
	$(CC) $(CCFLAGS) -c $< -o $@
semantic_control.o: semantic_control.c semantic_control.h stack.h symtable.h error.h scanner.h generate.h clear.h
	$(CC) $(CCFLAGS) -c $< -o $@
stack.o: stack.c stack.h error.h clear.h
	$(CC) $(CCFLAGS) -c $< -o $@
strlib.o: strlib.c strlib.h error.h clear.h
	$(CC) $(CCFLAGS) -c $< -o $@
symtable.o: symtable.c symtable.h error.h scanner.h semantic_control.h clear.h
	$(CC) $(CCFLAGS) -c $< -o $@
clear.o: clear.c clear.h stack.h error.h
	$(CC) $(CCFLAGS) -c $< -o $@

test: $(EXECUTABLE)
test:
	python tests/tests.py

clean:
	$(REMOVE) $(OBJ) $(EXECUTABLE)




