#include <string.h>
#include "instructions.h"

/* 
 * Table of instructions. Each instruction contains:
 *   - Name of the instruction.
 *   - Operation code (opcode).
 *   - Additional information for the instruction (e.g., addressing modes, number of operands, etc.).
 */
instruction instruction_table[] = {
    {"mov", 0, 0, 1, 3, {IMMEDIATE, DIRECT, REGISTER}, 1,  2, {DIRECT, REGISTER}},
    {"cmp", 1, 0, 1, 3, {IMMEDIATE, DIRECT, REGISTER}, 1, 3, {IMMEDIATE, DIRECT, REGISTER}},
    {"add", 2, 1, 1, 3, {IMMEDIATE, DIRECT, REGISTER}, 1, 2, {DIRECT, REGISTER}},
    {"sub", 2, 2, 1, 3, {IMMEDIATE, DIRECT, REGISTER}, 1, 2, {DIRECT, REGISTER}},
    {"lea", 4, 0 , 1, 1, {DIRECT}, 1, 2, {DIRECT, REGISTER}},
    {"clr", 5, 1, 0, 0, {0}, 1, 2, {DIRECT, REGISTER}},
    {"not", 5, 2, 0, 0, {0}, 1, 2, {DIRECT, REGISTER}},
    {"inc", 5, 3, 0, 0, {0}, 1, 2, {DIRECT, REGISTER}},
    {"dec", 5, 4, 0, 0, {0}, 1, 2, {DIRECT, REGISTER}},
    {"jmp", 9, 1, 0, 0, {0}, 1, 2, {DIRECT, RELATIVE}},
    {"bne", 9, 2, 0, 0, {0}, 1, 2, {DIRECT, RELATIVE}},
    {"jsr", 9, 3, 0, 0, {0}, 1, 2, {DIRECT, RELATIVE}},
    {"red", 12, 0, 0, 0, {0}, 1, 2, {DIRECT, REGISTER}},
    {"prn", 13, 0, 0, 0, {0}, 1, 3, {IMMEDIATE, DIRECT, REGISTER}},
    {"rts", 14, 0, 0, 0, {0}, 0, 0, {0}},
    {"stop", 15, 0, 0, 0, {0}, 0, 0, {0}}
};

/* 
 * Array of data instructions. These are assembler directives.
 */
char * data_instruction[] = {".data", ".string", ".entry", "extern"};

/* 
 * Function to find an instruction by its name.
 * Searches for the instruction in the instruction_table array.
 * 
 * Parameters:
 *   word  - The instruction name to search for.
 *
 * Returns:
 *   The index of the instruction in the table if found, otherwise -1.
 */
int find_instruction_name(char *word)
{
    int i;
    for (i = 0; i < NUMBER_OF_INSTRUCTION; i++)
    {
        if (!strcmp(instruction_table[i].instruction_name, word))
        {
            return i;  /* Instruction found, return its index */
        }
    }
    return -1;  /* Instruction not found */
}

/* 
 * Function to find a data instruction by its name.
 * Searches for the data instruction in the data_instruction array.
 * 
 * Parameters:
 *   word  - The data instruction name to search for.
 *
 * Returns:
 *   The index of the data instruction in the array if found, otherwise -1.
 */
int find_data_instruction(char *word)
{
    int i;
    for (i = 0; i < NUMBER_OF_DATA_INSTRUCTION; i++)
    {
        if (!strcmp(data_instruction[i], word))
        {
            return i;  /* Data instruction found, return its index */
        }
    }
    return -1;  /* Data instruction not found */
}
