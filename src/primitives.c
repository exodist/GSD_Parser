#include "primitives.h"
#include "util.h"

parser_pattern_match parse_type(parser *p, int type, int match) {
    parser_pattern_match out = {
        .type       = MATCH_NONE,
        .value.snip = { .string = NULL, .size = 0, .idx = 0   },
        .flags      = { .space_prefix = 0, .space_postfix = 0 }
    };

    parser_state *s = &(p->state);
    
    uint8_t *start = p->text + s->pos;
    size_t   size  = p->length - s->pos;
    char_info i = parser_char_info(start, size);

    if (type != CHAR_WHITESPACE && i.type == CHAR_WHITESPACE) {
        out.flags.space_prefix = 1;

        // Slurp up the whitespace, ignore the result.
        parse_type(p, CHAR_WHITESPACE, MATCH_SPACE);

        i = parser_char_info(start, size);
    }

    if (i.type == type) {
        // We did match
        out.type = match;
        // Set snip start
        out.value.snip.string = start;

        // keep going until a character that is not space
        size_t count = 1;
        while (size) {
            s->pos += i.utf8_length;
            count  += i.utf8_length;
            start  += i.utf8_length;
            size   -= i.utf8_length;

            i = parser_char_info(start, size);

            if (i.type != type) {
                if (i.type == CHAR_WHITESPACE)
                    out.flags.space_postfix = 1;

                break;
            }
        }

        out.value.snip.size = count;
    }

    return out;
}

parser_pattern_match parse_space(parser *p) {
    return parse_type(p, CHAR_WHITESPACE, MATCH_SPACE);
}

parser_pattern_match parse_control(parser *p) {
    return parse_type(p, CHAR_CONTROL, MATCH_CONTROL);
}

parser_pattern_match parse_symbol(parser *p) {
    return parse_type(p, CHAR_SYMBOL, MATCH_SYMBOL);
}

parser_pattern_match parse_digit(parser *p) {
    return parse_type(p, CHAR_NUMERIC, MATCH_NUMBER);
}

parser_pattern_match parse_alpha(parser *p) {
    return parse_type(p, CHAR_ALPHABETIC, MATCH_ALPHA);
}

parser_pattern_match parse_number(parser *p);
parser_pattern_match parse_alphanum(parser *p);


parser_pattern_match parse_ualphanum(parser *p);

parser_pattern_match parse_literal(parser *p, parser_pattern_node *n);
parser_pattern_match parse_block(parser *p, parser_pattern_node *n);
parser_pattern_match parse_quote(parser *p, parser_pattern_node *n);

parser_pattern_match *parse_delim(parser *p, parser_pattern_node *n, int count);
