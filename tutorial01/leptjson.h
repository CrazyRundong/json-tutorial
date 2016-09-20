#ifndef LEPTJSON_H__
#define LEPTJSON_H__

typedef enum { LEPT_NULL, LEPT_FALSE, LEPT_TRUE, LEPT_NUMBER, LEPT_STRING, LEPT_ARRAY, LEPT_OBJECT } lept_type;

typedef struct {
    lept_type type;
}lept_value;

typedef enum {
    LEPT_PARSE_OK = 0,  /* no error during parse */
    LEPT_PARSE_EXPECT_VALUE,  /* all json contents are spaces */
    LEPT_PARSE_INVALID_VALUE,  /* invalid value */
    LEPT_PARSE_ROOT_NOT_SINGULAR  /* value after second space */
} lept_flag;

int lept_parse(lept_value* v, const char* json);

lept_type lept_get_type(const lept_value* v);

#endif /* LEPTJSON_H__ */
