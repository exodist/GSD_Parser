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

// TODO: Unicode
uint8_t *get_char_pair( uint8_t *c, size_t len ) {
    uint8_t *buff = malloc(len + 1);
    if (!buff) return NULL;
    memset( buff, 0, len + 1 );

    if ( len == 1 ) {
        switch (*c) {
            case '{': sprintf( buff, "}" ); return buff;
            case '(': sprintf( buff, ")" ); return buff;
            case '<': sprintf( buff, ">" ); return buff;
            case '[': sprintf( buff, "]" ); return buff;
        }
    }

    for (size_t i = 0; i < len && c[i] != '\0'; i++) {
        buff[i] = c[i];
    }

    return buff;
}

int stop_match( uint8_t *ptr, uint8_t *stop ) {
    if (ptr[0] == '\0' && stop[0] == '\0') return 1;
    if (ptr[0] == '\0' || stop[0] == '\0') return 0;

    for( size_t i = 0; ptr[i] != '\0' && stop[i] != '\0'; i++ ) {
        if (ptr[i] != stop[i]) return 0;
    }

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
        statement *s = gsd_parse_statement( p, stop );
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

        if (stop && stop_match(p->ptr, stop)) {
            (p->ptr) += strlen(stop);
            break;
        }
    }

    if (p->pop) p->pop(p->meta, b);
    return b;
}

statement *gsd_parse_statement( parser *p, uint8_t *stop ) {
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

    while (1) {
        if (s->token_idx >= s->token_count) {
            s->token_count += TLEN;
            void *check = realloc(s->tokens, sizeof(token) * s->token_count);
            if (check = NULL) {
                free_statement( s );
                return NULL;
            }
            memset(s->tokens + s->token_count - TLEN, 0, sizeof(token) * TLEN);
        }

        token *t = s->tokens + s->token_idx++;
        int c = gsd_parse_token( t, p, stop );

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
            int term = gsd_parse_keyword( p, k, s );
            if (term > 0) break;
            if (term < 0) {
                free_statement( s );
                return NULL;
            }
        }
    }

    return s;
}

int gsd_parse_token( token *t, parser *p, uint8_t *stop ) {
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
        if ( stop && stop_match(p->ptr, stop) ) return 0;

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

int gsd_parse_block( parser *p, knode *n, kp_match *m, statement *st ) {
    // Skip whitespace and newlines
    chartype ty = get_char_type( p->ptr, NONE_C );
    while (ty == SPACE_C) {
        p->ptr += get_char_length( p->ptr );
        ty = get_char_type( p->ptr, NONE_C );
    }
    // check for '{'
    if (p->ptr[0] != '{') return 0;
    p->ptr++;

    block *b = gsd_parse_code( p, "}" );
    if (!b) return 0;

    m->match.blk = b;

    return 1;
}

int gsd_parse_quote( parser *p, knode *n, kp_match *m, statement *st ) {
}

int gsd_parse_list( parser *p, knode *n, kp_match *m, statement *st ) {
}

int gsd_parse_signature( parser *p, knode *n, kp_match *m, statement *st ) {
}

int gsd_parse_kcode( parser *p, knode *n, kp_match *m, statement *st ) {
    token *t = st->tokens + st->token_idx - 1;
    uint8_t *stop = get_char_pair( t->ptr, t->size );
    block *b = gsd_parse_code( p, stop );
    free(stop);
    if (!b) return 0;

    t->block = b;

    return 1;
}

int gsd_parse_slurp( parser *p, knode *n, kp_match *m, statement *st ) {
}

int gsd_parse_delimited( parser *p, knode *n, kp_match *m, statement *st ) {
}

int gsd_parse_word( parser *p, knode *n, kp_match *m, statement *st ) {
}

int gsd_parse_number( parser *p, knode *n, kp_match *m, statement *st ) {
}

int gsd_parse_space( parser *p, knode *n, kp_match *m, statement *st ) {
}

int gsd_parse_behind( parser *p, knode *n, kp_match *m, statement *st ) {
}

int gsd_parse_behindns( parser *p, knode *n, kp_match *m, statement *st ) {
}

int gsd_parse_nospace( parser *p, knode *n, kp_match *m, statement *st ) {
}

