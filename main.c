#include <stdio.h>
#include <stdlib.h>
#include "defines.h"

int main(){
    int i, j;
    preprocessor("input");
    first_pass("input.am");
    for (i = 0; i < mcro_count; i++)
    {
        for (j = 0; j < mcro_table[i].line_count; j++)
        {
            free(mcro_table[i].mcro_content[j]);
        }
    }
    free(mcro_table);
    return 0;
}