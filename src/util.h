#ifndef GSD_PARSER_UTIL
#define GSD_PARSER_UTIL

#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "structures.h"

parser_snip parser_get_stop(parser_snip input);
parser_snip parser_reverse_snip(parser_snip input);

int parser_check_stop(parser_snip input, parser_snip stop);

parser_snip parser_slurp_until(parser_snip input, parser_snip stop);

#endif
