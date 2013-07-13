#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "../parser.h"
#include "../util.h"

void bm(void *m, block *b) {}

void *kc(parser *p, token *t) {
    if (*(t->ptr) == '{') return (void *)1;
    return NULL;
}

uint8_t *kp(parser *p, void *k) { return "c"; }

void kr(parser *p, void *k, statement *s, kp_match *m) {
}

void simple();
void keyword_block();

int main() {
    assert(stop_match( "};\n", "}" ));
    assert(stop_match( "", "" ));
    assert(stop_match( "---; xy", "-" ));
    assert(!stop_match( "foo", "o" ));
    assert(!stop_match( "foo", "" ));

    //simple();
    keyword_block();

    return 0;
}

void keyword_block() {
    parser p = {
        .meta = NULL,
        .ptr  = NULL,
        .code = "foo bar baz;\n"
                "{ x y; z };\n"
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
    dump_block(b, 0);
   
}

void simple() {
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


