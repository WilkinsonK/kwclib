#ifndef KWC_BLOCKS_H
#define KWC_BLOCKS_H
#include <stdlib.h>
/**
 * Tracks an allocated block of memory.
 */
typedef struct SystemBlock {
    void*  ptr;
    size_t heap_size;
} SystemBlock;
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
/** Deallocates all blocks allocated in this system */
void SystemBlocks_Clear(SystemBlocks*);
/** Destroys all heap blocks. */
void SystemBlocks_Free(SystemBlocks*, void*);
/** Initializes a block of memory on the heap. */
void* SystemBlocks_MAlloc(SystemBlocks*, size_t);
/** Resizes a block of memory on the heap. */
void* SystemBlocks_Realloc(SystemBlocks*, void*, size_t);
/** Initializes a heap management system. */
void SystemBlocks_Setup(SystemBlocks*);
/** Destroys all blocks allocated on the heap. */
void Blocks_Clear(void);
/** Initializes the heap block management system. */
void Blocks_Setup(void);
/** Free and destroy a single heap allocation. */
void Blocks_Free(void*);
/** Initializes a block of memory on the heap. */
void* Blocks_MAlloc(size_t);
/** Resizes a block of memory on the heap. */
void* Blocks_Realloc(void*, size_t);
#endif // Pragma
