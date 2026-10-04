#ifndef TPGZ_MODULES_BOOT_INCLUDE_CRASH_QR_H
#define TPGZ_MODULES_BOOT_INCLUDE_CRASH_QR_H
#include <os.h>

void GZ_captureCrash(u16 error, OSContext* context, u32 dsisr, u32 dar);
void GZ_drawCrashQr();

#endif
