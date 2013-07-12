#include <stdlib.h>
#include <string.h>
#include "keyword_pattern.h"
#include "keyword.h"
#include "parser.h"

int gsd_parse_keyword( parser *p, void *keyword, statement *s, size_t *tidx ) {
    uint8_t *patt = p->kpatt(p, keyword);
    if (!patt) {
        p->error = ERROR_KEYWORD;
        p->error_msg = "Could not get pattern from keyword";
        return -1;
    }

    kparser *kp = gsd_keyword_pattern_normalize(patt);
    if (!kp) {
        p->error = ERROR_MEMORY;
        return -1;
    }

    knode *n = gsd_keyword_pattern_compile(kp);
    if (!n) {
        p->error     = kp->error;
        p->error_msg = kp->error_msg;
        free_kparser(kp);
        return -1;
    }

    kp_match *matches = gsd_process_keyword(p, n, s);
    if (!matches) {
        free_knode(n);
        if (p->error) return -1;
        p->error = ERROR_KEYWORD;
        p->error_msg = "Syntax Error";
        return -1;
    }
    p->krun(p, keyword, s, tidx, matches);

    // See if we need to terminate the statement
    int out = 0;
    size_t midx = 0;
    while (matches[midx + 1].node) midx++;
    if ( matches[midx].node->want == 't' ) out = 1;

    free(matches);
    free_knode(n);

    return out;
}

kp_match *gsd_process_keyword(parser *p, knode *n, statement *st) {
    knode_stack *s = NULL;
    void *check = push_knode_stack(&s, n, p);
    if (!check) {
        p->error = ERROR_MEMORY;
        return NULL;
    }

    int       rollback = 0;
    size_t    idx      = 0;
    size_t    length   = 0;
    kp_match *matches  = NULL;

    while( s ) {
        if (!length || idx >= length - 1) {
            length += 16;
            void *check = realloc( matches, length * sizeof(kp_match) );
            if (!check) {
                if (matches) free(matches);
                free_knode_stack(s);
                p->error = ERROR_MEMORY;
                return NULL;
            }
            matches = check;
            memset( matches + s->idx, 0, length - s->idx );
        }

        int match = 0;
        if (s->node->want == '(') {
            // Try next alternate
            if ((rollback || !s->alt) && (s->node->data.alt[s->alt]->want)) {
                rollback = 0;
                idx = s->idx;
                void *check = push_knode_stack(&s, s->node->data.alt[s->alt++], p);
                if (!check) {
                    p->error = ERROR_MEMORY;
                    break;
                }
                continue;
            }

            match == !rollback;
            rollback = 0;
        }
        else {
            match = gsd_match_knode(p, s->node, matches + idx, st);
        }

        if (match) idx++;
        char mod = s->node->mod;

        // Multiple Occurence, loop again
        if (match && (mod == '+' || mod == '*')) {
            s->cnt++;
            s->alt = 0;
            // Bump idx and ptr so that we do not rollback the valid matches so far.
            s->idx = idx;
            s->ptr = p->ptr;
            continue;
        }

        // Not a match, and matching is not optional
        if (!match && !(mod == '*' || mod == '?' || s->cnt)) {
            if (s->node->want != '(') {
                p->error = ERROR_SYNTAX;
                p->error_msg = gsd_keyword_error(s->node);
            }
            rollback = 1;
            rollback_knode_stack(&s);
            continue;
        }

        // If there is another node in the chain, swap
        // nodes.
        if (s->node->next) {
            s->node = s->node->next;
            s->ptr  = p->ptr;
            s->idx = 0;
            s->alt = 0;
            s->cnt = 0;
            continue;
        }

        // We did it!
        pop_knode_stack(&s);
        p->error = ERROR_NONE;
        p->error_msg = NULL;
    }

    if (s) free_knode_stack(s);
    if (p->error) {
        if (matches) free(matches);
        return NULL;
    }

    // Null Terminate
    memset( matches + idx, 0, sizeof(kp_match) );
    return matches;
}

knode_stack *push_knode_stack(knode_stack **s, knode *n, parser *p) {
    knode_stack *ns = malloc(sizeof(knode_stack));
    if (!ns) return NULL;
    memset(ns, 0, sizeof(knode_stack));
    ns->parent = *s;
    ns->node = n;
    ns->ptr  = p->ptr;
    *s = ns;
    return ns;
}

knode *pop_knode_stack(knode_stack **s) {
    knode_stack *t = *s;
    *s = t->parent;
    knode *n = t->node;
    free(t);
    return n;
}

void free_knode_stack(knode_stack *s) {
    while (s != NULL) pop_knode_stack(&s);
}

void rollback_knode_stack(knode_stack **s) {
    pop_knode_stack(s);
    while ((*s)->node->want != '(') pop_knode_stack(s);
}

const char *gsd_keyword_error(knode *n) {
    switch(n->want) {
        case 'b': return "Expected block";
        case '"': return "Expected quote";
        case 'l': return "Expected list";
        case 'L': return "Expected parameter list";
        case 's': return "Expected signature";
        case 'S': return "Expected signature";
        case 'q': return "Expected quoted text";
        case 'c': return "Expected code";
        case 'C': return "Expected code";
        case '.': return "Expected stuff";
        case 'D': return "Expected delimiter";
        case 'w': return "Expected bareword";
        case 'n': return "Expected number";
        case '_': return "Expected whitespace";
        case '^': return "Expected new line";
        case '$': return "Expected end of line";
        case '<': return "Expected preceeding tokens";
        case '>': return "Expected preceeding token";
        case '/': return "Unexpected whitespace";
        case '(': return "Unexpected token";
        default:  return "Unknown Error";
    }
}

int gsd_match_knode(parser *p, knode *n, kp_match *m, statement *st) {
    switch(n->want) {
        case 'b': return gsd_parse_block(p, n, m);
        case '"': return gsd_parse_quote(p, n, m);
        case 'l': return gsd_parse_list(p, n, m);
        case 'L': return gsd_parse_list(p, n, m);
        case 's': return gsd_parse_signature(p, n, m);
        case 'S': return gsd_parse_signature(p, n, m);
        case 'q': return gsd_parse_quote(p, n, m);
        case 'c': return gsd_parse_kcode(p, n, m);
        case 'C': return gsd_parse_kcode(p, n, m);
        case '.': return gsd_parse_slurp(p, n, m);
        case 'D': return gsd_parse_delimited(p, n, m);
        case 'w': return gsd_parse_word(p, n, m);
        case 'n': return gsd_parse_number(p, n, m);
        case '_': return gsd_parse_space(p, n, m);
        case '<': return gsd_parse_behind(p, n, m, st);
        case '>': return gsd_parse_behindns(p, n, m, st);
        case '/': return gsd_parse_nospace(p, n, m);
        default:  return 0;
    }
}

