#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "createObjectFiles.h"

int create_object_file(const char *file_name, const int *instruction_area, const int *data_area, const int ICF, const int DCF){
    int i;
    char *object_file_name = malloc(strlen(file_name) + 4);
    FILE *object_file;
    sprintf(object_file_name, "%s.ob", file_name);
    object_file = fopen(object_file_name, "w");
    if (object_file == NULL){
        printf("Error: cannot open %s\n", object_file_name);
        return 1;
    }
    fprintf(object_file, "% 6d %d\n", ICF-100, DCF);
    for (i = 100; i < (ICF); i++)
    {
        fprintf(object_file, "%06d ", i);
        fprintf(object_file, "%06x\n", instruction_area[i] & 0xffffff);
        fprintf(object_file, "\n");
    }
    fprintf(object_file, "%s\n", "data");
    for (i = 0; i < (DCF); i++)
    {
        fprintf(object_file, "%06d ", i + (ICF));
        fprintf(object_file, "%06x\n", data_area[i] & 0xffffff);
        fprintf(object_file, "\n");
    }
    fclose(object_file);
    return 0;
}