#include <stdlib.h>
#include <string.h>
#include "keyword.h"
#include "parser.h"

#define DELIM_SIZE 4
#define ALT_SIZE   8
#define SYM_SIZE   16
#define MATCH_SIZE 32

void dump_knode( knode *n, uint8_t indent, uint8_t alt );

token *gsd_parse_keyword( parser *p, void *k, statement *s, size_t tidx ) {
    return NULL;
}

int gsd_keyword_pattern_valid_atom( uint8_t c ) {
    switch( c ) {
        case '(': case '-': case 'b': case '"': case 'l': case 'L': case 'S':
        case 'q': case 'c': case 'C': case '.': case 'D': case 'w': case 'n':
        case '_': case '^': case '$': case '<': case '>': case '/': case 't':
            return 1;
        default:
            return 0;
    }
}

int gsd_keyword_pattern_valid_mod( uint8_t a, uint8_t m ) {
    // These are the only valid modifiers
    switch ( m ) {
        case '*': case '?': case '+':
            break;
        default:
            return 0;
    }

    // These atoms cannot have modifiers
    switch ( a ) {
        case 'l': case 'c': case 's': case 'D': case '^': case '$': case '<':
        case '>': case '/': case 't':
            return 0;
    }

    // Everything else goes
    return 1;
}

kparser *gsd_keyword_pattern_normalize(uint8_t *pattern) {
    kparser *ps = malloc(sizeof(kparser));
    if (!ps) return NULL;

    size_t len = strlen(pattern);
    uint8_t *new = malloc(len);
    if (!new) {
        ps->error = ERROR_MEMORY;
        return ps;
    }
    memset(new, 0, len);

    int D_count = 0;
    size_t ni = 0;
    for( size_t pi = 0; pi < len; pi++ ) {
        if( pattern[pi] == ' ' )  continue;
        if( pattern[pi] == '\n' ) continue;
        if( pattern[pi] == '\r' ) continue;
        if( pattern[pi] == '\t' ) continue;
        if( pattern[pi] == 'D' ) D_count++;
        new[ni++] = pattern[pi];
    }

    if (D_count == 1) {
        ps->error = ERROR_KEYWORD;
        ps->error_msg = "Delimiter specified in keyword pattern, but only used once.";
        free(new);
        return ps;
    }

    ps->pattern = new;
    ps->pointer = new;
    return ps;
}

knode *gsd_keyword_pattern_compile(kparser *p) {
    knode *start = NULL;
    knode *curr  = NULL;
    while (*(p->pointer) != '\0' && *(p->pointer) != '|' && *(p->pointer) != ')') {
        knode *n = gsd_keyword_pattern_atom(p);
        if (!n) {
            free_knode( start );
            p->error = ERROR_MEMORY;
            return NULL;
        }
        if (!start) start = n;
        if (curr) {
            curr->next = n;
            curr = n;
        }
        else {
            curr = n;
        }
    }

    return start;
}

knode *gsd_keyword_pattern_atom(kparser *p) {
    if (! gsd_keyword_pattern_valid_atom(*(p->pointer))) {
        p->error = ERROR_KEYWORD;
        p->error_msg = "Invalid atom in keyword pattern";
        return NULL;
    }

    knode *n = malloc( sizeof(knode) );
    if (!n) {
        p->error = ERROR_MEMORY;
        return NULL;
    }
    memset(n, 0, sizeof(knode));

    n->want = *(p->pointer);
    p->pointer++;
    if (*(p->pointer) == '\0') {
        if (n->want == ')') {
            p->error = ERROR_KEYWORD;
            p->error_msg = "Unexpected end of pattern";
        }
        return n;
    }

    int success = 1;
    if (n->want == '-') {
        success = gsd_keyword_pattern_match(p, n) ;
    }
    else if (n->want == '(') {
        success = gsd_keyword_pattern_alt(p, n);
    }
    else if (n->want == 'q' && *(p->pointer) == '{') {
        success = gsd_keyword_pattern_delim(p, n);
    }
    else if (n->want == 'b' && *(p->pointer) == '{') {
        success = gsd_keyword_pattern_syms(p, n);
    }

    if (!success) {
        p->error = ERROR_MEMORY;
        free( n );
        return NULL;
    }

    if (gsd_keyword_pattern_valid_mod(n->want, *(p->pointer))) {
        n->mod = *(p->pointer);
        p->pointer++;
    }

    return n;
}

uint8_t *gsd_keyword_pattern_string(kparser *p, uint8_t *term, size_t s) {
    size_t size = s;
    uint8_t *new = malloc(size);
    if (!new) {
        p->error = ERROR_MEMORY;
        return NULL;
    }
    memset( new, 0, size );

    size_t ni = 0;
    while (1) {
        for( size_t ti = 0; term[ti] != '\0'; ti++ ) {
            if ( *(p->pointer) == term[ti] ) return new;
        }

        if (ni >= size - 1) {
            size += s;
            void *c = realloc(new, size);
            if (!c) {
                free( new );
                p->error = ERROR_MEMORY;
                return NULL;
            }
            new = c;
            memset( new + ni, 0, s );
        }

        new[ni++] = *(p->pointer);
        p->pointer++;
    }

    return new;
}

