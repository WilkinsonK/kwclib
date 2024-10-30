#include <stdlib.h>

#include "kwsys.h"

/**
 * Tracks an allocated block of memory.
 */
typedef struct SystemBlock {
    void*  ptr;
    size_t heap_size;
} SystemBlock;

/**
 * Destroys an allocated block of memory.
 */
void SystemBlock_Free(SystemBlock* blk) {
    if (blk == NULL) {
        ErrorF("Cannot destroy heap block @[%p] (is NULL)\n", (void*)blk);
        return;
    }
    SystemF("Destroyed heap block @[%p->%p] (%lu bytes)\n", (void*)blk, blk->ptr, blk->heap_size);
    free(blk->ptr);
    free(blk);
}

/**
 * Allocates a block of memory to the designated sized in
 * bytes.
 */
SystemBlock* SystemBlock_MAlloc(size_t size) {
    SystemBlock* blk = (SystemBlock*)malloc(sizeof(SystemBlock));
    blk->ptr       = malloc(size);
    blk->heap_size = size;
    SystemF("Allocated heap block @[%p->%p] (%lu bytes)\n", (void*)blk, blk->ptr, size);
    return blk;
}

/**
 * Resizes a block of memory to the designated size in
 * bytes.
 */
SystemBlock* SystemBlock_Realloc(SystemBlock* blk, size_t size) {
    SystemF("Adjusted heap block @[%p->%p] from (%lu bytes) to (%lu bytes)\n", (void*)blk, blk->ptr, blk->heap_size, size);
    blk->ptr       = realloc(blk->ptr, size);
    blk->heap_size = size;
    return blk;
}

/**
 * Tracks a series of blocks of memory allocated on the
 * heap.
 * Values are tracked to ensure that all blocks allocated
 * are freed.
 */
typedef struct SystemBlocks {
    size_t        capacity;
    size_t        count;
    size_t        count_allocs;
    size_t        count_frees;
    SystemBlock** items;
} SystemBlocks;
/** Global heap management system. */
static SystemBlocks BLOCKS = {0};

/**
 * Tries to find the index of a block of memory from the
 * associated pointer.
 */
size_t Blocks_IndexByPointer(void* ptr) {
    size_t idx = 0;
    while (idx < BLOCKS.count)
        if (BLOCKS.items[idx++]->ptr == ptr) return idx-1;
    ErrorF("Pointer (%p) could not be found in heap manager\n", ptr);
    return -1;
}

/**
 * Tries to free a block of memory located at the index of
 * the system blocks in the heap management system.
 */
void Blocks_FreeAt(size_t idx) {
    SystemBlock_Free(BLOCKS.items[idx]);

    BLOCKS.items[idx] = NULL;
    for (size_t i = idx; (i+1) < BLOCKS.count; i++) {
        BLOCKS.items[i]   = BLOCKS.items[i+1];
        BLOCKS.items[i+1] = NULL;
    }
    BLOCKS.count_frees++;
    BLOCKS.count--;
}

void Blocks_Free(void* ptr) {
    Blocks_FreeAt(Blocks_IndexByPointer(ptr));
}

void* Blocks_MAlloc(size_t size) {
    SystemBlock* blk = SystemBlock_MAlloc(size);
    if (BLOCKS.count+1 == BLOCKS.capacity) {
        BLOCKS.capacity *= 2;
        BLOCKS.items = (SystemBlock**)realloc(BLOCKS.items, sizeof(SystemBlock*)*BLOCKS.capacity);
    }
    BLOCKS.count_allocs++;
    BLOCKS.items[BLOCKS.count++] = blk;
    return blk->ptr;
}

void* Blocks_Realloc(void* ptr, size_t size) {
    size_t idx = Blocks_IndexByPointer(ptr);
    if (idx == (size_t)-1) return NULL;
    return SystemBlock_Realloc(BLOCKS.items[idx], size)->ptr;
}

void Blocks_Clear(void) {
    SystemF("Tearing down heap management.\n");
    while (BLOCKS.items[0] != NULL)
        Blocks_FreeAt(0);

    BLOCKS.capacity = 0;
    BLOCKS.count    = 0;
    free(BLOCKS.items);

    size_t left_over = (BLOCKS.count_allocs - BLOCKS.count_frees);
    if (left_over != 0) {
        ErrorF("Not all blocks freed. %lu blocks were not freed\n", left_over);
    } else {
        SystemF("All blocks freed successfully.\n");
    }
}

void Blocks_Setup(void) {
    BLOCKS.capacity     = 1;
    BLOCKS.count        = 0;
    BLOCKS.count_allocs = 0;
    BLOCKS.count_frees  = 0;
    BLOCKS.items        = (SystemBlock**)malloc(sizeof(SystemBlock*));
    SystemF("Heap managment initialized.\n")
}
