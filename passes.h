#ifndef PASSES_H
#define PASSES_H

/*Enum for symbol types*/
typedef enum symbol_type
{
    CODE,        /*Symbol type for code (instructions)*/
    DATA,        /*Symbol type for data (variables, constants)*/
    EXTERNAL     /*Symbol type for external symbols (externally defined)*/
} symbol_type;

/*Structure representing a symbol*/
typedef struct symbol
{
    char symbol_name[31];  /*Name of the symbol (max 30 characters + null terminator)*/
    symbol_type type;      /*Type of the symbol (CODE, DATA, EXTERNAL)*/
    int is_entry;          /*Flag indicating if the symbol is an entry point*/
    int address;           /*Memory address or offset of the symbol*/
} symbol;

/*Structure representing a symbol table*/
typedef struct symbol_table
{
    symbol *symbols;  /*Array of symbols*/
    int count;         /*Number of symbols in the table*/
} symbol_table;

#endif /*PASSES_H*/
