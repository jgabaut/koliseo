#include <stdint.h>
#include <stdio.h>
/* FNV-1a hash */
static uint64_t my_hash_str(const char *s, size_t len) {
    const uint8_t *p = (const uint8_t *)s;
    uint64_t h = 14695981039346656037ULL;
    for (size_t i = 0; i < len; i++) {
        h ^= (uint64_t)p[i];
        h *= 1099511628211ULL;
    }
    printf("%s() %s -> #[%llu]\n", __func__, s, h);
    return h;
}
#define HASHMAP_HASH my_hash_str
#define HASHMAP_T int
#define HASHMAP_NAME hash_map_int
#define HASHMAP_PREFIX hashmap_int_
#include "hashmap.h"

#define HASHMAP_HASH_MURMUR2
#define HASHMAP_T char
#define HASHMAP_NAME hash_map_char
#define HASHMAP_PREFIX hashmap_char_
#include "hashmap.h"

int main(void) {

    Koliseo* kls = kls_new(KLS_DEFAULT_SIZE*4);

    hash_map_int *map = hashmap_int_new(kls, 256);

    int x = 42;
    int y = 420;
    hashmap_int_push(map, "answer", &x);
    hashmap_int_push(map, "num", &y);
    int *v = hashmap_int_get(map, "answer");
    if (v) printf("answer: %d\n", *v);
    hashmap_int_remove(map, "answer");
    v = hashmap_int_get(map, "answer");
    if (v) printf("answer: %d\n", *v);

    for (size_t i = 0; i < map->bucket_count; i++) {
        da_hash_map_int_node* node = map->buckets[i];
        for (size_t j = 0; j < node->count; j++) {
            printf("%s (#%llu): %i\n", node->items[j].key, i, *(node->items[j].value));
        }
    }

    hash_map_char *c_map = hashmap_char_new(kls, 256);
    char c = 'w';
    char d = 'a';
    hashmap_char_push(c_map, "foo", &c);
    hashmap_char_push(c_map, "bar", &d);

    for (size_t i = 0; i < c_map->bucket_count; i++) {
        da_hash_map_char_node* node = c_map->buckets[i];
        for (size_t j = 0; j < node->count; j++) {
            printf("%s (#%llu): %c\n", node->items[j].key, i, *(node->items[j].value));
        }
    }

    kls_free(kls);
    return 0;
}
