#include <stdlib.h>

#include "blocks.h"
#include "kwsys.h"

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

/** Global heap management system. */
static SystemBlocks BLOCKS = {0};

/**
 * Tries to find the index of a block of memory from the
 * associated pointer.
 */
size_t SystemBlocks_IndexByPointer(SystemBlocks* sb, void* ptr) {
    size_t idx = 0;
    while (idx < sb->count)
        if (sb->items[idx++]->ptr == ptr) return idx-1;
    ErrorF("Pointer (%p) could not be found in heap manager\n", ptr);
    return -1;
}

/**
 * Tries to free a block of memory located at the index of
 * the system blocks in the heap management system.
 */
void SystemBlocks_FreeAt(SystemBlocks* sb, size_t idx) {
    SystemBlock_Free(sb->items[idx]);

    sb->items[idx] = NULL;
    for (size_t i = idx; (i+1) < sb->count; i++) {
        sb->items[i]   = sb->items[i+1];
        sb->items[i+1] = NULL;
    }
    sb->count_frees++;
    sb->count--;
}

void SystemBlocks_Free(SystemBlocks* sb, void* ptr) {
    SystemBlocks_FreeAt(sb, SystemBlocks_IndexByPointer(sb, ptr));
}

void* SystemBlocks_MAlloc(SystemBlocks* sb, size_t size) {
    SystemBlock* blk = SystemBlock_MAlloc(size);
    if (sb->count+1 == sb->capacity) {
        sb->capacity *= 2;
        sb->items = (SystemBlock**)realloc(sb->items, sizeof(SystemBlock*)*sb->capacity);
    }
    sb->count_allocs++;
    sb->items[sb->count++] = blk;
    return blk->ptr;
}

void* SystemBlocks_Realloc(SystemBlocks* sb, void* ptr, size_t size) {
    size_t idx = SystemBlocks_IndexByPointer(sb, ptr);
    if (idx == (size_t)-1) return NULL;
    return SystemBlock_Realloc(sb->items[idx], size)->ptr;
}

void SystemBlocks_Setup(SystemBlocks* sb) {
    sb->capacity     = 1;
    sb->count        = 0;
    sb->count_allocs = 0;
    sb->count_frees  = 0;
    sb->items        = (SystemBlock**)malloc(sizeof(SystemBlock*));
    SystemF("Heap managment initialized.\n");
}

void SystemBlocks_Clear(SystemBlocks* sb) {
    SystemF("Tearing down heap management.\n");
    while (sb->items[0] != NULL)
        SystemBlocks_FreeAt(sb, 0);

    sb->capacity = 0;
    sb->count    = 0;
    free(sb->items);

    size_t left_over = (sb->count_allocs - sb->count_frees);
    if (left_over != 0) {
        ErrorF("Not all blocks freed. %lu blocks were not freed\n", left_over);
    } else {
        SystemF("All blocks freed successfully.\n");
    }
}

void Blocks_Free(void* ptr) {
    SystemBlocks_Free(&BLOCKS, ptr);
}

void* Blocks_MAlloc(size_t size) {
    return SystemBlocks_MAlloc(&BLOCKS, size);
}

void* Blocks_Realloc(void* ptr, size_t size) {
    return SystemBlocks_Realloc(&BLOCKS, ptr, size);
}

void Blocks_Clear(void) {
    SystemBlocks_Clear(&BLOCKS);
}

void Blocks_Setup(void) {
    SystemBlocks_Setup(&BLOCKS);
}
