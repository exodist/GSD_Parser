#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include "parser.h"
#include "keyword.h"
#include "structures.h"
#include "GSD_Dict/src/include/gsd_dict.h"
#include "GSD_Dict/src/include/gsd_dict_return.h"

#define TLEN 20

// TODO: Unicode
chartype get_char_type( uint8_t *c, chartype last ) {
    if ( *c == ' '  || *c == '\t' ) return SPACE_C;
    if ( *c == '\n' || *c == '\r' ) return SPACE_C;

    if ( *c == '_' ) return WORD_C;
    if ( *c >= 'a'  && *c <= 'z'  ) return WORD_C;
    if ( *c >= 'A'  && *c <= 'Z'  ) return WORD_C;
    if ( *c >= '0'  && *c <= '9'  ) return WORD_C;
    if ( *c == '.'  && last == WORD_C ) return WORD_C;
    if ( *c == '-'  && last == WORD_C ) return WORD_C;
    if ( *c == '+'  && last == WORD_C ) return WORD_C;

    if ( *c == ';'  || *c == '\0' ) return TERM_C;
    assert(!( *c  < 32   || *c >= 127  ));
    return SYM_C;
}

// TODO: Unicode
uint8_t get_char_length( uint8_t *c ) {
    return 1;
}

block *gsd_parse_code( parser *p, uint8_t *stop ) {
    block *b = malloc(sizeof(block));
    if (!b) return NULL;
    memset(b, 0, sizeof(block));
    if (stop && !strcmp(p->ptr, stop)) return b;
    if (p->push) p->push(p->meta, b);

    statement *last = NULL;
    while (1) {
        statement *s = gsd_parse_statement( p );
        if (!s) {
            if (p->pop) p->pop(p->meta, b);
            free_block( b );
            return NULL;
        }

        if (!b->statements) b->statements = s;
        if (last) last->next = s;
        last = s;

        while (get_char_type(p->ptr, 0) == SPACE_C)
            p->ptr += get_char_length(p->ptr);

        if (stop && !strcmp(p->ptr, stop)) {
            (p->ptr) += strlen(stop);
            break;
        }
    }

    if (p->pop) p->pop(p->meta, b);
    return b;
}

statement *gsd_parse_statement( parser *p ) {
    printf( "=====\n%s\n=====\n", p->ptr );
    statement *s = malloc(sizeof(statement));
    if (!s) return NULL;
    memset(s, 0, sizeof(statement));
    s->token_count = TLEN;

    s->tokens = malloc(sizeof(token) * s->token_count);
    if (!s->tokens) {
        free(s);
        return NULL;
    }
    memset(s->tokens, 0, sizeof(token) * s->token_count);

    size_t tidx = 0;
    while (1) {
        if (tidx >= s->token_count) {
            s->token_count += TLEN;
            void *check = realloc(s->tokens, sizeof(token) * s->token_count);
            if (check = NULL) {
                free_statement( s );
                return NULL;
            }
            memset(s->tokens + s->token_count - TLEN, 0, sizeof(token) * TLEN);
        }

        token *t = s->tokens + tidx++;
        int c = gsd_parse_token( t, p );

        // Handle Errors
        if (c < 0) {
            free_statement( s );
            return NULL;
        }

        // Handle Statement Terminator
        if (c == 0) break;

        // Handle keywords
        void *k = p->kcheck(p, t);
        if (k) {
            int term = gsd_parse_keyword( p, k, s, &tidx );
            if (term > 0) break;
            if (term < 0) {
                free_statement( s );
                return NULL;
            }
        }
    }

    return s;
}

int gsd_parse_token( token *t, parser *p ) {
    chartype ty = get_char_type( p->ptr, 0 );
    if (ty == SPACE_C) {
        t->space_prefix = 1;
        while (ty == SPACE_C) {
            (p->ptr) += get_char_length( p->ptr );
            ty = get_char_type( p->ptr, 0 );
        }
    }

    ty     = get_char_type( p->ptr, 0 );
    t->ptr = p->ptr;
    while ( 1 ) {
        uint8_t len = get_char_length( p->ptr );
        (t->count)++;
        (p->ptr)  += len;
        (t->size) += len;

        chartype n = get_char_type( p->ptr, ty );
        if (n != ty || ty == TERM_C) break;
    }

    if ( ty == TERM_C ) return 0;

    ty = get_char_type( p->ptr, 0 );
    if (ty == SPACE_C) t->space_postfix = 1;

    return 1;
}

void free_statement( statement *s ) {
    if (s->tokens) free( s->tokens );
    if (s->next) free_statement(s->next);
    free(s);
}

void free_block( block *b ) {
    if (b->statements)
        free_statement( b->statements );

    if (b->symbols)
        dict_free(&(b->symbols));

    free(b);
}

int gsd_parse_block( parser *p, knode *n, kp_match *m ) {
}

int gsd_parse_quote( parser *p, knode *n, kp_match *m ) {
}

int gsd_parse_list( parser *p, knode *n, kp_match *m ) {
}

int gsd_parse_signature( parser *p, knode *n, kp_match *m ) {
}

int gsd_parse_kcode( parser *p, knode *n, kp_match *m ) {
}

int gsd_parse_slurp( parser *p, knode *n, kp_match *m ) {
}

int gsd_parse_delimited( parser *p, knode *n, kp_match *m ) {
}

int gsd_parse_word( parser *p, knode *n, kp_match *m ) {
}

int gsd_parse_number( parser *p, knode *n, kp_match *m ) {
}

int gsd_parse_space( parser *p, knode *n, kp_match *m ) {
}

int gsd_parse_behind( parser *p, knode *n, kp_match *m, statement *st ) {
}

int gsd_parse_behindns( parser *p, knode *n, kp_match *m, statement *st ) {
}

int gsd_parse_nospace( parser *p, knode *n, kp_match *m ) {
}

