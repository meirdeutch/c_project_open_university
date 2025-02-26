my_project: main.o preprocessor.o
	gcc -ansi -pedantic -Wall main.o preprocessor.o -o my_project
main.o: main.c defines.h
	gcc -c -ansi -pedantic -Wall main.c -o main.o
preprocessor.o: preprocessor.c defines.h
	gcc -c -ansi -pedantic -Wall preprocessor.c -o preprocessor.o	

