#include "defines.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


mcro *mcro_table = NULL;
int mcro_count = 0;
int find_mcro_in_table(char *word)
{
    int i = 0;
    for (i = 0; i < mcro_count; i++)
    {
        if (!strcmp(word, mcro_table[i].mcro_name))
        {
            return i;
        }
    }
    return -1;
}

int preprocessor(char *file_name)
{
    char line[81];
    char word[31];
    char sorce_file_name[31], des_file_name[31];
    int line_length, position, i, j, in_mcro = 0;
    mcro *temp = NULL;
    FILE *sorce, *destination;
    sprintf(sorce_file_name, "%s.as", file_name);
    sprintf(des_file_name, "%s.am", file_name);
    sorce = fopen(sorce_file_name, "r");
    if (sorce == NULL)
    {
        printf("Error: cannot open %s\n", sorce_file_name);
        return 1;
    }
    destination = fopen(des_file_name, "w");
    if (destination == NULL)
    {
        printf("Error: cannot open %s\n", des_file_name);
        fclose(sorce);
        return 1;
    }
    while (fgets(line, sizeof(line), sorce))
    {
        line_length = strlen(line);
        sscanf(line, "%s", word);
        /*in a mcro definition*/
        if (in_mcro)
        {
            /* end of mcro definition*/
            if (!strcmp(word, "mcroend"))
            {
                in_mcro = 0;
                mcro_count++;
                continue;
            }
            else
            {
                /*continue mcro definition*/
                mcro_table[mcro_count].mcro_content[mcro_table[mcro_count].line_count] = (char *)malloc(line_length + 1);
                if (mcro_table[mcro_count].mcro_content[mcro_table[mcro_count].line_count] == NULL)
                {
                    printf("Memoty allocation faild in preprocessor\n");
                    remove(des_file_name);
                    fclose(sorce);
                    fclose(destination);
                    return 1;
                }
                strcpy(mcro_table[mcro_count].mcro_content[mcro_table[mcro_count].line_count], line);
                mcro_table[mcro_count].line_count++;
            }
        }
        else
        {
            /*Macro definition*/
            if (!strcmp(word, "mcro"))
            {
                sscanf(line, "%*s%s", word);
                position = find_mcro_in_table(word);
                /*check if the mcro name already exists*/
                if (position != -1){
                    printf("Error: You cannot declare two macros with the same name");
                    remove(des_file_name);
                    fclose(sorce);
                    fclose(destination);
                    return 1;
                }
                in_mcro = 1;
                temp = (mcro *)realloc(mcro_table, (mcro_count + 1) * sizeof(mcro));
                if (temp == NULL)
                {
                    printf("Memoty allocation faild in preprocessor\n");
                    remove(des_file_name);
                    fclose(sorce);
                    fclose(destination);
                    return 1;
                }
                mcro_table = temp;
                sscanf(word, "%s",mcro_table[mcro_count].mcro_name);
                mcro_table[mcro_count].line_count = 0;
                continue;
            }
            position = find_mcro_in_table(word);
            /*if it is not a mcro name then write the line as it is*/
            if (position == -1)
            {
                fprintf(destination, "%s", line);
            }
            /*if it is a mcro name then write the  contents of the mcro  instead of the name*/
            else
            {
                for (i = 0; i < mcro_table[position].line_count; i++)
                {
                    fprintf(destination, "%s", mcro_table[position].mcro_content[i]);
                }
            }
        }
    }
    fclose(sorce);
    fclose(destination);
    return 0;
}
