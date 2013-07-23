#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "../parser.h"
#include "../util.h"

void bm(void *m, block *b) {}

void *kc(parser *p, token *t) {
    if (*(t->ptr) == '"') return (void *)"\"";
    if (*(t->ptr) == 'Q') return (void *)"q{'|-}";
    if (*(t->ptr) == '{') return (void *)"c";
    if (*(t->ptr) == 'S') return (void *)"b";
    return NULL;
}

uint8_t *kp(parser *p, void *k) {
    return k;
}

void kr(parser *p, void *k, statement *s, kp_match *m) {
    while (m->node) {
        if (m->node->want == 'b') {
            printf( "Grabbed block:\n" );
            dump_block( m->match.blk, 4 );
            //s->token_idx--;
            //memset( s->tokens + s->token_idx, 0, sizeof(token) );
        }
        else if ( m->node->want == 'c' ) {
            printf( "Matched block\n" );
        }
        else if ( m->node->want == '"' ) {
            printf( "Matched quote:\n*****\n%s\n*****\n", m->match.str );
        }
        else {
            printf( "Matched Something\n" );
        }
        m++;
    }
}

void test_simple();
void test_keyword_block();
void test_stop_match();
void test_quote();

int main() {
    test_stop_match();
    test_simple();
    test_keyword_block();
    test_quote();

    return 0;
}

void test_stop_match() {
    assert(stop_match( "};\n", "}" ));
    assert(stop_match( "", "" ));
    assert(stop_match( "---; xy", "-" ));
    assert(!stop_match( "foo", "o" ));
    assert(!stop_match( "foo", "" ));
}

void test_quote() {
    printf( "\n================\nQuote Keyword test\n" );
    parser p = {
        .meta = NULL,
        .ptr  = NULL,
        .code = "foo bar baz \"\n"
                "inside; \\\\ the \\\"quote\\\"\n"
                "Still in it\" out;\n"
                "Q -foo bar-;\n"
                "Q 'baz bat';\n",

        .push = bm,
        .pop  = bm,

        .kcheck = kc,
        .kpatt  = kp,
        .krun   = kr,

        .error = 0,
        .error_msg = NULL,
        .error_src = NULL
    };
    p.ptr = p.code;

    block *b = gsd_parse_code(&p, "\0");
    printf( "\n" );
    if (!b) {
        printf( "Error (%i): %s\n", p.error, p.error_msg );
    }
    dump_block(b, 0);
}

void test_keyword_block() {
    printf( "\n================\nBlock Keyword test\n" );
    parser p = {
        .meta = NULL,
        .ptr  = NULL,
        .code = "foo bar baz;\n"
                "{ x y; S { z + v }};\n"
                "foo bar baz;\n",

        .push = bm,
        .pop  = bm,

        .kcheck = kc,
        .kpatt  = kp,
        .krun   = kr,

        .error = 0,
        .error_msg = NULL,
    };
    p.ptr = p.code;

    block *b = gsd_parse_code(&p, "\0");
    printf( "\n" );
    dump_block(b, 0);
}

void test_simple() {
    printf( "\n=================\nsimple test\n" );
    parser p = {
        .meta = NULL,
        .ptr  = NULL,
        .code = "foo bar baz;\n"
                "bat + ban -- +-\n"
                "*** x- -x _foo;\n"
                "0123 345 1x123 0f 1.0e-1 10e+1 foo22;"
                "\n\n;;\n\n"
                "gg::gg:x;\n",

        .push = bm,
        .pop  = bm,

        .kcheck = kc,
        .kpatt  = kp,
        .krun   = kr,

        .error = 0,
        .error_msg = NULL,
    };
    p.ptr = p.code;

    block *b = gsd_parse_code(&p, "\0");
    dump_block(b, 0);
}


