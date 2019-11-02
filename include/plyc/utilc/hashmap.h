#ifndef UTILC_HASHMAP_H
#define UTILC_HASHMAP_H

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

/**
 * Creates a HashMap with the name (name) and the types for key and value (key_type) (value_type).
 * The function hashfunction should take a key and return an unsigned hash number.
 * The function keycmp_function should return true, if both key parameters are equal.
 * The function keycpy_function should copy the input key into the return value.
 * The function keykill_function should free the given key.
 * 
 * Construct a Map with <name> map; <name>_new(&map, approx_size);.
 * Kill it with <name>_kill.
 * <name>_get returns an pointer to a value for the given key (if not available yet, it will create a new entry).
 * <name>_remove removes the value under the given key, if available.
 */
#define HashMap(name, key_type, value_type, hash_function, keycmp_function, keycpy_function, keykill_function)\
struct name ## _Item_ {\
    key_type key;\
    value_type value;\
    struct name ## _Item_ *next;\
};\
typedef struct {\
    struct name ## _Item_ **map;\
    int map_size;\
} name;\
void name ## _new(name *self, int approx_size) {\
    self->map = (struct name ## _Item_ **) calloc(approx_size, sizeof(struct name ## _Item_ *));\
    self->map_size = approx_size;\
}\
void name ## _kill(name  *self) {\
    for (int i = 0; i < self->map_size; i++) {\
        struct name ## _Item_ *item = self->map[i];\
        while (item) {\
            struct name ## _Item_ *next = item->next;\
            keykill_function(item->key);\
            free(item);\
            item = next;\
        }\
    }\
    free(self->map);\
    self->map = NULL;\
    self->map_size = 0;\
}\
value_type *name ## _get(const name *self, key_type key) {\
    unsigned hash = hash_function(key) % self->map_size;\
    struct name ## _Item_ **item = &self->map[hash];\
    while (*item && !keycmp_function(key, (*item)->key))\
        item = &(*item)->next;\
    if (!*item) {\
        *item = (struct name ## _Item_ *) malloc(sizeof(struct name ## _Item_));\
        (*item)->key = keycpy_function(key);\
        (*item)->next = NULL;\
    }\
    return &(*item)->value;\
}\
void name ## _remove(const name *self, key_type key) {\
    unsigned hash = hash_function(key) % self->map_size;\
    struct name ## _Item_ **item = &self->map[hash];\
    while (*item && !keycmp_function(key, (*item)->key))\
        item = &(*item)->next;\
    if (*item) {\
        struct name ## _Item_ *kill = *item;\
        *item = (*item)->next;\
        keykill_function(kill->key);\
        free(kill);\
    }\
}

/** impl function for StrHashMap */
static unsigned StrHashMap_impl_hash(const char *key) {
    unsigned hash = 5381;
    int c;
    while ((c = *key++))
        hash = ((hash << 5u) + hash) + c; /* hash * 33 + c */
    return hash;
}

/** impl function for StrHashMap */
static bool StrHashMap_impl_keycmp(const char *a, const char *b) {
    return strcmp(a, b) == 0;
}

/** impl function for StrHashMap */
static const char *StrHashMap_impl_keycpy(const char *cpy) {
    char *result = (char *) malloc(strlen(cpy) + 1);
    strcpy(result, cpy);
    return result;
}

/** impl function for StrHashMap */
static void StrHashMap_impl_keykill(const char *key) {
    free((void *) key);
}


/**
 * Creates a string HashMap with the name (name) and the type for value (value_type).
 * 
 * Construct a Map with <name> map; <name>_new(&map, approx_size);.
 * Kill it with <name>_kill.
 * <name>_get returns an pointer to a value for the given string key (if not available yet, it will create a new entry).
 * <name>_remove removes the value under the given string key, if available.
 */
#define StrHashMap(name, value_type) HashMap(name, const char *, value_type,\
StrHashMap_impl_hash, StrHashMap_impl_keycmp, StrHashMap_impl_keycpy, StrHashMap_impl_keykill)


#endif //UTILC_HASHMAP_H
