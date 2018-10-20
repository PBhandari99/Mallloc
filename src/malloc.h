#ifndef MALLOC_H
#define MALLOC_H

#include <string.h>
#include <sys/types.h>

struct allocated_meta {
    size_t size;
};

struct free_meta {
    size_t size;
    struct free_meta* prev;
    struct free_meta* next;
};

#define FREE_META_SIZE sizeof(struct free_meta) // 24 bytes
#define ALLOCATED_META_SIZE sizeof(struct allocated_meta) // 8 bytes


void* malloc(size_t size);
struct allocated_meta* find_free_block(size_t);
struct allocated_meta* request_space(size_t);
void split_block(struct free_meta*, size_t, size_t);

void free(void*);
void insert_block_to_list(void*);

#endif
