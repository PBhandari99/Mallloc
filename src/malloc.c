#include <assert.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include "malloc.h"

void* global_base = NULL;

void* malloc(size_t size) {
    struct block_meta* block;

    if (size <= 0) {
        return NULL;
    }

    if (!global_base) { // First allocation
        block = request_space(NULL, size);
        if (!block) {
            return NULL;
        }
        global_base = block;
    }
    else { // We have already allocated blocks previously.
        struct block_meta* last = global_base;
        block = find_free_block(&last, size);
        if (!block) {
            block = request_space(last, size);
            if (!block) {
                return NULL;
            }
        }
        else { // Yes free block.
            // TODO: Split the block if it's bigger.
            block->free = 0;
            block->magic = 0x77777777;
        }
    }
    return (block+1);
}

struct block_meta *find_free_block(struct block_meta **last, size_t size) {
  struct block_meta *current = global_base;
  while (current && !(current->free && current->size >= size)) {
    *last = current;
    current = current->next;
  }
  return current;
}


struct block_meta *request_space(struct block_meta* last, size_t size) {
  struct block_meta *block;
  // Returns a pointer to the top of the heap.
  block = sbrk(0);
  // Returns a pointer to the previous top of the heap(after allocation).
  void *request = sbrk(size + META_SIZE);
  // p points to the top of the heap before new allocation and request points
  // to a previous top of heap after allocation.
  assert((void*)block == request); // Not thread safe.
  if (request == (void*) -1) {
    return NULL; // sbrk failed.
  }
  if (last) { // NULL on first request.
    last->next = block;
  }
  block->size = size;
  block->next = NULL;
  block->free = 0;
  block->magic = 0x12345678;
  return block;
}
