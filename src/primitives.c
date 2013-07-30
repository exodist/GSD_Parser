#include "primitives.h"

parser_pattern_match parse_space(parser *p) {
    parser_pattern_match out = {
        .type       = MATCH_NONE,
        .value.snip = NULL,
        .flags = {
            .space_prefix  = 0,
            .space_postfix = 0
        }
    };

    

    return out;
}

parser_pattern_match parse_control(parser *p);
parser_pattern_match parse_symbol(parser *p);
parser_pattern_match parse_number(parser *p);
parser_pattern_match parse_alpha(parser *p);
parser_pattern_match parse_alphanum(parser *p);
parser_pattern_match parse_ualphanum(parser *p);

parser_pattern_match parse_literal(parser *p, parser_pattern_node *n);
parser_pattern_match parse_block(parser *p, parser_pattern_node *n);
parser_pattern_match parse_quote(parser *p, parser_pattern_node *n);

parser_pattern_match *parse_delim(parser *p, parser_pattern_node *n, int count);
