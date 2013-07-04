#ifndef GSD_PARSER_H
#define GSD_PARSER_H

#include <stdint.h>
#include <stdlib.h>

typedef struct dict      dict;
typedef struct token     token;
typedef struct statement statement;
typedef struct block     block;
typedef struct parser    parser;

typedef uint8_t  (block_mod)(void *meta, block *b);
typedef void    *(keyword_check)(parser *p, token *t);
typedef uint8_t *(keyword_pattern)(parser *p, void *k);

typedef enum { NONE_C = 0, SPACE_C, WORD_C, SYM_C, CONTROL_C, TERM_C } chartype;

struct parser {
    void *meta;
    uint8_t *code;

    uint8_t *ptr;

    block_mod *push;
    block_mod *pop;

    keyword_check   *kcheck;
    keyword_pattern *kpatt;
};

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
    size_t     token_count;
    token     *tokens;
    statement *next;
};

struct block {
    statement *statements;
    dict      *symbols;
};

chartype get_char_type( uint8_t *c, chartype last );
uint8_t get_char_length( uint8_t *c );

block *gsd_parse_code( parser *p, uint8_t *stop );
statement *gsd_parse_statement( parser *p );
int gsd_parse_token( token *t, parser *p );

void free_block( block *b );
void free_statement( statement *a );

#endif
