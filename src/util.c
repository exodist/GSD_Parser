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
            free(out.string)
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
            out.string[oi + i] = input.string[ii];
        }
        ii += len;
    }

    assert(oi == 0);

    return out;
}

int parser_check_stop(parser_snip input, parser_snip stop);

parser_snip parser_slurp_until(parser_snip input, parser_snip stop);


