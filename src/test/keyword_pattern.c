#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "../keyword_pattern.h"
#include "../util.h"

int main() {
    kparser *p = gsd_keyword_pattern_normalize(
        "( b{foo,bar,baz} (w (w (n|q|L)-,-)* (n|q|L)-,-?) -;- | ((w l)|(w w)|l|w) (-;-|-=-) )"
    );
    knode *n = gsd_keyword_pattern_compile(p);

    p = gsd_keyword_pattern_normalize(
        "( b{foo, bar, baz}   (w(w(n|q|L)-,-)* (n|q|L)-,-?)    -;-    |\n\r((w l)|(w w)|l|w)    (-;-|-=-)) "
    );
    knode *n2 = gsd_keyword_pattern_compile(p);

    dump_knode( n, 0, 0 );

    assert( n == n2 );

    free_pattern_cache();
}
