#ifndef STRUCTURES_H
#define STRUCTURES_H

#include <stdint.h>
#include <stdlib.h>

typedef struct block        block;
typedef struct dict         dict;
typedef struct knode        knode;
typedef struct kparser      kparser;
typedef struct parser       parser;
typedef struct statement    statement;
typedef struct token        token;
typedef struct kp_match     kp_match;
typedef struct knode_stack  knode_stack;
typedef struct pattern_type pattern_type;

typedef void     (block_mod)(void *meta, block *b);
typedef void    *(keyword_check)(parser *p, token *t);
typedef uint8_t *(keyword_pattern)(parser *p, void *k);
typedef void     (keyword_run)(parser *p, void *k, statement *s, kp_match *matches);

typedef enum {
    NONE_C = 0,
    SPACE_C,
    WORD_C,
    SYM_C,
    CONTROL_C,
    TERM_C
} chartype;

typedef enum {
    ERROR_NONE = 0,
    ERROR_MEMORY,
    ERROR_SYNTAX,
    ERROR_KEYWORD,
    ERROR_OTHER
} parse_error;

struct kp_match {
    knode   *node;
    size_t   match_size;
    union {
        uint8_t *str;
        block   *blk;
    } match;
};

struct knode_stack {
    knode_stack *parent;
    size_t idx;
    size_t alt;
    size_t cnt;
    knode *node;
    uint8_t *ptr;
};

struct pattern_type {
    enum {
        PTYPE_KPARSER = 0,
        PTYPE_KNODE   = 1
    } type : 32;

    int32_t refcount;
};

struct kparser {
    pattern_type type;

    uint8_t *pattern;
    uint8_t *ptr;

    parse_error error;
    const uint8_t *error_msg;
    uint8_t *error_src;
};

struct knode {
    pattern_type type;

    uint8_t want;
    uint8_t mod;

    knode *next;

    union {
        knode   **alt;
        uint8_t  *match;
        uint8_t  *delim;
        uint8_t **syms;
    } data;
};

struct parser {
    void *meta;
    uint8_t *code;

    uint8_t *ptr;

    block_mod *push;
    block_mod *pop;

    keyword_check   *kcheck;
    keyword_pattern *kpatt;
    keyword_run     *krun;

    parse_error error;
    const uint8_t *error_msg;
    uint8_t *error_src;
};

struct token {
    size_t   size;
    size_t   count;
    uint8_t *ptr;
    block   *block;
    uint8_t  space_prefix;
    uint8_t  space_postfix;
    uint8_t  is_string;
};

struct statement {
    size_t     token_count;
    size_t     token_idx;
    token     *tokens;
    statement *next;
};

struct block {
    statement *statements;
    dict      *symbols;
};

#endif
