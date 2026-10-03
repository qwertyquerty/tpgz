#ifndef TPGZ_MODULES_BOOT_INCLUDE_UTILS_MEMORY_H
#define TPGZ_MODULES_BOOT_INCLUDE_UTILS_MEMORY_H
#include <cstdint>

#include "font.h"
#include "utils/containers/deque.h"

enum MemoryType { MEM_TYPE_U8, MEM_TYPE_S8, MEM_TYPE_U16, MEM_TYPE_S16, MEM_TYPE_U32, MEM_TYPE_S32, MEM_TYPE_F32, MEM_TYPE_STR };

enum MemoryColumns { WatchAddress, WatchX, WatchY, WatchHex, WatchType, WatchOffset };

struct MemoryWatch {
    uint32_t address;
    float x;
    float y;
    bool hex;
    uint8_t type;
    uint16_t offset;
    uint32_t value;
    bool visible;
    bool line_selected;
    bool value_selected;
};

inline void GZ_initMemoryWatch(MemoryWatch& watch) {
    watch.address = 0x80000000;
    watch.x = 400.0f;
    watch.y = 100.0f;
    watch.hex = false;
    watch.type = MEM_TYPE_STR;
    watch.offset = 0x0000;
    watch.visible = false;
    watch.line_selected = false;
    watch.value_selected = false;
}

void GZ_drawWatches();
void GZ_drawHeapInfo();

#endif
