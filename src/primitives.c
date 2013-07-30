#include "primitives.h"

parser_snip parse_space    (parser *p);
parser_snip parse_control  (parser *p);
parser_snip parse_symbol   (parser *p);
parser_snip parse_number   (parser *p);
parser_snip parse_alpha    (parser *p);
parser_snip parse_alphanum (parser *p);
parser_snip parse_ualphanum(parser *p);

parser_snip parse_literal(parser *p, parser_pattern_node *n);
parser_snip parse_block  (parser *p, parser_pattern_node *n);
parser_snip parse_quote  (parser *p, parser_pattern_node *n);

parser_snip parse_delim(parser *p, parser_pattern_node *n, int count);


