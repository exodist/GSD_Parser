#ifndef KEYWORD_H
#define KEYWORD_H

#include <stdint.h>
#include "structures.h"

int gsd_parse_keyword( parser *p, void *keyword, statement *s );
kp_match *gsd_process_keyword(parser *p, knode *n, statement *st);

knode_stack *push_knode_stack(knode_stack **s, knode *n, parser *p);
knode *pop_knode_stack(knode_stack **s);
void rollback_knode_stack(knode_stack **s);

void free_knode_stack(knode_stack *s);

const char *gsd_keyword_error(knode *n);

int gsd_match_knode(parser *p, knode *n, kp_match *m, statement *st);

#endif

