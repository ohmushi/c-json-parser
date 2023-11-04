
//
// Created by Theo OMNES on 04/11/2023.
//

#include "query.h"

Query query(Json json, const char *path) {
    if (json.type == j_empty) return query_left(query_error(qe_NotFound, "Empty json"));
    (void) path;
    return query_right(json.values[0]);
}

QueryError query_error(QueryErrorType type, const char *message) {
    return (QueryError) {
            .type = type,
            .message = message,
    };
}

Query query_left(QueryError error) {
    return (Query) {
            .tag = q_Left,
            .error = error,
    };
}

Query query_right(Json json) {
    return (Query) {
            .tag = q_Right,
            .value = json,
    };
}