#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistdio.h>

#include "../util.h"
#include "../structures.h"

void get_stop();
void reverse_snip();
void check_stop();
void slurp_until();
void check_char_info();

int main() {
    get_stop();
    reverse_snip();
    check_stop();
    slurp_until();
    check_char_info();

    printf("Looks Good\n");
    return 0;
}

void get_stop() {
    parser_snip open = {
        .string = (uint8_t*)"-{[(<\"`'",
        .size   = 8,
        .idx    = 0
    };

    parser_snip close = parser_get_stop(open);

    uint8_t *want = (uint8_t*)"-}])>\"`'";
    for(int i = 0; i < 8; i++) {
        assert( close.string[i] == want[i] );
        printf( "%c %c\n", open.string[i], close.string[i] );
    }
}

void reverse_snip() {
    parser_snip open = {
        .string = (uint8_t*)"abcd癖efgh",
        .size   = 0,
        .idx    = 0
    };
    open.size = strlen((char *)open.string);

    parser_snip mirr = parser_reverse_snip(open);
    for(int i = 0; i < open.size; i++) {
        if ( mirr.string[i] < 127 ) {
            assert( mirr.string[i] == open.string[open.size - (1 + i)] );
            printf( "%3c %-3c\n", open.string[i], mirr.string[i] );
        }
        else {
            printf( "%3hi %-3hi\n", open.string[i], mirr.string[i] );
        }
    }
}

void check_stop() {
    parser_snip full = {
                           //000000000111111111122222222223
                           //123456789012345678901234567890
        .string = (uint8_t*)"foo bar baz STOP bat STOP HERE; garbage",
        .size   = 0,
        .idx    = 0
    };
    full.size = strlen((char *)full.string);

    parser_snip stop = {
        .string = (uint8_t*)"STOP HERE;",
        .size   = 10,
        .idx    = 0,
    };

    assert (!parser_check_stop(full, stop, 0));

    // This is the first "STOP" which is not complete
    assert (!parser_check_stop(full, stop, 12));
    assert (!parser_check_stop(full, stop, 13));

    // This is where it stops
    assert (parser_check_stop(full, stop, 21));

    // Oops, passed it.
    assert (!parser_check_stop(full, stop, 22));
}

void slurp_until() {
    parser_snip full = {
                           //000000000111111111122222222223
                           //123456789012345678901234567890
        .string = (uint8_t*)"foo bar baz STOP bat STOP HERE; garbage",
        .size   = 0,
        .idx    = 0
    };
    full.size = strlen((char *)full.string);

    parser_snip stop = {
        .string = (uint8_t*)"STOP HERE;",
        .size   = 10,
        .idx    = 0,
    };

    parser_snip result = parser_slurp_until(full, stop);
    assert( result.string == full.string );
    assert( result.idx    == full.idx    );
    assert( result.size   == 22          );
}

void check_char_info() {
    char_info temp = parser_char_info( (uint8_t*)"abc", 3 );
    assert( temp.ucs4 == (uint32_t)'a' );
    assert( temp.utf8[0] == 'a' );
    assert( temp.utf8_length == 1 );
    assert( temp.type == CHAR_ALPHABETIC );

    temp = parser_char_info( (uint8_t*)"{abc", 4 );
    assert( temp.type == CHAR_SYMBOL );

    temp = parser_char_info( (uint8_t*)"\aabc", 4 );
    assert( temp.type == CHAR_CONTROL );

    temp = parser_char_info( (uint8_t*)"\fabc", 4 );
    assert( temp.type == CHAR_CONTROL );

    temp = parser_char_info( (uint8_t*)"4abc", 4 );
    assert( temp.type == CHAR_NUMERIC );

    temp = parser_char_info( (uint8_t*)" abc", 4 );
    assert( temp.type == CHAR_WHITESPACE );

    temp = parser_char_info( (uint8_t*)"\tabc", 4 );
    assert( temp.type == CHAR_WHITESPACE );

    temp = parser_char_info( (uint8_t*)"_abc", 4 );
    assert( temp.type == CHAR_SYMBOL );

    temp = parser_char_info( (uint8_t*)"Δabc", 4 );
    assert( temp.type == CHAR_ALPHABETIC );

    temp = parser_char_info( (uint8_t*)"ܞabc", 4 );
    assert( temp.type == CHAR_ALPHABETIC );

    temp = parser_char_info( (uint8_t*)"⋖abc", 4 );
    assert( temp.utf8_length == 3 );
    assert( temp.type == CHAR_SYMBOL );

    temp = parser_char_info( (uint8_t*)"⑥abc", 4 );
    assert( temp.type == CHAR_SYMBOL );

    temp = parser_char_info( (uint8_t*)"λabc", 4 );
    assert( temp.type == CHAR_ALPHABETIC );

    temp = parser_char_info( (uint8_t*)"؏abc", 4 );
    assert( temp.type == CHAR_SYMBOL );

    temp = parser_char_info( (uint8_t*)"␁abc", 4 );
    assert( temp.type == CHAR_SYMBOL );

    temp = parser_char_info( (uint8_t*)"abc", 4 );
    assert( temp.type == CHAR_CONTROL );

    temp = parser_char_info( (uint8_t*)"abc", 4 );
    assert( temp.type == CHAR_CONTROL );

    temp = parser_char_info( (uint8_t*)"🁘abc", 4 );
    assert( temp.type == CHAR_SYMBOL );
    assert( temp.utf8_length == 4 );
}

