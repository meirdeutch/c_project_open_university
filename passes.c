#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "instructions.h"
#include "mcro.h"
#include "passes.h"

/*
 * Trim leading whitespace from a string.
 * This function removes any whitespace characters (spaces, tabs, etc.)
 * from the beginning of the string and shifts the rest of the string to the left.
 *
 * Parameters:
 *   char *str - The string to trim.
 *
 * Returns:
 *   char* - The trimmed string.
 */
char *trim_leading_whitespace(char *str)
{
    int i = 0;
    /* Loop through the string and skip leading whitespace characters */
    while (isspace(str[i]))
    {
        i++;
    }
    /* Shift the string to remove leading spaces */
    strcpy(str, str + i);
    return str;
}

/*
 * Find a symbol in the symbol table.
 * This function searches for a symbol in the symbol table by its name.
 *
 * Parameters:
 *   char *word - The symbol name to search for.
 *   const symbol_table *table - The symbol table to search in.
 *
 * Returns:
 *   int - The index of the symbol in the table if found, otherwise -1.
 */
int find_symbol_in_table(char *word, const symbol_table *table)
{
    int i;
    for (i = 0; i < table->count; i++)
    {
        if (!strcmp(word, table->symbols[i].symbol_name))
        {
            return i;
        }
    }
    return -1;
}

/*
 * Add a new symbol to the symbol table.
 * This function inserts a new symbol with its name, address, and type into the symbol table.
 *
 * Parameters:
 *   char *symbol_name - The name of the symbol.
 *   int address - The address of the symbol.
 *   symbol_type type - The type of the symbol.
 *   symbol_table *table - The symbol table to add the symbol to.
 */
void enter_in_symbol_table(char *symbol_name, int address, symbol_type type, symbol_table *table)
{
    symbol *temp = NULL;
    temp = (symbol *)realloc(table->symbols, ((table->count) + 1) * sizeof(symbol));
    if (temp == NULL)
    {
        printf("Memory allocation failed in first pass\n");
    }
    else
    {
        table->symbols = temp;
        sscanf(symbol_name, "%s", table->symbols[table->count].symbol_name);
        table->symbols[table->count].type = type;
        table->symbols[table->count].address = address;
        table->symbols[table->count].is_entry = 0;
        (table->count)++;
    }
}

/*
 * Check the operand type based on the symbol table and the operand's format.
 * This function determines whether an operand is a register, immediate, direct, or relative operand.
 *
 * Parameters:
 *   char *operand - The operand to check.
 *   const symbol_table *symbols_table - The symbol table to use for checking direct and relative operands.
 *
 * Returns:
 *   int - The operand type (REGISTER, IMMEDIATE, DIRECT, RELATIVE), or -1 if invalid.
 */
int check_operand_type(char *operand, const symbol_table *symbols_table)
{
    int number, i, length;
    /* Check if the operand is a register (r0 to r7) */
    if (operand[0] == 'r' && sscanf(operand + 1, "%d", &number) && number >= 0 && number <= 7)
    {
        return REGISTER;
    }
    /* Check if the operand is an immediate value (starts with '#') */
    if (operand[0] == '#')
    {
        length = strlen(operand);
        for (i = 1; i < length - 2; i++)
        {
            if (!isdigit(operand[i]))
            {
                return -1;
            }
        }
        return IMMEDIATE;
    }
    /* Check if the operand is a symbol from the symbol table (direct address) */
    if (find_symbol_in_table(operand, symbols_table) != -1)
    {
        return DIRECT;
    }
    /* Check if the operand is a relative address (starts with '&') */
    if (operand[0] == '&' && find_symbol_in_table(operand + 1, symbols_table) != -1)
    {
        return RELATIVE;
    }
    return -1; /* Invalid operand type */
}

/*
 * Check if the operand type is legal for the given instruction.
 * This function checks whether the operand type is allowed for either a source or destination operand in an instruction.
 *
 * Parameters:
 *   int operand_type - The operand type to check.
 *   int instruction - The instruction index to check against.
 *   int is_sorce_operand - Flag indicating if the operand is a source operand (1) or destination operand (0).
 *
 * Returns:
 *   int - 1 if the operand type is legal, 0 if not.
 */
