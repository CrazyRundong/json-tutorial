#include "leptjson.h"
#include <assert.h>  /* assert() */
#include <stdlib.h>  /* NULL, strtod() */
#include <string.h>

#define EXPECT(c, ch)       do { assert(*c->json == (ch)); c->json++; } while(0)

typedef struct {
    const char* json;
}lept_context;

static void lept_parse_whitespace(lept_context* c) {
    const char *p = c->json;
    while (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r')
        p++;
    c->json = p;
}

/* add: refactoring parse function: true/false/null to literal */
#define PARSE_LIT(c, v, tar) \
    do {\
        EXPECT(c, *tar); /* c->json++ */ \
        for (size_t idx = 0; idx < strlen(tar) - 1; ++idx) { \
            if (c->json[idx] != tar[idx + 1]) \
                return LEPT_PARSE_INVALID_VALUE; \
        } \
        c->json += (strlen(tar) - 1);\
    } while(0)

static int lept_parse_literal(lept_context* c, lept_value* v, const char* tar) {
    PARSE_LIT(c, v, tar);
    switch (*tar) {
        case 't':  v->type = LEPT_TRUE; break;
        case 'f':  v->type = LEPT_FALSE; break;
        default:   v->type = LEPT_NULL;
    }
    return LEPT_PARSE_OK;
}

/*
static int lept_parse_true(lept_context* c, lept_value* v) {
    EXPECT(c, 't');
    if (c->json[0] != 'r' || c->json[1] != 'u' || c->json[2] != 'e')
        return LEPT_PARSE_INVALID_VALUE;
    c->json += 3;
    v->type = LEPT_TRUE;
    return LEPT_PARSE_OK;
}

static int lept_parse_false(lept_context* c, lept_value* v) {
    EXPECT(c, 'f');
    if (c->json[0] != 'a' || c->json[1] != 'l' || c->json[2] != 's' || c->json[3] != 'e')
        return LEPT_PARSE_INVALID_VALUE;
    c->json += 4;
    v->type = LEPT_FALSE;
    return LEPT_PARSE_OK;
}

static int lept_parse_null(lept_context* c, lept_value* v) {
    EXPECT(c, 'n');
    if (c->json[0] != 'u' || c->json[1] != 'l' || c->json[2] != 'l')
        return LEPT_PARSE_INVALID_VALUE;
    c->json += 3;
    v->type = LEPT_NULL;
    return LEPT_PARSE_OK;
}
*/

/* add: parse exp value (int) */
static int double_exp(const char* c) {
    const char *p = c;
    while (*p != 'e' && *p != 'E' && (p - c) < strlen(c))
        ++p;
    if ((p - c) == strlen(c))
        return 0;
    else {
        return atoi(++p);
    }
}

static int lept_parse_number(lept_context* c, lept_value* v) {
    char* end;
    /* \TODO validate number */
    if (*c->json == '+' || /* start with '+' */
            *c->json == '.' || /* start with '.' */
            c->json[strlen(c->json) - 1] == '.' || /* end with '.' */
            (*c->json != '-' && (*c->json) - '0' > 9)) {
        return LEPT_PARSE_INVALID_VALUE;
    } else if ((*c->json == '0' && strlen(c->json) > 1 && c->json[1] != '.')) {
        /* after zero should be '.' or nothing */
        return LEPT_PARSE_ROOT_NOT_SINGULAR;
    } else if (double_exp(c->json) > 308) {
        /* number too big for double */
        return LEPT_PARSE_NUMBER_TOO_BIG;
    }
    v->n = strtod(c->json, &end);
    if (c->json == end)
        return LEPT_PARSE_INVALID_VALUE;
    c->json = end;
    v->type = LEPT_NUMBER;
    return LEPT_PARSE_OK;
}

static int lept_parse_value(lept_context* c, lept_value* v) {
    switch (*c->json) {
        /*case 't':  return lept_parse_true(c, v);
        case 'f':  return lept_parse_false(c, v);
        case 'n':  return lept_parse_null(c, v);*/
        /* add: unified entry for literal parse */
        case 't':  return lept_parse_literal(c, v, "true");
        case 'f':  return lept_parse_literal(c, v, "false");
        case 'n':  return lept_parse_literal(c, v, "null");
        default:   return lept_parse_number(c, v);
        case '\0': return LEPT_PARSE_EXPECT_VALUE;
    }
}

int lept_parse(lept_value* v, const char* json) {
    lept_context c;
    int ret;
    assert(v != NULL);
    c.json = json;
    v->type = LEPT_NULL;
    lept_parse_whitespace(&c);
    if ((ret = lept_parse_value(&c, v)) == LEPT_PARSE_OK) {
        lept_parse_whitespace(&c);
        if (*c.json != '\0') {
            v->type = LEPT_NULL;
            ret = LEPT_PARSE_ROOT_NOT_SINGULAR;
        }
    }
    return ret;
}

lept_type lept_get_type(const lept_value* v) {
    assert(v != NULL);
    return v->type;
}

double lept_get_number(const lept_value* v) {
    assert(v != NULL && v->type == LEPT_NUMBER);
    return v->n;
}
