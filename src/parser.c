#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "parser.h"
#include "keyword.h"

// TODO: Unicode
chartype get_char_type( uint8_t *c ) {
    if ( *c >= 'a'  && *c <= 'z'  ) return WORD_C;
    if ( *c >= 'A'  && *c <= 'Z'  ) return WORD_C;
    if ( *c >= '0'  && *c <= '9'  ) return WORD_C;
    if ( *c == ' '  || *c == '\t' || *c == '\n' || *c == '\r' ) return SPACE_C;
    if ( *c == ';' ) return TERM_C;
    if ( *c  < 32   || *c >= 127  ) return CONTROL_C;
    return SYM_C;
}

// TODO: Unicode
uint8_t get_char_length( uint8_t *c ) {
    return 1;
}

block *gsd_parse( uint8_t *start, size_t length, void *meta, block_mod push, block_mod pop, keyword_check c ) {
    block *b = malloc(sizeof(block));
    if (!b) return NULL;
    memset(b, 0, sizeof(block));

    if (push) push(meta, b);
    statement *s = gsd_parse_statement( &start, &length, meta, c );
    while (s) {
        if (b->statements) {
            b->statements->next = s;
        }
        else {
            b->statements = s;
        }
        s = gsd_parse_statement( &start, &length, meta, c );
    }

    if (pop) pop(meta, b);
    return b;
}

statement *gsd_parse_statement( uint8_t **c_ptr, size_t *c_rem, void *meta, keyword_check c ) {
    statement *s = malloc(sizeof(statement));
    if (!s) return NULL;
    memset(s, 0, sizeof(statement));

    token *t = gsd_parse_token( c_ptr, c_rem, meta, c );
    while (t && *(t->ptr) != ';') {
        uint8_t *k = c( meta, t );
        if (k) {
            t = gsd_parse_keyword( k, c_ptr, c_rem, meta, c, s, t );
        }
        if (s->tokens) {
            s->tokens->next = t;
        }
        else {
            s->tokens = t;
        }
        token *t = gsd_parse_token( c_ptr, c_rem, meta, c );
    }

    return s;
}

token *gsd_parse_token( uint8_t **c_ptr, size_t *c_rem, void *meta, keyword_check c ) {
    token *t = malloc(sizeof(token));
    if (!t) return NULL;
    memset(t, 0, sizeof(token));

    if( !*c_rem ) return t;
    chartype ty = get_char_type( *c_ptr );
    if (ty == SPACE_C) t->space_prefix = 1;
    while (ty == SPACE_C || ty == TERM_C) {
        size_t cl = get_char_length( *c_ptr );
        if ( *c_rem < cl ) {
            *c_rem = 0;
            return t;
        }
        (*c_rem) -= cl;
        (*c_ptr) += cl;
        if( !*c_rem ) return t;
        chartype ty = get_char_type( *c_ptr );
    }

    t->ptr = *c_ptr;
    size_t size  = 0;
    size_t count = 0;
    while (ty == get_char_type( *c_ptr )) {
        size_t cl = get_char_length( *c_ptr );
        count++;
        size += cl;
        (*c_ptr) += cl;
    }
    t->size  = size;
    t->count = count;

    if (ty == SPACE_C) t->space_postfix = 1;
    return t;
}

token *gsd_parse_keyword( uint8_t *k, uint8_t **c_ptr, size_t *c_rem, void *meta, keyword_check c, statement *s, token *t ) {
    knode *kn = compile_keyword(k);    
}
