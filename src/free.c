#include <assert.h>
#include <string.h> 

#include "free.h"
#include "malloc.h"


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

void insert_block_to_list(void* ptr) {
    if (global_base) {
        void* current = global_base;
        while(((struct free_meta*)current)->next) {
            if (ptr > current) {
                /* TODO: handle the case of inserting at the beginning. */
                ((struct free_meta*)current)->prev->next = ptr;
                ((struct free_meta*)ptr)->prev = ((struct free_meta*)current)->prev;
                ((struct free_meta*)current)->prev = ptr;
                ((struct free_meta*)ptr)->next = current;
                return;
            }
            current = ((struct free_meta*)current)->next;
        }
        if (ptr > current) {
            ((struct free_meta*)current)->prev->next = ptr;
            ((struct free_meta*)ptr)->prev = ((struct free_meta*)current)->prev;
            ((struct free_meta*)current)->prev = ptr;
            ((struct free_meta*)ptr)->next = current;
            return;
        }
        else { // add the block to the last of the list.
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
