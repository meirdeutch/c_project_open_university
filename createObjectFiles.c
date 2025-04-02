#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "createObjectFiles.h"

/**
 * Creates an object file with the given instruction and data areas.
 * The object file will be named "<file_name>.ob" and will contain
 * the instructions and data in the specified format.
 *
 * @param file_name The name of the input file used for generating the object file.
 * @param instruction_area A pointer to an array containing the instructions.
 * @param data_area A pointer to an array containing the data.
 * @param ICF The instruction counter, indicating the total number of instructions.
 * @param DCF The data counter, indicating the total number of data entries.
 *
 * @return Returns 0 if the object file is created successfully, or 1 if there is an error opening the file.
 */
int create_object_file(const char *file_name, const int *instruction_area, const int *data_area, const int ICF, const int DCF){
    int i;
    char *object_file_name = malloc(strlen(file_name) + 4); /* Allocate memory for object file name */
    FILE *object_file;
    
    /* Create the object file name by appending ".ob" to the input file name */
    sprintf(object_file_name, "%s.ob", file_name);
    
    /* Open the object file for writing */
    object_file = fopen(object_file_name, "w");
    
    /* Check if the file was opened successfully */
    if (object_file == NULL){
        printf("Error: cannot open %s\n", object_file_name);
        return 1;  /* Return error code 1 if file cannot be opened */
    }

    /* Write the instruction count and data count to the object file */
    fprintf(object_file, "% 6d %d\n", ICF - 100, DCF);
    
    /* Write the instructions to the object file */
    for (i = 100; i < ICF; i++)
    {
        fprintf(object_file, "%06d ", i);                 /* Write the instruction address */
        fprintf(object_file, "%06x\n", instruction_area[i] & 0xffffff); /* Write the instruction in hexadecimal format */
        fprintf(object_file, "\n");
    }

    /* Write the data section header */
    fprintf(object_file, "%s\n", "data");
    
    /* Write the data to the object file */
    for (i = 0; i < DCF; i++)
    {
        fprintf(object_file, "%06d ", i + ICF);             /* Write the data address */
        fprintf(object_file, "%06x\n", data_area[i] & 0xffffff); /* Write the data in hexadecimal format */
        fprintf(object_file, "\n");
    }

    /* Close the object file */
    fclose(object_file);
    
    /* Free the allocated memory for the object file name */
    free(object_file_name);

    return 0;  /* Return 0 indicating successful file creation */
}