int gsd_keyword_pattern_match(kparser *p, knode *n) {
    uint8_t *new = gsd_keyword_pattern_string( p, "-", MATCH_SIZE );
    if (!new) {
        p->error = ERROR_MEMORY;
        return 0;
    }

    n->data.match = new;
    p->pointer++;

    return 1;
}

int gsd_keyword_pattern_delim(kparser *p, knode *n) {
    if (*(p->pointer) == '{') *(p->pointer++);
    uint8_t *new = gsd_keyword_pattern_string( p, "}", MATCH_SIZE );
    if (!new) {
        p->error = ERROR_MEMORY;
        return 0;
    }

    n->data.match = new;
    p->pointer++;

    return 1;
}

int gsd_keyword_pattern_syms(kparser *p, knode *n) {
    if (*(p->pointer) == '{') p->pointer++;

    size_t count = SYM_SIZE;
    uint8_t **syms = malloc(sizeof(uint8_t *) * count);
    if (!syms) {
        p->error = ERROR_MEMORY;
        return 0;
    }
    memset( syms, 0, sizeof(uint8_t *) * count );

    size_t si = 0;
    while ( 1 ) {
        uint8_t *s = gsd_keyword_pattern_string( p, ",}", SYM_SIZE );
        if (!s) goto SYMS_CLEANUP;

        if (si >= count - 1) {
            count += SYM_SIZE;
            void *c = realloc(syms, sizeof(uint8_t *) * count);
            if (!c) goto SYMS_CLEANUP;
            syms = c;
            memset( syms + si, 0, sizeof(uint8_t *) * SYM_SIZE );
        }

        syms[si++] = s;
        if ( *(p->pointer) == '}' ) break;
        p->pointer++;
    }

    p->pointer++;
    n->data.syms = syms;
    return 1;

    SYMS_CLEANUP:
    p->error = ERROR_MEMORY;
    si = 0;
    while (syms[si] != NULL) free( syms[si++] );
    free(syms);
    return 0;
}

int gsd_keyword_pattern_alt(kparser *p, knode *n) {
    size_t count = ALT_SIZE;
    knode **alt = malloc(sizeof(knode *) * count);
    if (!alt) return 0;
    memset( alt, 0, sizeof(knode *) * count);

    size_t ai = 0;
    while (*(p->pointer) != ')') {
        if (*(p->pointer) == '|') p->pointer++;
        knode *n = gsd_keyword_pattern_compile(p);
        if (!n) goto ALT_CLEANUP;

        if (ai >= count - 1) {
            count += ALT_SIZE;
            void *c = realloc(alt, sizeof(knode *) * count);
            if (!c) goto ALT_CLEANUP;
            alt = c;
            memset( alt + ai, 0, sizeof(knode *) * ALT_SIZE );
        }

        alt[ai++] = n;
    }

    p->pointer++;
    n->data.alt = alt;
    return 1;

    ALT_CLEANUP:
    ai = 0;
    while (alt[ai] != NULL) free( alt[ai++] );
    free( alt );
    return 0;
}

void free_knode(knode *n) {
    if( n->want == '(' ) {
        size_t i = 0;
        while ( n->data.alt[i] != NULL ) {
            free_knode(n->data.alt[i++]);
        }
        free( n->data.alt );
    }
    else if (n->want == '-') {
        free( n->data.match );
    }
    else if (n->want == 'q' && n->data.delim) {
        free( n->data.delim );
    }
    else if (n->want == 'b' && n->data.syms != NULL) {
        size_t i = 0;
        while ( n->data.syms[i] != NULL ) {
            free(n->data.syms[i++]);
        }
        free( n->data.syms );
    }

    if( n->next ) free_knode( n->next );

    free(n);
}

#include <stdio.h>

void dump_knode( knode *n, uint8_t indent, uint8_t alt ) {
    if (indent) {
        uint8_t end = alt ? indent - 4 : indent;

        for( uint8_t x = 0; x < end; x++ ) {
            printf( " " );
        }

        if (alt) {
            printf(" ?->");
        }
    }

    printf(
        "{ '%c', '%c', ",
        n->want,
        n->mod
    );

    if ( n->want == '-' || n->want == 'q' ) {
        if (n->data.match) {
            printf( "'%s'", n->data.match );
        }
    }
    else if (n->want == 'b' && n->data.syms) {
        for ( size_t i = 0; n->data.syms[i]; i++ ) {
            if (i > 0) printf( ", " );
            printf( "'%s'", n->data.syms[i] );
        }
    }
    else {
        printf( "NA" );
    }

    printf( " }\n" );

    if ( n->want == '(' ) {
        uint8_t ind = indent + 4;
        size_t i = 0;
        while ( n->data.alt[i] != NULL ) {
            dump_knode(n->data.alt[i++], ind, 1);
        }
    }

    if ( n->next ) {
        dump_knode(n->next, indent, 0);
    }
}

int main() {
    //uint8_t *kw = "(w|q{'\"}|(-a-|b))*q?b{foo,bar}?-foobar baz-t";
    uint8_t *kw = "( (w (w (n|q|L)-,-)* (n|q|L)-,-?) -;- | ((w l)|(w w)|l|w) (-;-|-=-) )";
    printf( "\n%s\n", kw );
    kparser *p = gsd_keyword_pattern_normalize(kw);
    knode *kn = gsd_keyword_pattern_compile(p);
    dump_knode(kn, 0, 0);
    free_knode( kn );
    return 0;
}
