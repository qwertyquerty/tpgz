#include "utils/mem2.h"
#include <cstring>
#include "rels/include/defines.h"
#include "JSystem/JKernel/JKRExpHeap.h"

#define MEM2_HEAP_SIZE 0x100000

#ifdef GCN_PLATFORM
#define NIN_CFG_ADDR 0x93004000
#define NIN_CFG_MAGIC 0x01070CF6
#define NIN_CFG_GAME_PATH_OFFSET 0x14
#define NIN_KERNEL_START 0x92F00000
#define NIN_KERNEL_END 0x92FE0000
#define NIN_CACHE_START 0x11000000
#define NIN_CACHE_SIZE_SMALL 0x300000
#define NIN_CACHE_SIZE_LARGE 0x1E80000
#define NIN_MEM2_FREE_START 0x91300000
#define NIN_MEM2_FREE_END 0x92E80000
#endif

KEEP_VAR JKRHeap* g_tpgzMem2Heap;
KEEP_VAR uint8_t* g_mem2Storage;
KEEP_VAR uint32_t g_mem2StorageSize;
KEEP_VAR const char* g_mem2UnavailableReason;

#ifdef GCN_PLATFORM
static asm u32 readPVR() {
    nofralloc
    mfspr r3, 287
    blr
}

static asm u32 readHID4() {
    nofralloc
    mfspr r3, 1011
    blr
}

static asm u32 readDBAT4U() {
    nofralloc
    mfspr r3, 568
    blr
}

static const char* checkNintendontKernel() {
    bool sawSmall = false, sawLarge = false, sawUnbounded = false;
    const u32* words = reinterpret_cast<const u32*>(NIN_KERNEL_START);
    u32 count = (NIN_KERNEL_END - NIN_KERNEL_START) / sizeof(u32);
    for (u32 i = 1; i + 1 < count; i++) {
        u32 offset = words[i] - NIN_CACHE_START;
        for (int j = -1; words[i] >= NIN_CACHE_START && offset < NIN_CACHE_SIZE_LARGE && j <= 1; j += 2) {
            u32 total = offset + words[i + j];
            sawLarge |= total == NIN_CACHE_SIZE_LARGE;
            sawSmall |= total == NIN_CACHE_SIZE_SMALL && offset <= NIN_CACHE_SIZE_SMALL;
            sawUnbounded |= total == NIN_CACHE_SIZE_SMALL && offset > NIN_CACHE_SIZE_SMALL;
        }
    }
    if (sawUnbounded) {
        return "save states need a 251 block or smaller memory card";
    }
    if (sawLarge || !sawSmall) {
        return "save states need nintendont 6.507 or newer";
    }
    return NULL;
}

static const char* checkNintendont() {
    u32 pvr = readPVR();
    if ((pvr >> 16) != 0x0008 || (pvr & 0xF000) != 0x7000 || (readHID4() & 0x02000000) == 0) {
        return "save states need nintendont";
    }
    u32 dbat = readDBAT4U();
    const u8* cfg = reinterpret_cast<const u8*>(NIN_CFG_ADDR);
    if ((dbat & 0xF0000000) != 0x90000000 || (dbat & 0x2) == 0 || *reinterpret_cast<const u32*>(cfg) != NIN_CFG_MAGIC) {
        return "save states need nintendont";
    }
    if (memcmp(cfg + NIN_CFG_GAME_PATH_OFFSET, "di", 3) == 0) {
        return "save states don't work when playing from a disc";
    }
    return checkNintendontKernel();
}
#endif

KEEP_FUNC void GZ_initMem2() {
#ifdef WII_PLATFORM
    g_tpgzMem2Heap = JKRExpHeap::create(MEM2_HEAP_SIZE, JKRHeap::getRootHeap2(), false);
#else
    g_mem2UnavailableReason = checkNintendont();
    if (g_mem2UnavailableReason == NULL) {
        g_tpgzMem2Heap = JKRExpHeap::create(reinterpret_cast<void*>(NIN_MEM2_FREE_START), MEM2_HEAP_SIZE,
                                            JKRHeap::getRootHeap(), false);
        g_mem2Storage = reinterpret_cast<uint8_t*>(NIN_MEM2_FREE_START + MEM2_HEAP_SIZE);
        g_mem2StorageSize = NIN_MEM2_FREE_END - (NIN_MEM2_FREE_START + MEM2_HEAP_SIZE);
    }
#endif
    if (g_mem2UnavailableReason == NULL && g_tpgzMem2Heap == NULL) {
        g_mem2UnavailableReason = "save states unavailable";
    }
}
