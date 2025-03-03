my_project: main.o preprocessor.o first_pass.o instruction.o
	gcc -ansi -pedantic -Wall main.o preprocessor.o first_pass.o instruction.o -o my_project
main.o: main.c defines.h
	gcc -c -ansi -pedantic -Wall main.c -o main.o
preprocessor.o: preprocessor.c defines.h
	gcc -c -ansi -pedantic -Wall preprocessor.c -o preprocessor.o	
first_pass.o: first_pass.c defines.h
	gcc -c -ansi -pedantic -Wall first_pass.c -o first_pass.o
instruction.o: instruction.c defines.h
	gcc -c -ansi -pedantic -Wall instruction.c -o instruction.o	