// Head of the free memory block linked list.
void* global_base = NULL;

void free(void*);
void insert_block_to_list(void*);
