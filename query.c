
//
// Created by Theo OMNES on 04/11/2023.
//

#include "query.h"

Query query(Json json, const char *path) {
    (void) json; (void) path;
    return query_left(query_error(qe_NotFound, "Not implemented"));
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