int is_operand_type_legal(int operand_type, int instruction, int is_sorce_operand)
{
    int i;
    /* Check legal operand type for destination operand */
    if (!is_sorce_operand)
    {
        for (i = 0; i < instruction_table[instruction].number_type_destination_operand; i++)
        {
            if (operand_type == instruction_table[instruction].destination_operand_type[i])
            {
                return 1;
            }
        }
    }
    /* Check legal operand type for source operand */
    else
    {
        for (i = 0; i < instruction_table[instruction].number_type_sorce_operand; i++)
        {
            if (operand_type == instruction_table[instruction].sorce_operand_type[i])
            {
                return 1;
            }
        }
    }
    return 0; /* Operand type is not legal */
}

/*
 * First pass of the assembler process.
 * This function processes the source file, handling symbol definitions, instructions,
 * and data assignments (e.g., .data, .string, etc.) while updating the symbol table,
 * instruction area, and data area.
 *
 * Parameters:
 *   const char *file_name - The name of the source file to process.
 *   const mcro_table *table - The macro table to look for macros.
 *   symbol_table *symbols_table - The symbol table to store symbol definitions.
 *   int **instruction_area - The array to store machine code instructions.
 *   int **data_area - The array to store data.
 *   int *ICF - The final value of the instruction counter (IC) after processing.
 *   int *DCF - The final value of the data counter (DC) after processing.
 *   int *there_is_an_error - Flag to indicate if any errors were encountered during the pass.
 *
 * Returns:
 *   int - 0 if successful, 1 if an error occurred.
 */
