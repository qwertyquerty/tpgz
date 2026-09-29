#ifndef TPGZ_MODULES_BOOT_INCLUDE_UTILS_QRCODE_H
#define TPGZ_MODULES_BOOT_INCLUDE_UTILS_QRCODE_H
#include <stdint.h>

#define QR_VERSION 15
#define QR_SIZE (QR_VERSION * 4 + 17)
#define QR_ALNUM_CAPACITY 600

struct QRCode {
    uint8_t modules[(QR_SIZE * QR_SIZE + 7) / 8];
};

bool QR_isAlnum(char c);
bool QR_encodeAlnum(QRCode* qr, const char* text);
bool QR_getModule(const QRCode* qr, int x, int y);

#endif
