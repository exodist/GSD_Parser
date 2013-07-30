#ifndef GSD_PARSER_STRUCTURES_H
#define GSD_PARSER_STRUCTURES_H

#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>

#include "GSD_Dict/src/include/gsd_dict.h"
#include "GSD_Dict/src/include/gsd_dict_return.h"

typedef struct parser parser;

typedef struct parser_block     parser_block;
typedef struct parser_config    parser_config;
typedef struct parser_error     parser_error;
typedef struct parser_pattern   parser_pattern;
typedef struct parser_quote     parser_quote;
typedef struct parser_snip      parser_snip;
typedef struct parser_state     parser_state;

typedef struct parser_pattern_match parser_pattern_match;
typedef struct parser_pattern_node  parser_pattern_node;

typedef int(parser_block_callback)(parser *p, parser_block *b, uint8_t **symbols, size_t symbols_count);
typedef parser_pattern *(parser_keyword_check)(parser *p, parser_snip *s);

struct parser_snip {
    uint8_t *string;
    size_t   size;
    size_t   idx;
};

struct parser_state {
    size_t pos;
    size_t line;
    parser_block *current;
};

struct parser_error {
    enum {
        PARSER_ERROR_NONE = 0,
        PARSER_ERROR_MEMORY,
        PARSER_ERROR_SYNTAX,
        PARSER_ERROR_OTHER
    } category;

    const uint8_t *message;
    parser_state   state;
};

struct parser_config {
    void *meta;
    parser_keyword_check  *check;
    parser_block_callback *push;
    parser_block_callback *pop;

    uint8_t **terminators;
    uint8_t   terminators_count;

    dict *patterns;
    dict *pattern_cache;
};

struct parser {
    parser_config config;

    uint8_t *filename;
    uint8_t *code;

    parser_state *state;
    parser_block *root;
    parser_error *error;

    void *meta;
};

struct parser_block {
    parser_block *parent;

    uint8_t *filename;
    size_t   start_line;
    size_t   end_line;

    parser_pattern_match **content;
    size_t                 content_count;

    void *meta;
};

struct parser_quote {
    uint8_t *value;
    size_t   size;
    size_t   length;

    uint8_t *delimiter;
    size_t   delimiter_size;
    size_t   delimiter_length;
};

struct parser_pattern {
    uint8_t *raw;
    size_t   raw_size;

    parser_pattern_node *nodes;
    size_t               nodes_size;
    size_t               node_idx;
};

struct parser_pattern_node {
    enum {
        NODE_MOD_NONE = 0,
        NODE_MOD_MULTI, // +
        NODE_MOD_ANY,   // *
        NODE_MOD_MAYBE  // ?
    } mod;

    struct {
        uint8_t no_capture;
        uint8_t keyword_check;
    } flags;

    enum {
        NODE_WANT_ALPHA,
        NODE_WANT_ALPHANUM,
        NODE_WANT_ALT,
        NODE_WANT_BLOCK,
        NODE_WANT_CONTROL,
        NODE_WANT_DELIM,
        NODE_WANT_INLINE,
        NODE_WANT_NAMED,
        NODE_WANT_NOSPACE,
        NODE_WANT_NUMBER,
        NODE_WANT_QUOTE,
        NODE_WANT_SPACE,
        NODE_WANT_SYMBOL,
        NODE_WANT_UALPHANUM
    } want;

    union {
        parser_snip match;

        struct {
            parser_pattern_node *values;
            size_t count;
        } alt;

        struct {
            parser_snip *delimiters;
            size_t       delimiters_count;
        } quote;

        struct {
            parser_snip    *name;
            parser_pattern *pattern;
        } named;

        struct {
            parser_snip *delimiters;
            size_t       delimiters_count;

            parser_snip **symbols;
            size_t        symbols_count;
        } block;
    } data;
};

struct parser_pattern_match {
    enum {
        MATCH_NONE = 0,
        MATCH_ALPHA,
        MATCH_ALPHANUM,
        MATCH_BLOCK,
        MATCH_CONTROL,
        MATCH_DELIM,
        MATCH_INLINE,
        MATCH_NOSPACE,
        MATCH_NUMBER,
        MATCH_QUOTE,
        MATCH_SPACE,
        MATCH_SYMBOL,
        MATCH_UALPHANUM
    } type;

    union {
        parser_block *block;
        parser_quote *quote;
        parser_snip  *snip;
    } value;

    struct {
        uint8_t space_prefix;
        uint8_t space_postfix;
    } flags;
};

#endif

