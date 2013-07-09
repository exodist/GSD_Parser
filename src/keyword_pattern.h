#ifndef KEYWORD_PATTERN_H
#define KEYWORD_PATTERN_H

#include <stdint.h>
#include "structures.h"

int gsd_keyword_pattern_valid_atom( uint8_t c );
int gsd_keyword_pattern_valid_mod( uint8_t a, uint8_t m );

kparser *gsd_keyword_pattern_normalize(uint8_t *pattern);
knode   *gsd_keyword_pattern_compile(kparser *p);
knode   *gsd_keyword_pattern_atom(kparser *p);

int gsd_keyword_pattern_match(kparser *p, knode *n);
int gsd_keyword_pattern_delim(kparser *p, knode *n);
int gsd_keyword_pattern_syms(kparser *p, knode *n);
int gsd_keyword_pattern_alt(kparser *p, knode *n);

void free_knode(knode *n);
void free_kparser(kparser *kp);

#endif
