/**	@file cxx.h
 *	@brief Custom override of (de)allocation onto specific heaps.
 *
 *  @author Zephiles
 *	@bug No known bugs.
 */
#include "rels/include/cxx.h"

#include <cstdint>
#include <cstring>

#include "os/OSCache.h"
#include "JSystem/JKernel/JKRHeap.h"
#include "JSystem/JKernel/JKRExpHeap.h"
#include "JSystem/JKernel/JKRAssertHeap.h"
#include "rels/include/defines.h"

#ifdef WII_PLATFORM
#include "DynamicLink.h"
#endif

extern JKRAssertHeap* AssertHeap;
extern JKRExpHeap* DbPrintHeap;
extern JKRExpHeap* gameHeap;
extern JKRExpHeap* zeldaHeap;
extern JKRExpHeap* archiveHeap;
extern JKRExpHeap* j2dHeap;
#ifndef WII_PLATFORM
extern JKRExpHeap* HostIOHeap;
#endif

JKRHeap* getHeapPtr(int32_t id) {
    static JKRHeap** heapPtrArray[] = {
        (JKRHeap**)&AssertHeap,
        (JKRHeap**)&DbPrintHeap,
        (JKRHeap**)&gameHeap,
        (JKRHeap**)&zeldaHeap,
        (JKRHeap**)&archiveHeap,
        (JKRHeap**)&j2dHeap,

#ifndef WII_PLATFORM
        (JKRHeap**)&HostIOHeap,
#else
        &DynamicModuleControlBase::m_heap,
#endif  // WII_PLATFORM

    };

    // Make sure the id is valid
    const uint32_t heapPtrArraySize = sizeof(heapPtrArray) / sizeof(heapPtrArray[0]);
    if ((id < 0) || (static_cast<uint32_t>(id) >= heapPtrArraySize)) {
        // The id is invalid, so use the archive heap by default
        id = HEAP_ARCHIVE;
    }

    return *heapPtrArray[id];
}

void* allocateMemory(size_t size, JKRHeap* heap, int alignment) {
    // Make sure the heap exists
    if (!heap) {
        return NULL;
    }

    void* ptr = JKRHeap::alloc(size, alignment, heap);
    ptr = memset(ptr, 0, size);
    DCFlushRange(ptr, size);
    return ptr;
}

void* allocateMemoryFromMainHeap(size_t size, int alignment) {
#ifndef WII_PLATFORM
    JKRHeap* heapPtr = archiveHeap;
#else
    JKRHeap* heapPtr = zeldaHeap;
#endif
    return allocateMemory(size, heapPtr, alignment);
}

void* allocateMemoryFromHeapId(size_t size, int alignment, int id) {
    JKRHeap* heapPtr = getHeapPtr(id);
    return allocateMemory(size, heapPtr, alignment);
}

void* operator new(size_t size) {
    return allocateMemoryFromMainHeap(size, 0x20);
}

void* operator new[](size_t size) {
    return allocateMemoryFromMainHeap(size, 0x20);
}

void* operator new(size_t size, int alignment) {
    return allocateMemoryFromMainHeap(size, alignment);
}

void* operator new[](size_t size, int alignment) {
    return allocateMemoryFromMainHeap(size, alignment);
}

void* operator new(size_t size, int alignment, int id) {
    return allocateMemoryFromHeapId(size, alignment, id);
}

void* operator new[](size_t size, int alignment, int id) {
    return allocateMemoryFromHeapId(size, alignment, id);
}
