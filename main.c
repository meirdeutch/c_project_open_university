#include <stdio.h>
#include <stdlib.h>
#include "instructions.h"
#include "mcro.h"
#include "passes.h"
#include "createObjectFiles.h"

/**
 * Main function of the program that processes one or more input files,
 * performs pre-processing, first and second passes, and generates object files.
 * The program takes in a list of file names as arguments and processes each file.
 *
 * @param argc The number of arguments passed to the program (including the program name).
 * @param argv An array of strings representing the command-line arguments. The input files are passed as arguments.
 *
 * @return Returns 0 if the program completes successfully.
 */
int main(int argc, char *argv[])
{
    int i, j, k, ICF, DCF, there_is_an_error;
    mcro_table macro_table;
    symbol_table symbols_table;
    int *instruction_area, *data_area;

    /* Loop through each input file passed as argument */
    for (i = 1; i < argc; i++)
    {
        ICF = 0, DCF = 0, there_is_an_error = 0;

        /* Allocate memory for instruction and data areas */
        instruction_area = (int *)malloc(100 * sizeof(int));
        data_area = (int *)malloc(100 * sizeof(int));

        /* Initialize macro table and symbol table */
        macro_table.macros = NULL;
        macro_table.count = 0;
        symbols_table.symbols = NULL;
        symbols_table.count = 0;

        /* Pre-process the file (expand macros) */
        preprocessor(argv[i], &macro_table, &there_is_an_error);

        /* If no errors occurred in pre-processing, proceed with the first and second passes */
        if (!there_is_an_error)
        {
            first_pass(argv[i], &macro_table, &symbols_table, &instruction_area, &data_area, &ICF, &DCF, &there_is_an_error);
            second_pass(argv[i], &macro_table, &symbols_table, &instruction_area, &data_area, &ICF, &DCF, &there_is_an_error);
        }

        /* If no errors occurred during passes, create the object file */
        if (!there_is_an_error)
        {
            create_object_file(argv[i], instruction_area, data_area, ICF, DCF);
        }

        /* Free the memory allocated for macros */
        for (k = 0; k < macro_table.count; k++)
        {
            for (j = 0; j < macro_table.macros[k].line_count; j++)
            {
                free(macro_table.macros[k].mcro_content[j]);
            }
        }
        free(macro_table.macros);

        /* Free the memory allocated for instruction and data areas */
        free(instruction_area);
        free(data_area);
    }
    return 0;  /* Return 0 to indicate successful execution */
}
