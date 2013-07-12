#ifndef GSD_PARSER_H
#define GSD_PARSER_H

#include <stdint.h>
#include <stdlib.h>
#include "structures.h"

chartype get_char_type( uint8_t *c, chartype last );
uint8_t get_char_length( uint8_t *c );

block *gsd_parse_code( parser *p, uint8_t *stop );
statement *gsd_parse_statement( parser *p );
int gsd_parse_token( token *t, parser *p );

void free_block( block *b );
void free_statement( statement *a );

int gsd_parse_block(     parser *p, knode *n, kp_match *m                );
int gsd_parse_quote(     parser *p, knode *n, kp_match *m                );
int gsd_parse_list(      parser *p, knode *n, kp_match *m                );
int gsd_parse_signature( parser *p, knode *n, kp_match *m                );
int gsd_parse_quote(     parser *p, knode *n, kp_match *m                );
int gsd_parse_kcode(     parser *p, knode *n, kp_match *m                );
int gsd_parse_slurp(     parser *p, knode *n, kp_match *m                );
int gsd_parse_delimited( parser *p, knode *n, kp_match *m                );
int gsd_parse_word(      parser *p, knode *n, kp_match *m                );
int gsd_parse_number(    parser *p, knode *n, kp_match *m                );
int gsd_parse_space(     parser *p, knode *n, kp_match *m                );
int gsd_parse_behind(    parser *p, knode *n, kp_match *m, statement *st );
int gsd_parse_behindns(  parser *p, knode *n, kp_match *m, statement *st );
int gsd_parse_nospace(   parser *p, knode *n, kp_match *m                );

#endif
