#include "kwsys.h"

int main(void) {
    Kwc_Setup();
    MAlloc( 256);
    void* ptr_b = MAlloc( 256);
    MAlloc( 256);
    void* ptr_a = MAlloc(2048);
    Realloc(ptr_a, 256);
    Free(ptr_b);
    Kwc_Clear();
    return 0;
}
