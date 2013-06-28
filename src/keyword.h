#ifndef KEYWORD_H
#define KEYWORD_H

#include <stdint.h>

typedef struct knode     knode;

struct knode {
    uint8_t want;
    uint8_t mod;

    knode *alternate;
    knode *next;

    size_t  length;
    uint8_t *data; // Match for a literal, delimiters for quote
};

knode *compile_keyword(uint8_t *k);
knode *compile_part(uint8_t **k);
knode *compile_item(uint8_t **k);
void dump_knode( knode *n, uint8_t indent );

#endif
