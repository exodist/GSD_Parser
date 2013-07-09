#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "util.h"

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

