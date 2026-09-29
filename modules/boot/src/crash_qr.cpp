#include "crash_qr.h"
#include <cstdio>
#include <cstring>
#include "utils/qrcode.h"
#include "rels/include/defines.h"
#include "os/OSCache.h"
#include "os/OSModule.h"
#include "JSystem/JUtility/JUTDirectPrint.h"

#define CRASH_FORMAT 1
#define OS_MODULE_LIST 0x800030C8
#define DIRECT_PRINT_WIDTH_OFFSET 0x4
#define DIRECT_PRINT_HEIGHT_OFFSET 0x6
#define DIRECT_PRINT_STRIDE_OFFSET 0x8
#define MAX_BACKTRACE 32
#define QR_MODULE_PIXELS 3
#define QR_QUIET_MODULES 4
#define QR_MARGIN 8
#define XFB_BLACK 0x1080
#define XFB_WHITE 0xEB80

static QRCode l_crashQr;
static char l_crashText[QR_ALNUM_CAPACITY + 1];
static u32 l_crashLength;
static bool l_crashQrReady;

static bool isValidStackAddress(u32 addr) {
    if (addr & 3) {
        return false;
    }
#ifdef WII_PLATFORM
    if (addr >= 0x90000000 && addr < 0x94000000) {
        return true;
    }
#endif
    return addr >= 0x80000000 && addr < 0x81800000;
}

static bool append(const char* str) {
    u32 length = strlen(str);
    if (l_crashLength + length > QR_ALNUM_CAPACITY) {
        return false;
    }
    for (u32 i = 0; i < length; i++) {
        char c = str[i];
        if (c >= 'a' && c <= 'z') {
            c -= 'a' - 'A';
        }
        l_crashText[l_crashLength++] = QR_isAlnum(c) ? c : '-';
    }
    l_crashText[l_crashLength] = '\0';
    return true;
}

static bool appendValue(u32 value) {
    char buf[32];
    OSModuleQueue* modules = reinterpret_cast<OSModuleQueue*>(OS_MODULE_LIST);
    for (OSModuleInfo* module = modules->head; module != NULL; module = module->link.next) {
        OSSectionInfo* sections = reinterpret_cast<OSSectionInfo*>(module->sectionInfoOffset);
        for (u32 i = 0; i < module->numSections; i++) {
            u32 start = sections[i].offset & ~1;
            if (start != 0 && value >= start && value < start + sections[i].size) {
                snprintf(buf, sizeof(buf), " %X.%X.%X", (u32)module->id, i, value - start);
                return append(buf);
            }
        }
    }
    snprintf(buf, sizeof(buf), " %X", value);
    return append(buf);
}

KEEP_FUNC void GZ_captureCrash(u16 error, OSContext* context, u32 dsisr, u32 dar) {
    l_crashLength = 0;
    char buf[48];
    snprintf(buf, sizeof(buf), "TPGZ%d %s %X %X", CRASH_FORMAT, _VERSION, VERSION, error);
    append(buf);
    appendValue(context->srr0);
    appendValue(context->lr);
    appendValue(dsisr);
    appendValue(dar);
    for (int i = 0; i < 32; i++) {
        appendValue(context->gpr[i]);
    }
    u32* frame = reinterpret_cast<u32*>(context->gpr[1]);
    for (int i = 0; i < MAX_BACKTRACE && isValidStackAddress((u32)frame) && appendValue(frame[1]); i++) {
        frame = reinterpret_cast<u32*>(frame[0]);
    }
    l_crashQrReady = QR_encodeAlnum(&l_crashQr, l_crashText);
}

KEEP_FUNC void GZ_drawCrashQr() {
    JUTDirectPrint* directPrint = JUTDirectPrint::getManager();
    if (!l_crashQrReady || directPrint == NULL || directPrint->getFrameBuffer() == NULL) {
        return;
    }
    u8* fields = reinterpret_cast<u8*>(directPrint);
    int width = *reinterpret_cast<u16*>(fields + DIRECT_PRINT_WIDTH_OFFSET);
    int height = *reinterpret_cast<u16*>(fields + DIRECT_PRINT_HEIGHT_OFFSET);
    int stride = *reinterpret_cast<u16*>(fields + DIRECT_PRINT_STRIDE_OFFSET);
    u16* frameBuffer = static_cast<u16*>(directPrint->getFrameBuffer());

    int pixels = (QR_SIZE + QR_QUIET_MODULES * 2) * QR_MODULE_PIXELS;
    int left = width - pixels - QR_MARGIN;
    int top = height - pixels - QR_MARGIN;
    if (left < 0 || top < 0) {
        return;
    }
    for (int py = 0; py < pixels; py++) {
        u16* row = frameBuffer + (top + py) * stride + left;
        int y = py / QR_MODULE_PIXELS - QR_QUIET_MODULES;
        for (int px = 0; px < pixels; px++) {
            int x = px / QR_MODULE_PIXELS - QR_QUIET_MODULES;
            bool dark = x >= 0 && y >= 0 && x < QR_SIZE && y < QR_SIZE && QR_getModule(&l_crashQr, x, y);
            row[px] = dark ? XFB_BLACK : XFB_WHITE;
        }
    }
    DCStoreRange(frameBuffer + top * stride, pixels * stride * sizeof(u16));
}
