#ifndef KEYWORD_H
#define KEYWORD_H

#include <stdint.h>
#include "parser.h"

typedef struct kparser kparser;
typedef struct knode   knode;

//struct kparser {
//    uint8_t *pattern;
//    uint8_t *pointer;
//
//    enum {
//        ERROR_NONE = 0,
//        ERROR_MEMORY,
//        ERROR_SYNTAX,
//        ERROR_KEYWORD,
//        ERROR_OTHER
//    } error;
//
//    uint8_t *error_msg;
//};

struct knode {
    uint8_t want;
    uint8_t mod;

    knode *next;

    union {
        knode   **alt;
        uint8_t  *match;
        uint8_t  *delim;
        uint8_t **syms;
    } data;
};

token *gsd_parse_keyword( parser *p, void *keyword, statement *s, size_t tidx );

uint8_t *gsd_keyword_pattern_normalize(parser *ps, uint8_t *pattern);
knode   *gsd_keyword_pattern_compile(parser *ps, uint8_t **pattern);
knode   *gsd_keyword_pattern_atom(parser *ps, uint8_t **pattern);

int gsd_keyword_pattern_valid_atom( uint8_t c );
int gsd_keyword_pattern_valid_mod( uint8_t a, uint8_t m );
int gsd_keyword_pattern_match(parser *ps, knode *n, uint8_t **p);
int gsd_keyword_pattern_delim(parser *ps, knode *n, uint8_t **p);
int gsd_keyword_pattern_syms(parser *ps, knode *n, uint8_t **p);
int gsd_keyword_pattern_alt(parser *ps, knode *n, uint8_t **p);

void free_knode(knode *n);

#endif
