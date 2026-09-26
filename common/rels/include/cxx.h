/**	@file cxx.h
 *	@brief Custom override of (de)allocation onto specific heaps.
 *
 *	@author Zephiles
 *	@author kipcode66
 *	@bug No known bugs.
 */
#ifndef LIBTP_CXX_H
#define LIBTP_CXX_H

#include <cstddef>
#include <cstdint>

enum HeapID {
    HEAP_ASSERT = 0,
    HEAP_DBPRINT,
    HEAP_GAME,
    HEAP_ZELDA,
    HEAP_ARCHIVE,
    HEAP_J2D,
    HEAP_HOST_IO,
};

void* operator new(size_t size);
void* operator new[](size_t size);
void* operator new(size_t size, int alignment);
void* operator new[](size_t size, int alignment);
void* operator new(size_t size, int alignment, int heapId);
void* operator new[](size_t size, int alignment, int heapId);

#endif  // LIBTP_CXX_H