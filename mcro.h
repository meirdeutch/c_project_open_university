#ifndef MCRO_H
#define MCRO_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* 
 * Structure representing a macro.
 * Contains the macro name, its content (multiple lines), and line count.
 */
typedef struct mcro
{
    char mcro_name[31];         /* The macro name (up to 30 characters + null terminator) */
    char *mcro_content[81];     /* Array of pointers for each line of macro content (max 81 lines) */
    int line_count;             /* Number of lines in the macro content */
} mcro;

/* 
 * Structure representing a table of macros.
 * Contains a pointer to an array of macros and the number of macros in the table.
 */
typedef struct mcro_table
{
    mcro *macros;    /* Pointer to dynamically allocated array of macros */
    int count;       /* Number of macros in the table */
} mcro_table;

/*
 * Function to find a macro in the macro table by its name.
 * Returns the index of the macro if found, otherwise returns -1.
 */
int find_mcro_in_table(char *word, const mcro_table *table);

/*
 * Preprocessor function to read the source file and replace macros with their content.
 * Writes the processed content to a destination file.
 */
int preprocessor(const char *file_name, mcro_table *table, int *there_is_an_error);

#endif  /* MCRO_H */
