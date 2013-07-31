#include <unictype.h>
#include <unistr.h>
#include <unitypes.h>
#include <assert.h>
#include "util.h"

parser_snip parser_get_stop(parser_snip input) {
    parser_snip out = {
        .string = NULL,
        .size   = input.size,
        .idx    = 0
    };

    out.string = malloc(out.size);
    if (!out.string) {
        out.size = 0;
        return out;
    }

    size_t ii = input.idx;
    size_t oi = 0;
    while (ii < input.size) {
        ucs4_t orig = 0;
        int ilen = u8_mbtouc(&orig, input.string + ii, input.size - ii);
        ii += ilen;

        ucs4_t mirror = 0;
        uc_mirror_char(orig, &mirror);

        uint8_t miu8[4];
        int olen = u8_uctomb(miu8, mirror, 4);

        if (out.size - oi < olen) {
            void *check = realloc(out.string, out.size + 16);
            if (!check) {
                free(out.string);
                out.size = 0;
                return out;
            }
            out.string = check;
            out.size  += 16;
        }

        for (int i = 0; i < olen; i++) {
            out.string[oi++] = miu8[i];
        }
    }

    if (out.size > oi) {
        void *check = realloc(out.string, oi);
        if (!check) {
            out.size = 0;
            free(out.string);
            return out;
        }
        out.string = check;
        out.size = oi;
    }

    return out;
}

parser_snip parser_reverse_snip(parser_snip input) {
    parser_snip out = {
        .string = NULL,
        .size   = input.size - input.idx,
        .idx    = 0
    };
    out.string = malloc(out.size);
    if (!out.string) {
        out.size = 0;
        return out;
    }

    size_t ii = input.idx;
    size_t oi = input.size;
    while (ii < input.size) {
        int len = u8_mblen(input.string + ii, input.size - ii);
        assert(len);

        oi -= len;
        assert(oi >= 0);
        for (int i = 0; i < len; i++) {
            out.string[oi + i] = input.string[ii + i];
        }
        ii += len;
    }

    assert(oi == 0);

    return out;
}

int parser_check_stop(parser_snip input, parser_snip stop, size_t offset) {
    size_t a = input.size - (input.idx + offset);
    size_t b = stop.size  - stop.idx;
    for(size_t i = 0; i < a && i < b; i++) {
        if (input.string[input.idx + i + offset] != stop.string[stop.idx + i])
            return 0;
    }

    return 1;
}

parser_snip parser_slurp_until(parser_snip input, parser_snip stop) {
    parser_snip out = input;
    out.size = out.idx + 1;
    for (size_t i = 0; out.size <= input.size; i++) {
        if (parser_check_stop(input, stop, i))
            return out;
        out.size++;
    }

    out.idx    = 0;
    out.size   = 0;
    out.string = NULL;

    return out;
}

char_info parser_char_info(uint8_t *start, size_t length) {
    char_info out = {
        .ucs4 = 0,
        .utf8 = { 0, 0, 0, 0 },
        .utf8_length = 0,
        .type = CHAR_INVALID,
    };

    if (!length) return out;
    if (!start)  return out;

    out.utf8_length = u8_mbtouc(&(out.ucs4), start, length);
    assert( out.utf8_length <= 4 );
    for (int i = 0; i < out.utf8_length; i++) {
        out.utf8[i] = start[i];
    }

    switch(out.ucs4) {
        case ' ':
        case '\t':
        case '\n':
        case '\r':
            out.type = CHAR_WHITESPACE;
        break;
    }

    if (out.type == CHAR_INVALID) {
        if (uc_is_general_category(out.ucs4, UC_CATEGORY_L)) {
            out.type = CHAR_ALPHABETIC;
        }
        else if (uc_is_general_category(out.ucs4, UC_CATEGORY_Nd)) {
            out.type = CHAR_NUMERIC;
        }
        else if (uc_is_general_category(out.ucs4, UC_CATEGORY_Z)) {
            out.type = CHAR_WHITESPACE;
        }
        else if (uc_is_general_category(out.ucs4, UC_CATEGORY_C)){
            out.type = CHAR_CONTROL;
        }
        else {
            out.type = CHAR_SYMBOL;
        }
    }

    return out;
}
