#include "mcro.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "instructions.h"

/*
 * Function to find the location of a macro in the macro table
 * This function searches for the macro name in the table and returns its position if found, otherwise returns -1.
 *
 * Parameters:
 *   word  - The name of the macro to search for.
 *   table - The table that contains all the macros.
 *
 * Returns:
 *   The position of the macro in the table if found.
 *   -1 if the macro is not found in the table.
 */
int find_mcro_in_table(char *word, const mcro_table *table)
{
    int i;
    for (i = 0; i < table->count; i++)
    {
        if (!strcmp(word, table->macros[i].mcro_name))
        {
            return i;
        }
    }
    return -1;
}

/*
 * Preprocessor function for processing a source file and replacing macros with their content.
 * The function reads the source file, finds macros, and replaces them with their content in a new output file.
 *
 * Parameters:
 *   file_name        - The base name of the file (without extension) to read from.
 *   table            - The macro table, which stores all the macros defined.
 *   there_is_an_error- A flag indicating if there was an error during processing.
 *
 * Returns:
 *   0 - If processing was successful.
 *   1 - If there was an error during processing.
 */
int preprocessor(const char *file_name, mcro_table *table, int *there_is_an_error)
{
    char line[81]; /* Input line buffer for reading lines from the file */
    char word[80]; /* Word buffer for extracting individual words from each line */

    /* Allocate memory for file names with extensions */
    char *sorce_file_name = malloc(strlen(file_name) + 4);
    char *des_file_name = malloc(strlen(file_name) + 4);

    /* Build file names */
    sprintf(sorce_file_name, "%s.as", file_name);
    sprintf(des_file_name, "%s.am", file_name);

    mcro *temp = NULL;
    int line_length, position, i, j, in_mcro = 0; /* Variables for processing macros */
    FILE *sorce, *destination;

    /* Open the source file for reading */
    sorce = fopen(sorce_file_name, "r");
    if (sorce == NULL)
    {
        printf("Error: cannot open %s\n", sorce_file_name);
        return 1;
    }

    /* Open the destination file for writing */
    destination = fopen(des_file_name, "w");
    if (destination == NULL)
    {
        printf("Error: cannot open %s\n", des_file_name);
        fclose(sorce);
        return 1;
    }

    /* Read each line from the source file */
    while (fgets(line, sizeof(line), sorce))
    {
        line_length = strlen(line);
        sscanf(line, "%s", word); /* Extract the first word from the line */

        /* If we are inside a macro definition */
        if (in_mcro)
        {
            /* If we find the end of the macro definition */
            if (!strcmp(word, "mcroend"))
            {
                in_mcro = 0; /* Exit macro definition mode */
                table->count++;
                continue;
            }
            else
            {
                /* Continue adding lines to the macro definition */
                table->macros[table->count].mcro_content[table->macros[table->count].line_count] = (char *)malloc(line_length + 1);
                if (table->macros[table->count].mcro_content[table->macros[table->count].line_count] == NULL)
                {
                    printf("Memory allocation failed in preprocessor\n");
                    return 1;
                }
                strcpy(table->macros[table->count].mcro_content[table->macros[table->count].line_count], line);
                table->macros[table->count].line_count++;
            }
        }
        else
        {
            /* Macro definition */
            if (!strcmp(word, "mcro"))
            {
                sscanf(line, "%*s%s", word); /* Extract the macro name */
                position = find_mcro_in_table(word, table);

                /* Check if the macro already exists in the table */
                if (position != -1)
                {
                    printf("Error: You cannot declare two macros with the same name\n");
                    *there_is_an_error = 1;
                    return 1;
                }

                /* Check if the macro name conflicts with an instruction or directive name */
                else if (find_instruction_name(word) != -1 || find_data_instruction(word) != -1)
                {
                    printf("Error in file %s on mcro name %s: A macro name cannot be the same as an instruction or directive name.\n", sorce_file_name, word);
                    *there_is_an_error = 1;
                    continue;
                }

                in_mcro = 1; /* Enter macro definition mode */

                /* Allocate additional memory for the macro table */
                temp = (mcro *)realloc(table->macros, (table->count + 1) * sizeof(mcro));
                if (temp == NULL)
                {
                    printf("Memory allocation failed in preprocessor\n");
                    remove(des_file_name);
                    fclose(sorce);
                    fclose(destination);
                    return 1;
                }
                table->macros = temp;

                /* Save the macro name */
                sscanf(word, "%s", table->macros[table->count].mcro_name);
                table->macros[table->count].line_count = 0;
                continue;
            }

            /* Check if the word is a macro name */
            position = find_mcro_in_table(word, table);
            if (position == -1)
            {
                /* If it's not a macro, just write the line to the destination file */
                fprintf(destination, "%s", line);
            }
            else
            {
                /* If it's a macro, replace the macro name with its content */
                for (i = 0; i < table->macros[position].line_count; i++)
                {
                    fprintf(destination, "%s", table->macros[position].mcro_content[i]);
                }
            }
        }
    }

    /* If there was an error, remove the generated destination file */
    if (*there_is_an_error)
    {
        remove(des_file_name);
    }

    /* Close the files */
    fclose(sorce);
    fclose(destination);

    /* Free the memory allocated for the file names */
    free(sorce_file_name);
    free(des_file_name);

    return 0;
}
