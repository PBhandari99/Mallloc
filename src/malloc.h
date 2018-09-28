#include <sys/types.h>

struct block_meta {
    size_t size;
    struct block_meta* next;
    int free;
    int magic; // for debugging only. TODO: remove this in non-debug mode.
};

#define META_SIZE sizeof(struct block_meta)

void* malloc(size_t size);
struct block_meta* find_free_block(struct block_meta**, size_t);
struct block_meta* request_space(struct block_meta*, size_t);
