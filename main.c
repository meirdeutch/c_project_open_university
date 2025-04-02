#include <stdio.h>
#include <stdlib.h>
#include "instructions.h"
#include "mcro.h"
#include "passes.h"
#include "createObjectFiles.h"

int main(int argc, char *argv[])
{
    int i, j, k, ICF, DCF, there_is_an_error;
    mcro_table macro_table;
    symbol_table symbols_table;
    int *instruction_area, *data_area;
    for (i = 1; i < argc; i++)
    {
        ICF = 0, DCF = 0, there_is_an_error = 0;
        instruction_area = (int *)malloc(100 * sizeof(int));
        data_area = (int *)malloc(100 * sizeof(int));
        macro_table.macros = NULL;
        macro_table.count = 0;
        symbols_table.symbols = NULL;
        symbols_table.count = 0;
        preprocessor(argv[i], &macro_table, &there_is_an_error);
        if (!there_is_an_error)
        {
            first_pass(argv[i], &macro_table, &symbols_table, &instruction_area, &data_area, &ICF, &DCF, &there_is_an_error);
            second_pass(argv[i], &macro_table, &symbols_table, &instruction_area, &data_area, &ICF, &DCF, &there_is_an_error);
        }
        if (!there_is_an_error)
        {
            create_object_file(argv[i], instruction_area, data_area, ICF, DCF);
        }
        for (k = 0; k < macro_table.count; k++)
        {
            for (j = 0; j < macro_table.macros[i].line_count; j++)
            {
                free(macro_table.macros[k].mcro_content[j]);
            }
        }
        free(macro_table.macros);
        free(instruction_area);
        free(data_area);
    }

    return 0;
}