int first_pass(const char *file_name, const mcro_table *table, symbol_table *symbols_table, int **instruction_area, int **data_area, int *ICF, int *DCF, int *there_is_an_error)
{
    char line[82], word[80], symbol_name[31];
    char *sorce_file_name = malloc(strlen(file_name) + 4);
    char operands[2][31];
    int i, destination_operand_type, sorce_operand_type, position, number, line_length, word_length, line_counter = 0, DC = 0, IC = 100, L = 0, there_symbol_definition;
    int *temp = NULL;
    FILE *sorce;
    sprintf(sorce_file_name, "%s.am", file_name);
    sorce = fopen(sorce_file_name, "r");
    if (sorce == NULL)
    {
        printf("Error: cannot open %s\n", sorce_file_name);
        *there_is_an_error = 1;
        return 1;
    }

    /* Process each line in the source file */
    while (fgets(line, sizeof(line), sorce))
    {
        line_counter++;
        line_length = strlen(line);

        /* Ensure line does not exceed 81 characters */
        if (line_length > 81)
        {
            printf("line %d is longer than allowed\n", line_counter);
        }

        /* Skip leading whitespace */
        for (i = 0; i < line_length; i++)
        {
            if (!isspace(line[i]))
            {
                break;
            }
        }
        if (i == line_length)
        {
            continue;
        }

        sscanf(line, "%s", word);
        word_length = strlen(word);

        /* Trim leading whitespace from the line */
        trim_leading_whitespace(line);
        strcpy(line, line + word_length);

        /* Handle labels */
        if (word[word_length - 1] == ':')
        {
            word[word_length - 1] = '\0'; /*Remove the colon*/
            strcpy(symbol_name, word);

            /* Check for errors if label matches any instruction, macro, or existing symbol */
            if ((find_instruction_name(symbol_name) != -1) ||
                (find_data_instruction(symbol_name) != -1) ||
                (find_mcro_in_table(symbol_name, table) != -1) ||
                (find_symbol_in_table(symbol_name, symbols_table) != -1))
            {
                printf("Error in file %s on line %d: The name of a label cannot be the name of an instruction or macro or another symbol\n", sorce_file_name, line_counter);
                *there_is_an_error = 1;
                continue;
            }

            there_symbol_definition = 1; /*Mark symbol definition*/
            sscanf(line, "%s", word);    /*Move to next word*/
            word_length = strlen(word);
            trim_leading_whitespace(line);
            strcpy(line, line + word_length);
        }

        /* Handle directives such as .data, .string, .entry, .extern */
        if (!strcmp(word, ".data"))
        {
            if (there_symbol_definition)
            {
                enter_in_symbol_table(symbol_name, DC, DATA, symbols_table);
                there_symbol_definition = 0;
            }

            /* Read data values and insert into data area */
            while (strlen(line))
            {
                sscanf(line, "%[^,]", word); /*Extract data value*/
                sscanf(word, "%d", &number); /*Convert to integer*/

                word_length = strlen(word);
                if (line[word_length] == ',')
                {
                    word_length++;
                }

                trim_leading_whitespace(line);
                strcpy(line, line + word_length);

                /* Allocate memory for the data area and store the data */
                temp = (int *)realloc(*data_area, (DC + 1) * sizeof(int));
                if (temp == NULL)
                {
                    printf("Memory allocation failed in first pass\n");
                    fclose(sorce);
                    return 1;
                }
                *data_area = temp;
                (*data_area)[DC] = number;
                DC++;
            }
        }
        else if (!strcmp(word, ".string"))
        {
            if (there_symbol_definition)
            {
                enter_in_symbol_table(symbol_name, DC, DATA, symbols_table);
                there_symbol_definition = 0;
            }

            /* Handle string values */
            sscanf(line, "%s", word);
            word_length = strlen(word);

            /* Check if string is properly quoted */
            if (word[0] != '"' || word[word_length - 1] != '"')
            {
                printf("Error in file %s on line %d: A string must begin and end with double quotes\n", sorce_file_name, line_counter);
                *there_is_an_error = 1;
                continue;
            }

            /* Store each character of the string in the data area */
            for (i = 1; i < word_length - 1; i++)
            {
                temp = (int *)realloc(*data_area, (DC + 1) * sizeof(int));
                if (temp == NULL)
                {
                    printf("Memory allocation failed in first pass\n");
                    fclose(sorce);
                    return 1;
                }
                *data_area = temp;
                (*data_area)[DC] = word[i];
                DC++;
            }

            /* Add null terminator */
            temp = (int *)realloc(*data_area, (DC + 1) * sizeof(int));
            if (temp == NULL)
            {
                printf("Memory allocation failed in first pass\n");
                fclose(sorce);
                return 1;
            }
            *data_area = temp;
            (*data_area)[DC] = '\0';
            DC++;
        }
        else if (!strcmp(word, ".extern"))
        {
            /* Handle .entry and .extern directives */
            sscanf(line, "%s", symbol_name);
            enter_in_symbol_table(symbol_name, 0, EXTERNAL, symbols_table);
            continue;
        }
        else if (!strcmp(word, ".entry"))
        {
            continue;
        }
        else if (word[0] == ';')
        {
            /* Skip comment lines */
            continue;
        }
        else
        {
            /* Process instructions */
            position = find_instruction_name(word);
            if (position == -1)
            {
                printf("Error in file %s on line %d: The line does not start with the name of an instruction or directive or symbol definition\n", sorce_file_name, line_counter);
                *there_is_an_error = 1;
                continue;
            }
            else
            {
                /* Process operands */
                trim_leading_whitespace(line);
                i = 0;
                while (strlen(line))
                {
                    sscanf(line, "%[^,]", word); /* Extract operand*/
                    sscanf(word, "%s", word);
                    word_length = strlen(word);
                    if (line[word_length] == ',')
                    {
                        word_length++;
                    }
                    trim_leading_whitespace(line);
                    strcpy(line, line + word_length);
                    trim_leading_whitespace(line);
                    strcpy(operands[i], word);
                    i++;
                }

                /* Verify operand count matches the instruction */
                if (instruction_table[position].there_a_sorce_operand + instruction_table[position].there_a_destination_operand != i)
                {
                    printf("Error in file %s on line %d: The standard number of operands for the %s instruction is %d.\n", sorce_file_name, line_counter, instruction_table[position].instruction_name, instruction_table[position].there_a_sorce_operand + instruction_table[position].there_a_destination_operand);
                    *there_is_an_error = 1;
                    continue;
                }
                else
                {
                    if (there_symbol_definition)
                    {
                        enter_in_symbol_table(symbol_name, IC, CODE, symbols_table);
                        there_symbol_definition = 0;
                    }

                    /* Generate machine code for the instruction */
                    switch (i)
                    {
                        /*Handle different operand count cases*/
                        /* Handle different operand count cases */
                    /* Handle the case where there are 0 operands */
                    case 0:
                        /* Initialize number to 4 (default value for instructions) */
                        number = 4;

                        /* Set the opcode and function values from the instruction table */
                        number |= (instruction_table[position].opcode << OPCODE_POSITION);
                        number |= (instruction_table[position].function << FUNCTION_POSITION);

                        /* Reallocate memory for instruction_area to accommodate the new instruction */
                        temp = (int *)realloc(*instruction_area, (IC + 1) * sizeof(int));
                        if (temp == NULL)
                        {
                            printf("Memory allocation failed in first pass\n");
                            fclose(sorce);
                            return 1; /* Return error if memory allocation fails */
                        }

                        /* Update instruction_area with the new instruction */
                        *instruction_area = temp;
                        (*instruction_area)[IC] = number;
                        IC++; /* Increment the instruction counter */
                        break;

                    /* Handle the case where there is 1 operand */
                    case 1:
                        /* Check the operand type for the destination operand */
                        destination_operand_type = check_operand_type(operands[0], symbols_table);

                        /* Validate the operand type against the instruction's expectations */
                        if (!is_operand_type_legal(destination_operand_type, position, 0) && destination_operand_type != -1)
                        {
                            printf("Error in file %s on line %d: The instruction %s does not support the destination operand of operand type %d\n",
                                   sorce_file_name, line_counter, instruction_table[position].instruction_name, destination_operand_type);
                            *there_is_an_error = 1;
                            break; /* Break if there's an operand type error */
                        }

                        /* Initialize the instruction number */
                        number = 4;
                        number |= (instruction_table[position].opcode << OPCODE_POSITION);
                        number |= (instruction_table[position].function << FUNCTION_POSITION);

                        /* If destination operand type is valid, set its address type in the instruction */
                        if (destination_operand_type != -1)
                        {
                            number |= (destination_operand_type << TYPE_DESTINATION_ADDRESS_POSITION);
                        }

                        /* Reallocate memory for the instruction area */
                        temp = (int *)realloc(*instruction_area, (IC + 1) * sizeof(int));
                        if (temp == NULL)
                        {
                            printf("Memory allocation failed in first pass\n");
                            fclose(sorce);
                            return 1;
                        }

                        /* Update the instruction in the area */
                        *instruction_area = temp;
                        (*instruction_area)[IC] = number;
                        IC++; /* Increment the instruction counter */

                        /* Handle different cases based on the operand type */
                        switch (destination_operand_type)
                        {
                        case REGISTER:
                            /* Extract the register number from the operand and add it to the instruction */
                            sscanf(word + 1, "%d", &number);
                            (*instruction_area)[IC - 1] |= (number << REGISTER_DESTINATION_POSITION);
                            break;
                        case IMMEDIATE:
                            /* Extract the immediate value and add it to the instruction */
                            sscanf(word + 1, "%d", &number);
                            temp = (int *)realloc(*instruction_area, (IC + 1) * sizeof(int));
                            if (temp == NULL)
                            {
                                printf("Memory allocation failed in first pass\n");
                                fclose(sorce);
                                return 1;
                            }
                            *instruction_area = temp;
                            (*instruction_area)[IC] = 4 | (number << 3);
                            IC++; /* Increment instruction counter for immediate value */
                            break;
                        default:
                            /* Handle default case */
                            temp = (int *)realloc(*instruction_area, (IC + 1) * sizeof(int));
                            if (temp == NULL)
                            {
                                printf("Memory allocation failed in first pass\n");
                                fclose(sorce);
                                return 1;
                            }
                            *instruction_area = temp;
                            (*instruction_area)[IC] = 0;
                            IC++; /* Default instruction increment */
                            break;
                        }
                        break;

                    /* Handle the case where there are 2 operands */
                    case 2:
                        L = 0; /* Reset L for the number of additional instruction words */

                        /* Check the source operand type */
                        sorce_operand_type = check_operand_type(operands[0], symbols_table);
                        if (!is_operand_type_legal(sorce_operand_type, position, 1) && sorce_operand_type != -1)
                        {
                            printf("Error in file %s on line %d: The instruction %s does not support the source operand of operand type %d\n",
                                   sorce_file_name, line_counter, instruction_table[position].instruction_name, sorce_operand_type);
                            *there_is_an_error = 1;
                            break;
                        }

                        /* Check the destination operand type */
                        destination_operand_type = check_operand_type(operands[1], symbols_table);
                        if (!is_operand_type_legal(destination_operand_type, position, 0) && destination_operand_type != -1)
                        {
                            printf("Error in file %s on line %d: The instruction %s does not support the destination operand of operand type %d\n",
                                   sorce_file_name, line_counter, instruction_table[position].instruction_name, destination_operand_type);
                            *there_is_an_error = 1;
                            break;
                        }

                        /* Initialize the instruction number */
                        number = 4;
                        number |= (instruction_table[position].opcode << OPCODE_POSITION);
                        number |= (instruction_table[position].function << FUNCTION_POSITION);

                        /* Set operand types in the instruction */
                        if (sorce_operand_type != -1)
                        {
                            number |= (sorce_operand_type << TYPE_SORCE_ADDRESS_POSITION);
                        }
                        if (destination_operand_type != -1)
                        {
                            number |= (destination_operand_type << TYPE_DESTINATION_ADDRESS_POSITION);
                        }

                        /* Reallocate memory for the instruction area */
                        temp = (int *)realloc(*instruction_area, (IC + 1) * sizeof(int));
                        if (temp == NULL)
                        {
                            printf("Memory allocation failed in first pass\n");
                            fclose(sorce);
                            return 1;
                        }

                        /* Update the instruction in the area */
                        *instruction_area = temp;
                        (*instruction_area)[IC] = number;

                        /* Handle source operand case */
                        switch (sorce_operand_type)
                        {
                        case REGISTER:
                            /* Extract register number for source operand */
                            sscanf(operands[0] + 1, "%d", &number);
                            (*instruction_area)[IC] |= (number << REGISTER_SORCE_POSITION);
                            break;
                        case IMMEDIATE:
                            /* Handle immediate operand */
                            sscanf(operands[0] + 1, "%d", &number);
                            temp = (int *)realloc(*instruction_area, (IC + 2) * sizeof(int));
                            if (temp == NULL)
                            {
                                printf("Memory allocation failed in first pass\n");
                                fclose(sorce);
                                return 1;
                            }
                            *instruction_area = temp;
                            (*instruction_area)[IC + 1] = 4 | (number << 3);
                            L++; /* Increase the number of instructions for immediate operand */
                            break;
                        default:
                            /* Handle default case for source operand */
                            temp = (int *)realloc(*instruction_area, (IC + 2) * sizeof(int));
                            if (temp == NULL)
                            {
                                printf("Memory allocation failed in first pass\n");
                                fclose(sorce);
                                return 1;
                            }
                            *instruction_area = temp;
                            (*instruction_area)[IC + 1] = 0;
                            L++; /* Increase the number of instructions for default operand */
                            break;
                        }

                        /* Handle destination operand case */
                        switch (destination_operand_type)
                        {
                        case REGISTER:
                            /* Extract register number for destination operand */
                            sscanf(operands[1] + 1, "%d", &number);
                            (*instruction_area)[IC] |= (number << REGISTER_DESTINATION_POSITION);
                            break;
                        case IMMEDIATE:
                            /* Handle immediate destination operand */
                            sscanf(operands[1] + 1, "%d", &number);
                            temp = (int *)realloc(*instruction_area, (IC + L + 2) * sizeof(int));
                            if (temp == NULL)
                            {
                                printf("Memory allocation failed in first pass\n");
                                fclose(sorce);
                                free(*instruction_area); /* Free previously allocated memory */
                                return 1;
                            }
                            *instruction_area = temp;
                            (*instruction_area)[IC + L + 1] = 4 | (number << 3);
                            L++; /* Increase the instruction count for immediate operand */
                            break;
                        default:
                            /* Handle default case for destination operand */
                            temp = (int *)realloc(*instruction_area, (IC + L + 2) * sizeof(int));
                            if (temp == NULL)
                            {
                                printf("Memory allocation failed in first pass\n");
                                fclose(sorce);
                                free(*instruction_area); /* Free previously allocated memory */
                                return 1;
                            }
                            *instruction_area = temp;
                            (*instruction_area)[IC + L + 1] = 0;
                            L++; /* Increase instruction count for default operand */
                            break;
                        }

                        /* Increment the instruction counter by L and 1 for both operands */
                        IC += L + 1;
                        break;

                    default:
                        break;
                    }
                }
                memset(operands, 0, sizeof(operands));
            }
        }
    }

    /* Update instruction and data counters */
    *ICF = IC;
    *DCF = DC;
    IC = 100;
    line_counter = L = 0;

    /* Adjust symbol addresses based on instruction counter */
    for (i = 0; i < symbols_table->count; i++)
    {
        if (symbols_table->symbols[i].type == DATA)
        {
            symbols_table->symbols[i].address += (*ICF);
        }
    }

    fclose(sorce);
    return 0;
}

