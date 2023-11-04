
//
// Created by Theo OMNES on 04/11/2023.
//

#ifndef C_JSON_PARSER_QUERY_H
#define C_JSON_PARSER_QUERY_H

#include "json.h"

typedef enum QueryErrorType QueryErrorType;
enum QueryErrorType {
    qe_NotFound,
    qe_InvalidPath,
    qe_InvalidJson,
};

typedef struct QueryError QueryError;
struct QueryError {
    QueryErrorType type;
    const char *message;
};

typedef enum QueryTag QueryTag;
enum QueryTag {
    q_Right,
    q_Left,
};

typedef struct Query Query;
struct Query {
    QueryTag tag;
    union {
        Json value;
        QueryError error;
    };
};

Query query(Json json, const char *path);

void free_query(Query *query);

Query query_right(Json json);

Query query_left(QueryError error);

QueryError query_error(QueryErrorType type, const char *message);

#endif //C_JSON_PARSER_QUERY_H
