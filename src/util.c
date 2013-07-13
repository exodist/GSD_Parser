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

        switch(alt) {
            case 0:  break;
            case 1:  printf( " -->" ); break;
            default: printf( " ?->" );
        }
    }

    printf( "[ %c", n->want );
    if (n->want == '(') printf(")");

    if (n->mod) printf( "%c", n->mod );

    if ( n->want == '-' && n->data.match) {
        printf( " ~ %s", n->data.match );
    }
    if ( n->want == 'q' ) {
        if (n->data.match) {
            printf( " ~ %s", n->data.match );
        }
        else {
            printf( " ~ '\"", n->data.match );
        }
    }
    else if (n->want == 'b' && n->data.syms) {
        printf( " ~ " );
        for ( size_t i = 0; n->data.syms[i]; i++ ) {
            if (i != 0) printf( ", " );
            printf( "%s", n->data.syms[i] );
        }
    }

    printf( " ]\n" );

    if ( n->want == '(' ) {
        uint8_t ind = indent + 4;
        size_t i = 0;
        while ( n->data.alt[i] != NULL ) {
            dump_knode(n->data.alt[i++], ind, n->data.alt[1] ? 2 : 1);
        }
    }

    if ( n->next ) {
        dump_knode(n->next, indent, 0);
    }
}

void dump_token(token *t, int indent) {
    if ( t->block ) {
        dump_block( t->block, indent );
    }
    else {
        for (int i = 0; i < indent; i++) printf( " " );
        printf( t->space_prefix  ? "+" : "-" );
        printf( t->space_postfix ? "+" : "-" );
        printf( " " );

        if (t->is_string) printf( "\"" );
        for( size_t i = 0; i < t->count; i++ ) {
            printf( "%c", t->ptr[i] );
        }
        if (t->is_string) printf( "\"" );
        printf( "\n" );
    }
}

void dump_statement(statement *s, int indent) {
    token *t = s->tokens;
    for (int i = 0; i < indent; i++) printf( " " );
    printf( "[\n" );
    while( t->size ) dump_token(t++, indent + 2);
    for (int i = 0; i < indent; i++) printf( " " );
    printf( "]\n" );
}

void dump_block( block *b, int indent ) {
    for (int i = 0; i < indent; i++) printf( " " );
    printf( "{\n" );
    statement *s = b->statements;
    while (s != NULL) {
        dump_statement(s, indent + 2);
        s = s->next;
    }
    for (int i = 0; i < indent; i++) printf( " " );
    printf( "}\n" );
}
