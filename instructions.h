#ifndef DEFINES_H
#define DEFINES_H

/* Constants */
#define NUMBER_OF_INSTRUCTION 16           /* Total number of instructions */
#define NUMBER_OF_DATA_INSTRUCTION 4       /* Total number of data instructions */
#define OPCODE_POSITION 18                 /* Bit position of the opcode */
#define FUNCTION_POSITION 3                /* Bit position of the function code */
#define TYPE_DESTINATION_ADDRESS_POSITION 11  /* Position of the destination address type */
#define TYPE_SORCE_ADDRESS_POSITION 16        /* Position of the source address type */
#define REGISTER_DESTINATION_POSITION 8       /* Position of the destination register */
#define REGISTER_SORCE_POSITION 13            /* Position of the source register */
#define BIT_E 1                             /* Bit E (error or flag) */
#define BIT_R 2                             /* Bit R (reserved or flag) */
#define BIT_A 4                             /* Bit A (reserved or flag) */

/* Includes */
#include "mcro.h"
#include "passes.h"

/* Enum for operand types */
typedef enum operand_type {
    IMMEDIATE = 0,  /* Immediate operand type */
    DIRECT = 1,     /* Direct operand type */
    RELATIVE = 2,   /* Relative operand type */
    REGISTER = 3    /* Register operand type */
} operand_type;

/* Structure for instructions */
typedef struct instruction {
    char instruction_name[10];         /* Instruction name */
    int opcode;                        /* Opcode for the instruction */
    int function;                      /* Function code */
    int there_a_sorce_operand;         /* Flag indicating if there is a source operand */
    int number_type_sorce_operand;    /* Number of source operand types */
    operand_type sorce_operand_type[4]; /* Types of source operands */
    int there_a_destination_operand;  /* Flag indicating if there is a destination operand */
    int number_type_destination_operand; /* Number of destination operand types */
    operand_type destination_operand_type[4]; /* Types of destination operands */
} instruction;

/* Function declarations */

/* First pass of assembler: processes the source file and prepares data for the second pass */
int first_pass(const char *file_name, const mcro_table *table, symbol_table *symbols_table, int **instruction_area, int **data_area, int *ICF, int *DCF, int *there_is_an_error);

/* Second pass of assembler: processes the data and generates the final machine code */
int second_pass(const char *file_name, const mcro_table *table, symbol_table *symbols_table, int **instruction_area, int **data_area, int *ICF, int *DCF, int *there_is_an_error);

/* Function to find an instruction by its name */
int find_instruction_name(char *word);

/* Function to find a data instruction by its name */
int find_data_instruction(char *word);

/* External declaration for the instruction table */
extern instruction instruction_table[];

#endif
