#ifndef KEYWORD_H
#define KEYWORD_H

#include <stdint.h>
#include "parser.h"

typedef struct kparser kparser;
typedef struct knode   knode;

struct kparser {
    uint8_t *pattern;
    uint8_t *pointer;

    enum {
        ERROR_NONE = 0,
        ERROR_MEMORY,
        ERROR_SYNTAX,
        ERROR_KEYWORD,
        ERROR_OTHER
    } error;

    uint8_t *error_msg;
};

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

int gsd_keyword_pattern_valid_atom( uint8_t c );
int gsd_keyword_pattern_valid_mod( uint8_t a, uint8_t m );

token *gsd_parse_keyword( parser *p, void *keyword, statement *s, size_t tidx );

kparser *gsd_keyword_pattern_normalize(uint8_t *pattern);
knode   *gsd_keyword_pattern_compile(kparser *p);
knode   *gsd_keyword_pattern_atom(kparser *p);

int gsd_keyword_pattern_match(kparser *p, knode *n);
int gsd_keyword_pattern_delim(kparser *p, knode *n);
int gsd_keyword_pattern_syms(kparser *p, knode *n);
int gsd_keyword_pattern_alt(kparser *p, knode *n);

void free_knode(knode *n);

#endif
