#include <assert.h>
#include <sys/types.h>
#include <unistd.h>

#include "malloc.h"

// Head of the free memory block linked list.
void* global_base = NULL;

void* malloc(size_t size) {
    struct allocated_meta* block;

    if (size <= 0) {
        return NULL;
    }
    if (!global_base) { // if no free list.
        block = request_space(size);
        if (!block) {
            return NULL;
        }
        global_base = block;
    }
    else { // We have have free blocks.
        block = find_free_block(size);
        if (!block) {
            block = request_space(size);
            if (!block) {
                return NULL;
            }
        }
    }
    // +1 to only return pointer from the data region and hide the meta data.
    return (block+1);
}

struct allocated_meta* find_free_block(size_t size) {
    struct free_meta* current_best_match = NULL;
    struct free_meta* current = global_base;
    size += ALLOCATED_META_SIZE;
    while (current) {
        if (!current_best_match) {
            if (current->size >= size) {
            current_best_match = current;
            }
        }
        else {
            if (current->size >= size && (current->size - size < current_best_match->size -size)) {
                current_best_match = current;
            }
        }
        current = current->next;
    }
    // free block found.
    if (current_best_match) {
        // Split the block if it's bigger.
        split_block(current_best_match, current_best_match->size, size);
        ((struct allocated_meta*)current_best_match)->size = size;
        return (struct allocated_meta*)current_best_match;
    }
    return NULL;
}

// Reconfigure the free block chain and split if the block is bigger
// then required.
void split_block(struct free_meta* block, size_t initial_size, size_t final_size) {
    void* new_free_block;
    if (initial_size == final_size) { // no split is required.
        if (!block->prev)
            block->next->prev = block->prev;
        else if (!block->next)
            block->prev->next = block->next;
        else {
            block->next->prev = block->prev;
            block->prev->next = block->next;
        }
    }
    else {
        new_free_block = ((char*)block) + (final_size - 1);
        if (initial_size - final_size >= FREE_META_SIZE) {
            // Connect to the previous block.
            block->prev->next = new_free_block;
            ((struct free_meta*)new_free_block)->prev = block->prev;
            // Connect to the next block.
           ((struct free_meta*)new_free_block)->next = block->next;
            block->next->prev = new_free_block;

            // size of the new split block.
           ((struct free_meta*)new_free_block)->size = initial_size - final_size;
        }
        else {
        // TODO: Try to allocate memory in such a way that the block are
        // always larger than 24 bytes.
            assert(0);
        }
    }
}

/* Given a size of the block request, the function requests the OS(sbrk) to
 * increase the top of the heap address by that size(number of bytes).
 *
 * param:
 *     size: size of the block requested.
 * return: pointer to the beginning of the block with the size of the block in
 *         the first 4 byte as int. */

struct allocated_meta* request_space(size_t size) {
  struct allocated_meta* block;
  // Returns a pointer to the top of the heap.
  block = sbrk(0);
  // Returns a pointer to the previous top of the heap(after allocation).
  void *request = sbrk(size + ALLOCATED_META_SIZE);
  // p points to the top of the heap before new allocation and request points
  // to a previous top of heap after allocation.
  assert((void*)block == request); // Not thread safe.
  if (request == (void*) -1) {
    return NULL; // sbrk failed.
  }
  block->size = size;
  return block;
}


/* 
 *
 * THIS IS THE FREEING
 *
 *  */

/* Return an address to the mata-data region of a block given the block address. */
struct allocated_meta* get_block_ptr(void *ptr) {
  return (struct allocated_meta*)ptr - 1;
}

void free(void* ptr) {
    if (!ptr) {
        return;
    }
    size_t size_of_block;
  // TODO: consider merging blocks once splitting blocks is implemented.
    struct allocated_meta* block_ptr = get_block_ptr(ptr);
    ((struct free_meta*)ptr)->size = block_ptr->size;
    insert_block_to_list(ptr);
}

void insert_before(struct free_meta* current, struct free_meta* ptr) {
    if (ptr == (current->prev + (current->prev->size))) {  // coalase with the previous block.
        current->size = current->size + ptr->size;
        return;
    }
    current->prev->next = ptr;
    ptr->prev = current->prev;
    current->prev = ptr;
    ptr->next = current;
}

void insert_block_to_list(void* ptr) {
    if (global_base) {
        void* current = global_base;
        while(((struct free_meta*)current)->next) {
            if (ptr > current) {
                /* TODO: handle the case of inserting at the beginning. */
                insert_before(current, ptr);
                return;
            }
            current = ((struct free_meta*)current)->next;
        }
        if (ptr > current) {
            insert_before(current, ptr);
            return;
        }
        else { // add the block to the end of the list.
            ((struct free_meta*)current)->next = ptr;
            ((struct free_meta*)ptr)->prev = current;
            ((struct free_meta*)ptr)->next = NULL;
        }
    }
    else { // if the free list is empty.
        ((struct free_meta*)ptr)->next = NULL;
        ((struct free_meta*)ptr)->prev = NULL;
        global_base = ptr;
    }

}

