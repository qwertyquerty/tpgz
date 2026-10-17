#ifndef TPGZ_MODULES_BOOT_INCLUDE_UTILS_MEM2_H
#define TPGZ_MODULES_BOOT_INCLUDE_UTILS_MEM2_H
#include <stdint.h>

class JKRHeap;

extern JKRHeap* g_tpgzMem2Heap;
extern uint8_t* g_mem2Storage;
extern uint32_t g_mem2StorageSize;
extern const char* g_mem2UnavailableReason;

void GZ_initMem2();

#endif
