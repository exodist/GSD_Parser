#include <stdlib.h>
#include <string.h>
#include "keyword.h"
#include "parser.h"

knode *compile_keyword(uint8_t *k) {
    return compile_part( &k );
}

knode *compile_part(uint8_t **k) {
    uint8_t **ptr = k;
    knode *start = compile_item(k);
    if (start == NULL ) return NULL;

    knode *current = start;
    while(**k != '\0' && **k != '|' && **k != ')') {
        current->next = compile_item(k);
        current = current->next;
        while (**k == ' ') (*k)++;

        if (current == NULL) {
            current = start;
            while (current) {
                knode *n = current;
                current = current->next;
                free(n);
            }
            return NULL;
        }
    }

    return start;
}

knode *compile_item(uint8_t **k) {
    while (**k == ' ') (*k)++;

    if (**k == '(') {
        knode *start = malloc(sizeof(knode));
        if ( start == NULL ) return NULL;
        memset( start, 0, sizeof(knode) );
        start->want = '(';

        knode *curr = start;
        while (**k == ' ') (*k)++;
        while (**k == '(' || **k == '|') {
            (*k)++;
            curr->alternate = compile_part(k);
            curr = curr->alternate;
            while (**k == ' ') (*k)++;

            if (curr == NULL) {
                curr = start;
                while (curr) {
                    knode *n = curr;
                    curr = curr->alternate;
                    free(n);
                }
            }
            while (**k == ' ') (*k)++;
        }
        (*k)++;

        if (**k == '*' || **k == '?') {
            start->mod = **k;
            (*k)++;
        }

        return start;
    }

    knode *n = malloc( sizeof(knode));
    if (n == NULL) return NULL;
    memset( n, 0, sizeof(knode) );

    n->want = **k;

    if (**k == '-') {
        n->data = ++(*k);
        while ( **k != '-' ) {
            (*k)++;
            n->length++;
        }
        (*k)++;
    }
    else if ((*k)[1] == '{') {
        (*k) += 2;
        n->data = *k;
        while ( **k != '}' ) {
            (*k)++;
            n->length++;
        }
        (*k)++;
    }
    else {
        (*k)++;
    }

    if (**k == '*' || **k == '?') {
        n->mod = **k;
        (*k)++;
    }

    return n;
}

#include <stdio.h>

void dump_knode( knode *n, uint8_t indent ) {
    for( uint8_t x = 0; x < indent; x++ )
        printf( " " );

    printf(
        "{ w:%c m:%c d:",
        n->want,
        n->mod
    );

    for( size_t l = 0; l < n->length; l++ )
        printf( "%c", n->data[l] );
        
    printf( " }\n" );

    if ( n->alternate ) {
        uint8_t i = indent;
        if (n->want == '(') i += 4;
        dump_knode(n->alternate, i);
    }
    if ( n->next ) {
        dump_knode(n->next, indent);
    }
}

int main() {
    uint8_t *kw = "(w|q{'\"}|(a|b))*p?b{foo bar}?-foobar baz-t";
    printf( "\n%s\n", kw );
    knode *kn = compile_keyword(kw);
    dump_knode(kn, 0);
    return 0;
}