int second_pass(const char *file_name, const mcro_table *table, symbol_table *symbols_table, int **instruction_area, int **data_area, int *ICF, int *DCF, int *there_is_an_error)
{
    char line[82], word[31];
    char *sorce_file_name = malloc(strlen(file_name) + 4);
    char *entry_file_name, *extern_file_name;
    char operands[2][31];                         /*Array to hold operand values (for instruction)*/
    int i, destination_operand_type, sorce_operand_type, position, distance, word_length, line_counter = 0, IC = 100, L = 0, there_enrty_symbol = 0, there_extern_symbol = 0;
    FILE *sorce, *entry_file, *extern_file;
    sprintf(sorce_file_name, "%s.am", file_name); /*Create the source file name with .am extension*/
    /*Open the source file for reading*/
    sorce = fopen(sorce_file_name, "r");
    if (sorce == NULL)
    {
        printf("Error: cannot open %s\n", sorce_file_name);
        *there_is_an_error = 1; /*Set error flag if file cannot be opened*/
        return 1;
    }

    /*Process each line in the source file*/
    while (fgets(line, sizeof(line), sorce))
    {
        L = 0;
        line_counter++;           /*Increment line number*/
        sscanf(line, "%s", word); /*Read the first word (usually the instruction or label)*/
        word_length = strlen(word);
        trim_leading_whitespace(line);    /*Trim leading spaces from the line*/
        strcpy(line, line + word_length); /*Remove the processed word (instruction/label) from the line*/

        /*Handle labels (lines starting with a label followed by a colon)*/
        if (word[word_length - 1] == ':')
        {
            sscanf(line, "%s", word); /*Read the next word*/
            word_length = strlen(word);
            trim_leading_whitespace(line);
            strcpy(line, line + word_length); /* Remove the label*/
        }

        /* Ignore directives like .data, .string, .extern*/
        if ((!strcmp(word, ".data")) || (!strcmp(word, ".string")) || (!strcmp(word, ".extern")))
        {
            continue;
        }

        /*Find instruction type based on the word*/
        position = find_instruction_name(word);
        trim_leading_whitespace(line);
        i = 0;

        if (position != -1) /*If it's an instruction*/
        {
            while (strlen(line)) /*Parse operands*/
            {
                sscanf(line, "%[^,]", word); /* Read the next operand (before comma)*/
                sscanf(word, "%s", word);    /*Trim any additional spaces*/
                word_length = strlen(word);
                if (line[word_length] == ',') /*If there's a comma, skip it*/
                {
                    word_length++;
                }
                trim_leading_whitespace(line);
                strcpy(line, line + word_length);
                trim_leading_whitespace(line);
                strcpy(operands[i], word); /* Store operand in operands array*/
                i++;
            }

            /*Handle the case where there are no operands*/
            switch (i)
            {
            case 0:
                IC++; /*Increase instruction counter*/
                break;

            /*Handle case with one operand (destination operand)*/
            case 1:
                destination_operand_type = check_operand_type(operands[0], symbols_table); /*Get the operand type*/
                if (destination_operand_type == -1)
                {
                    printf("Error: in file %s on line %d: operand not legal\n", sorce_file_name, line_counter);
                    *there_is_an_error = 1; /* Flag error*/
                    break;
                }
                if (!is_operand_type_legal(destination_operand_type, position, 0)) /* Check if it's legal for the instruction*/
                {
                    printf("Error in file %s on line %d: The instruction %s does not support the destination operand of operand type %d\n",
                           sorce_file_name, line_counter, instruction_table[position].instruction_name, destination_operand_type);
                    *there_is_an_error = 1; /* Flag error*/
                    break;
                }
                /*Process the operand based on its type*/
                switch (destination_operand_type)
                {
                case REGISTER:
                    IC++; /*Increase IC for register type operand*/
                    break;
                case IMMEDIATE:
                    IC += 2; /*Immediate operand takes 2 words*/
                    break;
                case DIRECT:
                    (*instruction_area)[IC] |= (DIRECT << TYPE_DESTINATION_ADDRESS_POSITION); /*Set addressing mode*/
                    position = find_symbol_in_table(operands[0], symbols_table);              /*Find symbol in symbol table*/
                    if (symbols_table->symbols[position].type == EXTERNAL)                    /* If external symbol*/
                    {
                        (*instruction_area)[++IC] = 1;
                        if (!there_extern_symbol)
                        {
                            extern_file_name = malloc(strlen(file_name) + 5);
                            sprintf(extern_file_name, "%s.ext", file_name); /* Prepare external symbols file*/
                            extern_file = fopen(extern_file_name, "w");
                            there_extern_symbol = 1;
                        }
                        fprintf(extern_file, "%s %06d\n", symbols_table->symbols[position].symbol_name, IC); /*Write external symbol to file*/
                    }
                    else
                    {
                        (*instruction_area)[++IC] = 2 | (symbols_table->symbols[position].address << 3); /* Use address of symbol*/
                    }
                    IC++; /*Increase IC*/
                    break;
                case RELATIVE:
                    (*instruction_area)[IC] |= (RELATIVE << TYPE_DESTINATION_ADDRESS_POSITION); /*Relative addressing mode*/
                    position = find_symbol_in_table(operands[0] + 1, symbols_table);            /* Remove '+' sign for relative symbol*/
                    distance = ((IC - symbols_table->symbols[position].address) * (-1));        /*Calculate relative distance*/
                    (*instruction_area)[++IC] = 4 | (distance << 3);                            /*Write the instruction with relative distance*/
                    IC++;                                                                       /* Increase IC*/
                    break;
                default:
                    break;
                }
                break;

            /*Handle case with two operands (source and destination)*/
            case 2:
                sorce_operand_type = check_operand_type(operands[0], symbols_table); /*Get source operand type*/
                if (!is_operand_type_legal(sorce_operand_type, position, 1))         /*Check legality for source operand*/
                {
                    printf("Error in file %s on line %d: The instruction %s does not support the source operand of operand type %d\n",
                           sorce_file_name, line_counter, instruction_table[position].instruction_name, sorce_operand_type);
                    *there_is_an_error = 1; /*Flag error*/
                    break;
                }
                destination_operand_type = check_operand_type(operands[1], symbols_table); /* Get destination operand type*/
                if (!is_operand_type_legal(destination_operand_type, position, 0))         /* Check legality for destination operand*/
                {
                    printf("Error in file %s on line %d: The instruction %s does not support the destination operand of operand type %d\n",
                           sorce_file_name, line_counter, instruction_table[position].instruction_name, destination_operand_type);
                    *there_is_an_error = 1; /*Flag error*/
                    break;
                }

                /* Process the source operand*/
                switch (sorce_operand_type)
                {
                case REGISTER:
                    break; /*No action needed for register operand*/
                case IMMEDIATE:
                    L++; /*Immediate operand adds 1 more word*/
                    break;
                case DIRECT:
                    (*instruction_area)[IC] |= (DIRECT << TYPE_SORCE_ADDRESS_POSITION); /*Set source addressing mode*/
                    position = find_symbol_in_table(operands[0], symbols_table);        /* Find source symbol*/
                    if (symbols_table->symbols[position].type == EXTERNAL)              /*If external symbol*/
                    {
                        (*instruction_area)[IC + 1] = 1;
                        if (!there_extern_symbol)
                        {
                            extern_file_name = malloc(strlen(file_name) + 5);
                            sprintf(extern_file_name, "%s.ext", file_name);
                            extern_file = fopen(extern_file_name, "w");
                            there_extern_symbol = 1;
                        }
                        fprintf(extern_file, "%s %06d\n", symbols_table->symbols[position].symbol_name, IC + 1);
                    }
                    else
                    {
                        (*instruction_area)[IC + 1] = 2 | (symbols_table->symbols[position].address << 3);
                    }
                    L++;
                default:
                    break;
                }

                /*Process the destination operand*/
                switch (destination_operand_type)
                {
                case REGISTER:
                    IC += (L + 1);
                    break;
                case IMMEDIATE:
                    IC += (L + 2);
                    break;
                case DIRECT:
                    (*instruction_area)[IC] |= (DIRECT << TYPE_DESTINATION_ADDRESS_POSITION); /*Set destination addressing mode*/
                    position = find_symbol_in_table(operands[1], symbols_table);              /* Find destination symbol*/
                    if (symbols_table->symbols[position].type == EXTERNAL)                    /* If external symbol*/
                    {
                        (*instruction_area)[IC += (L + 1)] = 1;
                        if (!there_extern_symbol)
                        {
                            extern_file_name = malloc(strlen(file_name) + 5);
                            sprintf(extern_file_name, "%s.ext", file_name);
                            extern_file = fopen(extern_file_name, "w");
                            there_extern_symbol = 1;
                        }
                        fprintf(extern_file, "%s %06d\n", symbols_table->symbols[position].symbol_name, IC);
                    }
                    else
                    {
                        (*instruction_area)[IC += (L + 1)] = 2 | (symbols_table->symbols[position].address << 3);
                    }
                    IC++;
                default:
                    break;
                }
            default:
                break;
            }
            memset(operands, 0, sizeof(operands)); /*Reset operand array after processing*/
        }

        /*Process .entry directive*/
        if (!strcmp(word, ".entry"))
        {
            sscanf(line, "%s", word);                             /* Read the entry symbol*/
            position = find_symbol_in_table(word, symbols_table); /*Find symbol in symbol table*/
            if (position == -1)
            {
                printf("Error: in file %s on line %d there is an \".entry\" declaration on an undefined label name. \n", sorce_file_name, line_counter);
                *there_is_an_error = 1; /*Flag error if symbol is not found*/
            }
            else
            {
                symbols_table->symbols[position].is_entry = 1; /*Mark as entry symbol*/
                if (!there_enrty_symbol)
                {
                    entry_file_name = malloc(strlen(file_name) + 5);
                    sprintf(entry_file_name, "%s.ent", file_name); /*Prepare entry symbols file*/
                    entry_file = fopen(entry_file_name, "w");
                    there_enrty_symbol = 1;
                }
                fprintf(entry_file, "%s %06d\n", word, symbols_table->symbols[position].address); /*Write entry symbol to file*/
            }
        }
    }

    fclose(sorce);          /*Close source file*/
    if (there_enrty_symbol) /*Close entry file if used*/
    {
        fclose(entry_file);
    }
    if (there_extern_symbol) /*Close extern file if used*/
    {
        fclose(extern_file);
    }

    /* If there was an error, clean up and remove files*/
    if (*there_is_an_error)
    {
        remove(entry_file_name);
        remove(extern_file_name);
    }

    free(symbols_table->symbols); /*Free symbol table memory*/
    return 0;
}
