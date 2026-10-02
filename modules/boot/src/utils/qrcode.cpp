#include "utils/qrcode.h"
#include <cstring>

#define QR_DATA_CODEWORDS 415
#define QR_BLOCKS 10
#define QR_SHORT_BLOCKS 5
#define QR_SHORT_DATA_LEN 41
#define QR_ECC_LEN 24
#define QR_ECC_LEVEL_M 0

static const char l_alnumChars[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ $%*+-./:";
static const uint8_t l_alignment[] = {6, 26, 48, 70};

static uint8_t l_data[QR_DATA_CODEWORDS];
static uint8_t l_codewords[QR_DATA_CODEWORDS + QR_BLOCKS * QR_ECC_LEN];
static uint8_t l_function[sizeof(QRCode)];
static QRCode l_candidate;

static int alnumIndex(char c) {
    const char* found = c != '\0' ? strchr(l_alnumChars, c) : NULL;
    return found != NULL ? found - l_alnumChars : -1;
}

bool QR_isAlnum(char c) {
    return alnumIndex(c) >= 0;
}

static bool getBit(const uint8_t* bits, int x, int y) {
    int index = y * QR_SIZE + x;
    return (bits[index >> 3] >> (index & 7)) & 1;
}

static void setBit(uint8_t* bits, int x, int y, bool value) {
    int index = y * QR_SIZE + x;
    bits[index >> 3] = (bits[index >> 3] & ~(1 << (index & 7))) | (value << (index & 7));
}

bool QR_getModule(const QRCode* qr, int x, int y) {
    return getBit(qr->modules, x, y);
}

static void setFunction(QRCode* qr, int x, int y, bool dark) {
    setBit(qr->modules, x, y, dark);
    setBit(l_function, x, y, true);
}

static void putBits(uint32_t* pos, uint32_t value, int count) {
    for (int i = count - 1; i >= 0; i--, (*pos)++) {
        l_data[*pos >> 3] |= ((value >> i) & 1) << (7 - (*pos & 7));
    }
}

static uint8_t gfMul(uint8_t x, uint8_t y) {
    uint32_t z = 0;
    for (int i = 7; i >= 0; i--) {
        z = (z << 1) ^ ((z >> 7) * 0x11D);
        z ^= ((y >> i) & 1) * x;
    }
    return z;
}

static uint32_t bchEncode(uint32_t data, int eccBits, uint32_t poly) {
    uint32_t rem = data;
    for (int i = 0; i < eccBits; i++) {
        rem = (rem << 1) ^ ((rem >> (eccBits - 1)) * poly);
    }
    return data << eccBits | rem;
}

static int maxAbs(int a, int b) {
    a = a < 0 ? -a : a;
    b = b < 0 ? -b : b;
    return a > b ? a : b;
}

static void buildCodewords() {
    uint8_t divisor[QR_ECC_LEN] = {0};
    divisor[QR_ECC_LEN - 1] = 1;
    for (uint8_t i = 0, root = 1; i < QR_ECC_LEN; i++, root = gfMul(root, 2)) {
        for (int j = 0; j < QR_ECC_LEN; j++) {
            divisor[j] = gfMul(divisor[j], root) ^ (j + 1 < QR_ECC_LEN ? divisor[j + 1] : 0);
        }
    }

    const uint8_t* block = l_data;
    for (int b = 0; b < QR_BLOCKS; b++) {
        int length = QR_SHORT_DATA_LEN + (b >= QR_SHORT_BLOCKS);
        uint8_t ecc[QR_ECC_LEN] = {0};
        for (int i = 0; i < length; i++) {
            uint8_t factor = block[i] ^ ecc[0];
            memmove(ecc, ecc + 1, QR_ECC_LEN - 1);
            ecc[QR_ECC_LEN - 1] = 0;
            for (int j = 0; j < QR_ECC_LEN; j++) {
                ecc[j] ^= gfMul(divisor[j], factor);
            }
            int column = i < QR_SHORT_DATA_LEN ? i * QR_BLOCKS + b : QR_SHORT_DATA_LEN * QR_BLOCKS + b - QR_SHORT_BLOCKS;
            l_codewords[column] = block[i];
        }
        for (int i = 0; i < QR_ECC_LEN; i++) {
            l_codewords[QR_DATA_CODEWORDS + i * QR_BLOCKS + b] = ecc[i];
        }
        block += length;
    }
}

static void drawFormat(QRCode* qr, int mask) {
    uint32_t format = bchEncode(QR_ECC_LEVEL_M << 3 | mask, 10, 0x537) ^ 0x5412;
    for (int i = 0; i < 15; i++) {
        bool bit = (format >> i) & 1;
        setFunction(qr, 8, i < 6 ? i : i < 8 ? i + 1 : QR_SIZE - 15 + i, bit);
        setFunction(qr, i < 8 ? QR_SIZE - 1 - i : i < 9 ? 15 - i : 14 - i, 8, bit);
    }
    setFunction(qr, 8, QR_SIZE - 8, true);
}

static void drawFunctionPatterns(QRCode* qr) {
    for (int i = 0; i < QR_SIZE; i++) {
        setFunction(qr, 6, i, i % 2 == 0);
        setFunction(qr, i, 6, i % 2 == 0);
    }

    const int finders[3][2] = {{3, 3}, {QR_SIZE - 4, 3}, {3, QR_SIZE - 4}};
    for (int f = 0; f < 3; f++) {
        for (int dy = -4; dy <= 4; dy++) {
            for (int dx = -4; dx <= 4; dx++) {
                int x = finders[f][0] + dx;
                int y = finders[f][1] + dy;
                if (x >= 0 && x < QR_SIZE && y >= 0 && y < QR_SIZE) {
                    setFunction(qr, x, y, maxAbs(dx, dy) != 2 && maxAbs(dx, dy) != 4);
                }
            }
        }
    }

    int last = sizeof(l_alignment) - 1;
    for (int i = 0; i <= last; i++) {
        for (int j = 0; j <= last; j++) {
            if ((i == 0 && j == 0) || (i == 0 && j == last) || (i == last && j == 0)) {
                continue;
            }
            for (int dy = -2; dy <= 2; dy++) {
                for (int dx = -2; dx <= 2; dx++) {
                    setFunction(qr, l_alignment[i] + dx, l_alignment[j] + dy, maxAbs(dx, dy) != 1);
                }
            }
        }
    }

    drawFormat(qr, 0);
    uint32_t version = bchEncode(QR_VERSION, 12, 0x1F25);
    for (int i = 0; i < 18; i++) {
        bool bit = (version >> i) & 1;
        setFunction(qr, QR_SIZE - 11 + i % 3, i / 3, bit);
        setFunction(qr, i / 3, QR_SIZE - 11 + i % 3, bit);
    }
}

static void drawCodewords(QRCode* qr) {
    uint32_t bit = 0;
    for (int right = QR_SIZE - 1; right >= 1; right -= right == 8 ? 3 : 2) {
        bool upward = ((right + 1) & 2) == 0;
        for (int vert = 0; vert < QR_SIZE; vert++) {
            int y = upward ? QR_SIZE - 1 - vert : vert;
            for (int x = right; x > right - 2; x--) {
                if (!getBit(l_function, x, y)) {
                    bool dark = bit < sizeof(l_codewords) * 8 && ((l_codewords[bit >> 3] >> (7 - (bit & 7))) & 1);
                    setBit(qr->modules, x, y, dark);
                    bit++;
                }
            }
        }
    }
}

static bool maskBit(int mask, int x, int y) {
    switch (mask) {
    case 0:
        return (x + y) % 2 == 0;
    case 1:
        return y % 2 == 0;
    case 2:
        return x % 3 == 0;
    case 3:
        return (x + y) % 3 == 0;
    case 4:
        return (x / 3 + y / 2) % 2 == 0;
    case 5:
        return x * y % 2 + x * y % 3 == 0;
    case 6:
        return (x * y % 2 + x * y % 3) % 2 == 0;
    default:
        return ((x + y) % 2 + x * y % 3) % 2 == 0;
    }
}

static void applyMask(QRCode* qr, int mask) {
    for (int y = 0; y < QR_SIZE; y++) {
        for (int x = 0; x < QR_SIZE; x++) {
            if (!getBit(l_function, x, y) && maskBit(mask, x, y)) {
                setBit(qr->modules, x, y, !getBit(qr->modules, x, y));
            }
        }
    }
    drawFormat(qr, mask);
}

static int linePenalty(const QRCode* qr, bool rows, int line) {
    int penalty = 0;
    int run = 0;
    uint16_t window = 0;
    for (int i = 0; i < QR_SIZE; i++) {
        bool dark = rows ? getBit(qr->modules, i, line) : getBit(qr->modules, line, i);
        run = i > 0 && dark == (window & 1) ? run + 1 : 1;
        penalty += run == 5 ? 3 : run > 5 ? 1 : 0;
        window = ((window << 1) | dark) & 0x7FF;
        penalty += i >= 10 && (window == 0x5D0 || window == 0x05D) ? 40 : 0;
    }
    return penalty;
}

static int maskPenalty(const QRCode* qr) {
    int penalty = 0;
    int dark = 0;
    for (int y = 0; y < QR_SIZE; y++) {
        penalty += linePenalty(qr, true, y) + linePenalty(qr, false, y);
        for (int x = 0; x < QR_SIZE; x++) {
            bool module = getBit(qr->modules, x, y);
            dark += module;
            if (x + 1 < QR_SIZE && y + 1 < QR_SIZE && module == getBit(qr->modules, x + 1, y) &&
                module == getBit(qr->modules, x, y + 1) && module == getBit(qr->modules, x + 1, y + 1))
            {
                penalty += 3;
            }
        }
    }
    int total = QR_SIZE * QR_SIZE;
    int deviation = maxAbs(dark * 20 - total * 10, 0);
    return penalty + ((deviation + total - 1) / total - 1) * 10;
}

bool QR_encodeAlnum(QRCode* qr, const char* text) {
    uint32_t length = strlen(text);
    if (length > QR_ALNUM_CAPACITY) {
        return false;
    }

    memset(l_data, 0, sizeof(l_data));
    uint32_t pos = 0;
    putBits(&pos, 0x2, 4);
    putBits(&pos, length, 11);
    for (uint32_t i = 0; i < length; i += 2) {
        int a = alnumIndex(text[i]);
        int b = i + 1 < length ? alnumIndex(text[i + 1]) : 0;
        if (a < 0 || b < 0) {
            return false;
        }
        if (i + 1 < length) {
            putBits(&pos, a * 45 + b, 11);
        } else {
            putBits(&pos, a, 6);
        }
    }
    uint32_t capacity = sizeof(l_data) * 8;
    pos = (pos + (capacity - pos < 4 ? capacity - pos : 4) + 7) & ~7;
    for (uint8_t pad = 0xEC; pos < capacity; pad ^= 0xEC ^ 0x11) {
        putBits(&pos, pad, 8);
    }

    buildCodewords();
    memset(qr->modules, 0, sizeof(qr->modules));
    memset(l_function, 0, sizeof(l_function));
    drawFunctionPatterns(qr);
    drawCodewords(qr);

    int bestMask = 0;
    int bestPenalty = 0;
    for (int mask = 0; mask < 8; mask++) {
        memcpy(&l_candidate, qr, sizeof(l_candidate));
        applyMask(&l_candidate, mask);
        int penalty = maskPenalty(&l_candidate);
        if (mask == 0 || penalty < bestPenalty) {
            bestMask = mask;
            bestPenalty = penalty;
        }
    }
    applyMask(qr, bestMask);
    return true;
}
