#include <string.h>
#include "query.h"

Query query(Json json, const char *path) {
    if (json.type == j_empty) return query_left(query_error(qe_NotFound, "Empty json"));
    PathKey key = next_key(path);
    if (key.start == NULL) return query_left(query_error(qe_InvalidPath, "Path should start with /"));
    uint8_t key_len = (uint8_t) (key.end - key.start);
    if (key_len == 0) return query_left(query_error(qe_InvalidPath, "Empty key"));
    for (size_t i = 0; i < json.nb_elements; i++) {
        if (strlen(json.keys[i]) == key_len && strncmp(json.keys[i], key.start, key_len) == 0) {
            return query_right(json.values[i]);
        }
    }
    return query_left(query_error(qe_NotFound, "Key not found"));
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

PathKey next_key(const char *path) {
    if (path == NULL) return (PathKey) {.start = NULL, .end = NULL};
    if (path[0] != '/') return (PathKey) {.start = NULL, .end = NULL};
    char *end = strchr(path + 1, '/');
    if (end == NULL) end = strchr(path, '\0');
    return (PathKey) {
            .start = (char *)path + 1,
            .end = end,
    };
}