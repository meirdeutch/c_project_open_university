#ifndef DEFINES_H
#define DEFINES_H
#define NUMBER_OF_INSTRUCTION 16
#define NUMBER_OF_DATA_INSTRUCTION 4
typedef enum operand_type {
    IMMEDIATE = 0,
    DIRECT = 1,
    RELATIVE = 2,
    REGISTER = 3
} operand_type;

typedef struct instruction {
    char instruction_name[10];
    int opcode;
    int function;
    int number_type_sorce_operand;
    operand_type sorse_operand_type[4];
    int number_type_destination_operand;
    operand_type destination_operand_type[4];
} instruction;

int preprocessor(char *file_name);
int first_pass(char *file_name);
int find_instruction_name(char * word);
int find_data_instruction(char * word);
int find_mcro_in_table(char *word);
extern instruction instruction_table[];
typedef struct mcro
{
    char mcro_name[31];
    char *mcro_content[81];
    int line_count;
} mcro;
extern mcro *mcro_table;
extern int mcro_count;

#endif
