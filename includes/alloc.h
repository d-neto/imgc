#ifndef __ALLOC_H__
#define __ALLOC_H__

#include <stdlib.h>
#include <stdio.h>

#define ALLOC(size) malloc(size)
#define REALLOC(ptr, size) realloc(ptr, size)

#define FREE(ptr) do{ \
    if (ptr) { \
        free(ptr); \
        ptr = NULL; \
    } \
} while(0)

#endif /* __ALLOC_H__ */
