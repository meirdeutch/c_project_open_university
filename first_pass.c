#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "defines.h"

char *  trim_leading_whitespace(char *str) {
    int i = 0;
    while (isspace(str[i])) { 
        i++;
    }
    strcpy(str, str + i);
    return str;
}
typedef enum symbol_type {
    CODE,
    DATA,
    EXTERNAL
} symbol_type;
typedef struct symbol
{
    char symbol_name[31];
    symbol_type type;
    int address;
}symbol;
symbol *symbol_table = NULL;
int symbol_count = 0, there_symbol_definition = 0;

int find_symbol_in_table(char * symbol_name){
    int i;
    for ( i = 0; i < symbol_count; i++)
    {
        if(!strcmp(symbol_table[i].symbol_name,symbol_name)){
            return i;
        }
    }
    return -1;  
}

void enter_in_symbol_table(char *symbol_name, int address, symbol_type type){
    symbol * temp = NULL;
    temp = (symbol *)realloc(symbol_table, (symbol_count + 1) * sizeof(symbol));
    if(temp == NULL){
        printf("Memoty allocation faild in first pass\n");
    }else{
        symbol_table = temp;
        sscanf(symbol_name, "%s", symbol_table[symbol_count].symbol_name);
        symbol_table[symbol_count].type = type;
        symbol_table[symbol_count].address = address;
        symbol_count++;
    }
}

int first_pass(char *file_name){
    char line[82], word[31], symbol_name[31];
    int i, position, number, line_length, word_length, line_counter = 0, DC = 0, IC = 100;
    int * data_area = NULL, *temp = NULL;
    FILE * sorce;
    sorce = fopen(file_name, "r");
    if (sorce == NULL)
    {
        printf("Error: cannot open %s\n", file_name);
        return 1;
    }
    while (fgets(line, sizeof(line), sorce))
    {
        line_counter++;

        line_length = strlen(line);
        if(line_length > 81){
            printf("line %d is lorger than allowed\n", line_counter);
        }
        sscanf(line, "%s", word);
        word_length = strlen(word);
        trim_leading_whitespace(line);
        strcpy(line, line + word_length);
            if(word[word_length -1] == ':')
            {
                word[word_length - 1] = '\0';
                strcpy(symbol_name,word);
                if ((find_instruction_name(symbol_name) != -1) || (find_data_instruction(symbol_name) != -1) || (find_mcro_in_table(symbol_name) != -1) || (find_symbol_in_table(symbol_name) != -1)){
                    printf("Errorin line %d: The name of a label cannot be the name of an instruction or mcro name or an other symbol name\n", line_counter);
                    continue;
                }
                there_symbol_definition = 1;
                sscanf(line, "%s", word);
                word_length = strlen(word);
                trim_leading_whitespace(line);
                strcpy(line, line + word_length);
            }  
            if (!strcmp(word, ".data")){
                if (there_symbol_definition){
                    enter_in_symbol_table(symbol_name, DC, DATA);
                    there_symbol_definition = 0;
                }
                
                while (strlen(line))
                {
                    sscanf(line, "%[^,]", word);
                    sscanf(word, "%d", &number);
                    word_length = strlen(word);
                    trim_leading_whitespace(line);
                    strcpy(line, line + word_length);
                    temp = (int*)realloc(data_area, (DC + 1) * sizeof(int));
                    if (temp == NULL){
                        printf("Memoty allocation faild in first pass\n");
                        fclose(sorce);
                        return 1;
                    }
                    data_area = temp;
                    data_area[DC] = number;
                    DC++;
                }  
            }
            else if (!strcmp(word, ".string")){
                if (there_symbol_definition){
                    enter_in_symbol_table(symbol_name, DC, DATA);
                    there_symbol_definition = 0;
                }
                sscanf(line, "%s", word);
                word_length = strlen(word);
                if (word[0] != '"' || word[word_length-1] != '"'){
                    printf("Error in line %d: A string must begin and end with double quotes\n", line_counter);
                    continue;
                }
                for (i = 1; i <  word_length -1; i++)
                {
                    temp = (int*)realloc(data_area, (DC + 1) * sizeof(int));
                    if (temp == NULL){
                        printf("Memoty allocation faild in first pass\n");
                        fclose(sorce);
                        return 1;
                    }
                    data_area = temp;
                    data_area[DC] = word[i];
                    DC++;
                }
                temp = (int*)realloc(data_area, (DC + 1) * sizeof(int));
                if (temp == NULL){
                    printf("Memoty allocation faild in first pass\n");
                    fclose(sorce);
                    return 1;
                }
                data_area = temp;
                data_area[DC] = '\0';
                DC++;
            }
            else if (!strcmp(word, ".entry")){
                continue;
            }
            else if (!strcmp(word, ".extern")){
                sscanf(line, "%s", symbol_name);
                enter_in_symbol_table(symbol_name, 0, EXTERNAL);
                continue;
            }
            else {
                position = find_instruction_name(word);
                if (position == -1){
                    printf("Error in line %d: The line does not start with the name of an instruction or directive or a definition of a symbol\n", line_counter);
                    continue;
                }else{
                    word_length = strlen(word);
                    trim_leading_whitespace(line);
                    strcpy(line, line + word_length);
                }
            }

                /*if(find_instruction_name(second_word) != -1){
                    temp = (symbol *)realloc(symbol_table, (symbol_count + 1) * sizeof(symbol));
                    if(temp == NULL){
                        printf("Memoty allocation faild in first pass\n");
                        fclose(sorce);
                    }
                    symbol_table = temp;
                    sscanf(word, "%s", symbol_table[symbol_count].symbol_name);
                    symbol_table[symbol_count].type = CODE;
                    symbol_table[symbol_count].address = IC + 1;
                    symbol_count++;
                }else if ((!strcmp(second_word, ".data")) || (!strcmp(second_word, ".string"))){
                    temp = (symbol *)realloc(symbol_table, (symbol_count + 1) * sizeof(symbol));
                    if(temp == NULL){
                        printf("Memoty allocation faild in first pass\n");
                        fclose(sorce);
                        return 1;
                    }
                    symbol_table = temp;
                    sscanf(word, "%s", symbol_table[symbol_count].symbol_name);
                    symbol_table[symbol_count].type = DATA;
                    symbol_table[symbol_count].address = DC + 1;
                    symbol_count++;
                }else{
                    printf("Error: After declaring a symbol, an instruction name or a type-data name should appear\n");
                    fclose(sorce);
                    return 1;
                }*/
            }
    
    fclose(sorce);
    /*for ( i = 0; i < symbol_count; i++)
    {
        printf("%s\t%d\t%d\n", symbol_table[i].symbol_name, symbol_table[i].type, symbol_table[i].address);
    }
    free(symbol_table);
    for ( i = 0; i < DC; i++)
    {
        printf("%d\n", data_area[i]);
    }*/
    
    return 0;
}

