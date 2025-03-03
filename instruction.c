#include <string.h>
#include "defines.h"

instruction instruction_table[] = {
    {"mov", 0, 0, 3, {IMMEDIATE, DIRECT, REGISTER}, 2, {DIRECT, REGISTER}},
    {"cmp", 1, 0, 3, {IMMEDIATE, DIRECT, REGISTER}, 3, {IMMEDIATE, DIRECT, REGISTER}},
    {"add", 2, 1, 3, {IMMEDIATE, DIRECT, REGISTER}, 2, {DIRECT, REGISTER}},
    {"sub", 2, 2, 3, {IMMEDIATE, DIRECT, REGISTER}, 2, {DIRECT, REGISTER}},
    {"lea", 4, 0 , 1, {DIRECT}, 2, {DIRECT, REGISTER}},
    {"clr", 5, 1, 0, {0}, 2, {DIRECT, REGISTER}},
    {"not", 5, 2, 0, {0}, 2, {DIRECT, REGISTER}},
    {"inc", 5, 3, 0, {0}, 2, {DIRECT, REGISTER}},
    {"dec", 5, 4, 0, {0}, 2, {DIRECT, REGISTER}},
    {"jmp", 9, 1, 0, {0}, 2, {DIRECT, RELATIVE}},
    {"bne", 9, 2, 0, {0}, 2, {DIRECT, RELATIVE}},
    {"jsr", 9, 3, 0, {0}, 2, {DIRECT, RELATIVE}},
    {"red", 12, 0, 0, {0}, 2, {DIRECT, REGISTER}},
    {"prn", 13, 0, 0, {0}, 3, {IMMEDIATE, DIRECT, REGISTER}},
    {"rts", 14, 0, 0, {0}, 0, {0}},
    {"stop", 15, 0, 0, {0}, 0, {0}}
};
char * data_instruction [] = {".data", ".string", ".entry", "extern"};
int find_instruction_name(char * word){
    int i;
    for ( i = 0; i < NUMBER_OF_INSTRUCTION; i++)
    {
        if(!strcmp(instruction_table[i].instruction_name, word)){
            return i;
        }
    } 
    return -1;
}
int find_data_instruction(char * word){
    int i;
    for ( i = 0; i < NUMBER_OF_DATA_INSTRUCTION; i++)
    {
        if(!strcmp(data_instruction[i], word)){
            return i;
        }
    } 
    return -1;
}