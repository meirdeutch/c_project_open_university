my_project: main.o preprocessor.o passes.o instruction.o createObjectFiles.o
	gcc -ansi -pedantic -Wall main.o preprocessor.o passes.o instruction.o createObjectFiles.o -o my_project
main.o: main.c instructions.h mcro.h passes.h createObjectFiles.h
	gcc -c -ansi -pedantic -Wall main.c -o main.o
preprocessor.o: preprocessor.c mcro.h instructions.h
	gcc -c -ansi -pedantic -Wall preprocessor.c -o preprocessor.o	
passes.o: passes.c instructions.h mcro.h passes.h
	gcc -c -ansi -pedantic -Wall passes.c -o passes.o
instruction.o: instruction.c instructions.h
	gcc -c -ansi -pedantic -Wall instruction.c -o instruction.o	
createObjectFiles.o: createObjectFiles.c createObjectFiles.h
	gcc -c -ansi -pedantic -Wall createObjectFiles.c -o createObjectFiles.o	
