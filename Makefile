all: testsymtablelist testsymtablehash

testsymtablelist: symtablelist.o testsymtable.o
	gcc217 symtablelist.o testsymtable.o -o testsymtablelist

testsymtablelist.o: testsymtablelist.c symtable.h
	gcc217 -c testsymtable.c

symtablelist.o: symtablelist.c symtable.h
	gcc217 -c symtablelist.c

testsymtablehash: symtablehash.o testsymtable.o
	gcc217 symtablehash.o testsymtable.o -o testsymtablehash

testsymtablelhash.o: testsymtablehash.c symtable.h
	gcc217 -c testsymtable.c

symtablehash.o: symtablehash.c symtable.h
	gcc217 -c symtablehash.c

