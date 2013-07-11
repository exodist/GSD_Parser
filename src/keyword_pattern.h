#ifndef KEYWORD_PATTERN_H
#define KEYWORD_PATTERN_H

#include <stdint.h>
#include "structures.h"

int gsd_keyword_pattern_valid_atom(uint8_t c);
int gsd_keyword_pattern_valid_mod(uint8_t a, uint8_t m);

kparser *gsd_keyword_pattern_normalize(uint8_t *pattern);
knode   *gsd_keyword_pattern_compile(kparser *p);
knode   *gsd_keyword_pattern_atom(kparser *p);

int gsd_keyword_pattern_match(kparser *p, knode *n);
int gsd_keyword_pattern_delim(kparser *p, knode *n);
int gsd_keyword_pattern_syms(kparser *p, knode *n);
int gsd_keyword_pattern_alt(kparser *p, knode *n);

void free_knode(knode *n);
void free_kparser(kparser *kp);

void free_pattern_cache();
int pattern_compare(void *m, void *p1, void *p2);
size_t pattern_loc(size_t sc, void *m, void *p);
void pattern_ref(dict *d, void *ref, int delta);
uint64_t hash_pattern( uint8_t *p );

#endif
