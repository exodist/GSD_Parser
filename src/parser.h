#ifndef GSD_PARSER_H
#define GSD_PARSER_H

#include <stdint.h>
#include <stdlib.h>

typedef struct dict      dict;
typedef struct token     token;
typedef struct statement statement;
typedef struct block     block;

typedef uint8_t  (block_mod)(void *meta, block *b);
typedef uint8_t *(keyword_check)(void *meta, token *t);

typedef enum { SPACE_C, WORD_C, SYM_C, CONTROL_C, TERM_C } chartype;

struct token {
    size_t   size;
    size_t   count;
    uint8_t *ptr;
    enum { BLOCK_T, LITERAL_T, SYMBOL_T, OPERATOR_T, RUNBLOCK_T } type;
    token *next;

    uint8_t space_prefix;
    uint8_t space_postfix;
};

struct statement {
    token *tokens;
    statement *next;
};

struct block {
    statement *statements;
    dict      *symbols;
};

chartype get_char_type( uint8_t *c );
uint8_t get_char_length( uint8_t *c );

block *gsd_parse( uint8_t *start, size_t length, void *meta, block_mod push, block_mod pop, keyword_check c );
statement *gsd_parse_statement( uint8_t **start, size_t *length, void *meta, keyword_check c );
token *gsd_parse_token( uint8_t **start, size_t *length, void *meta, keyword_check c );
token *gsd_parse_keyword( uint8_t *k, uint8_t **start, size_t *length, void *meta, keyword_check c, statement *s, token *t );

#endif
