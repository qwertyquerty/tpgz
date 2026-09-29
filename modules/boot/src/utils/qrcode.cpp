#include "utils/qrcode.h"
#include <cstring>

#define QR_DATA_CODEWORDS 415
#define QR_TOTAL_CODEWORDS 655
#define QR_BLOCKS 10
#define QR_SHORT_BLOCKS 5
#define QR_SHORT_DATA_LEN 41
#define QR_ECC_LEN 24
#define QR_ECC_LEVEL_M 0
#define QR_MASK_COUNT 8
#define QR_PENALTY_RUN 3
#define QR_PENALTY_BLOCK 3
#define QR_PENALTY_FINDER 40
#define QR_PENALTY_BALANCE 10

static const char l_alnumChars[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ $%*+-./:";
static const uint8_t l_alignment[] = {6, 26, 48, 70};
static const int ALIGNMENT_COUNT = sizeof(l_alignment);

static uint8_t l_data[QR_DATA_CODEWORDS];
static uint8_t l_ecc[QR_BLOCKS][QR_ECC_LEN];
static uint8_t l_codewords[QR_TOTAL_CODEWORDS];
static QRCode l_candidate;

static int alnumIndex(char c) {
    for (int i = 0; l_alnumChars[i] != '\0'; i++) {
        if (l_alnumChars[i] == c) {
            return i;
        }
    }
    return -1;
}

bool QR_isAlnum(char c) {
    return alnumIndex(c) >= 0;
}

static void putBits(uint32_t* pos, uint32_t value, int count) {
    for (int i = count - 1; i >= 0; i--) {
        if ((value >> i) & 1) {
            l_data[*pos >> 3] |= 0x80 >> (*pos & 7);
        }
        (*pos)++;
    }
}

static uint8_t gfMul(uint8_t x, uint8_t y) {
    uint32_t z = 0;
    for (int i = 7; i >= 0; i--) {
        z = (z << 1) ^ ((z >> 7) * 0x11D);
        z ^= ((y >> i) & 1) * x;
    }
    return (uint8_t)z;
}

static void computeDivisor(uint8_t* divisor) {
    memset(divisor, 0, QR_ECC_LEN);
    divisor[QR_ECC_LEN - 1] = 1;
    uint8_t root = 1;
    for (int i = 0; i < QR_ECC_LEN; i++) {
        for (int j = 0; j < QR_ECC_LEN; j++) {
            divisor[j] = gfMul(divisor[j], root);
            if (j + 1 < QR_ECC_LEN) {
                divisor[j] ^= divisor[j + 1];
            }
        }
        root = gfMul(root, 0x02);
    }
}

static void computeEcc(const uint8_t* divisor, const uint8_t* data, int length, uint8_t* out) {
    memset(out, 0, QR_ECC_LEN);
    for (int i = 0; i < length; i++) {
        uint8_t factor = data[i] ^ out[0];
        memmove(out, out + 1, QR_ECC_LEN - 1);
        out[QR_ECC_LEN - 1] = 0;
        for (int j = 0; j < QR_ECC_LEN; j++) {
            out[j] ^= gfMul(divisor[j], factor);
        }
    }
}

static void setModule(QRCode* qr, int x, int y, bool dark) {
    int index = y * QR_SIZE + x;
    if (dark) {
        qr->modules[index >> 3] |= 1 << (index & 7);
    } else {
        qr->modules[index >> 3] &= ~(1 << (index & 7));
    }
}

bool QR_getModule(const QRCode* qr, int x, int y) {
    int index = y * QR_SIZE + x;
    return (qr->modules[index >> 3] >> (index & 7)) & 1;
}

static bool isAlignmentCenter(int i, int j) {
    int last = ALIGNMENT_COUNT - 1;
    return !((i == 0 && j == 0) || (i == 0 && j == last) || (i == last && j == 0));
}

static bool isFunctionModule(int x, int y) {
    if ((x < 9 && y < 9) || (x >= QR_SIZE - 8 && y < 9) || (x < 9 && y >= QR_SIZE - 8) || x == 6 || y == 6) {
        return true;
    }
    if ((x >= QR_SIZE - 11 && x < QR_SIZE - 8 && y < 6) || (y >= QR_SIZE - 11 && y < QR_SIZE - 8 && x < 6)) {
        return true;
    }
    for (int i = 0; i < ALIGNMENT_COUNT; i++) {
        for (int j = 0; j < ALIGNMENT_COUNT; j++) {
            int dx = x - l_alignment[i];
            int dy = y - l_alignment[j];
            if (isAlignmentCenter(i, j) && dx >= -2 && dx <= 2 && dy >= -2 && dy <= 2) {
                return true;
            }
        }
    }
    return false;
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

static void drawFunctionPatterns(QRCode* qr) {
    for (int i = 0; i < QR_SIZE; i++) {
        setModule(qr, 6, i, i % 2 == 0);
        setModule(qr, i, 6, i % 2 == 0);
    }

    const int finders[3][2] = {{3, 3}, {QR_SIZE - 4, 3}, {3, QR_SIZE - 4}};
    for (int f = 0; f < 3; f++) {
        for (int dy = -4; dy <= 4; dy++) {
            for (int dx = -4; dx <= 4; dx++) {
                int x = finders[f][0] + dx;
                int y = finders[f][1] + dy;
                int dist = maxAbs(dx, dy);
                if (x >= 0 && x < QR_SIZE && y >= 0 && y < QR_SIZE) {
                    setModule(qr, x, y, dist != 2 && dist != 4);
                }
            }
        }
    }

    for (int i = 0; i < ALIGNMENT_COUNT; i++) {
        for (int j = 0; j < ALIGNMENT_COUNT; j++) {
            if (!isAlignmentCenter(i, j)) {
                continue;
            }
            for (int dy = -2; dy <= 2; dy++) {
                for (int dx = -2; dx <= 2; dx++) {
                    setModule(qr, l_alignment[i] + dx, l_alignment[j] + dy, maxAbs(dx, dy) != 1);
                }
            }
        }
    }

    uint32_t version = bchEncode(QR_VERSION, 12, 0x1F25);
    for (int i = 0; i < 18; i++) {
        bool bit = (version >> i) & 1;
        int a = QR_SIZE - 11 + i % 3;
        int b = i / 3;
        setModule(qr, a, b, bit);
        setModule(qr, b, a, bit);
    }
}

static void drawFormat(QRCode* qr, int mask) {
    uint32_t format = bchEncode(QR_ECC_LEVEL_M << 3 | mask, 10, 0x537) ^ 0x5412;
    for (int i = 0; i <= 5; i++) {
        setModule(qr, 8, i, (format >> i) & 1);
    }
    setModule(qr, 8, 7, (format >> 6) & 1);
    setModule(qr, 8, 8, (format >> 7) & 1);
    setModule(qr, 7, 8, (format >> 8) & 1);
    for (int i = 9; i < 15; i++) {
        setModule(qr, 14 - i, 8, (format >> i) & 1);
    }
    for (int i = 0; i < 8; i++) {
        setModule(qr, QR_SIZE - 1 - i, 8, (format >> i) & 1);
    }
    for (int i = 8; i < 15; i++) {
        setModule(qr, 8, QR_SIZE - 15 + i, (format >> i) & 1);
    }
    setModule(qr, 8, QR_SIZE - 8, true);
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
            if (!isFunctionModule(x, y) && maskBit(mask, x, y)) {
                setModule(qr, x, y, !QR_getModule(qr, x, y));
            }
        }
    }
}

static bool lineModule(const QRCode* qr, bool rows, int line, int i) {
    return rows ? QR_getModule(qr, i, line) : QR_getModule(qr, line, i);
}

static int linePenalty(const QRCode* qr, bool rows, int line) {
    static const uint16_t finderA = 0x5D0;
    static const uint16_t finderB = 0x05D;
    int penalty = 0;
    int run = 1;
    uint16_t window = 0;
    for (int i = 0; i < QR_SIZE; i++) {
        bool dark = lineModule(qr, rows, line, i);
        if (i > 0 && dark == lineModule(qr, rows, line, i - 1)) {
            run++;
            if (run == 5) {
                penalty += QR_PENALTY_RUN;
            } else if (run > 5) {
                penalty++;
            }
        } else {
            run = 1;
        }
        window = ((window << 1) | dark) & 0x7FF;
        if (i >= 10 && (window == finderA || window == finderB)) {
            penalty += QR_PENALTY_FINDER;
        }
    }
    return penalty;
}

static int maskPenalty(const QRCode* qr) {
    int penalty = 0;
    int dark = 0;
    for (int line = 0; line < QR_SIZE; line++) {
        penalty += linePenalty(qr, true, line) + linePenalty(qr, false, line);
    }
    for (int y = 0; y < QR_SIZE; y++) {
        for (int x = 0; x < QR_SIZE; x++) {
            bool module = QR_getModule(qr, x, y);
            dark += module;
            if (x + 1 < QR_SIZE && y + 1 < QR_SIZE && module == QR_getModule(qr, x + 1, y) &&
                module == QR_getModule(qr, x, y + 1) && module == QR_getModule(qr, x + 1, y + 1))
            {
                penalty += QR_PENALTY_BLOCK;
            }
        }
    }
    int total = QR_SIZE * QR_SIZE;
    int deviation = dark * 20 - total * 10;
    deviation = deviation < 0 ? -deviation : deviation;
    return penalty + ((deviation + total - 1) / total - 1) * QR_PENALTY_BALANCE;
}

static int blockLength(int block) {
    return QR_SHORT_DATA_LEN + (block >= QR_SHORT_BLOCKS ? 1 : 0);
}

static void buildCodewords() {
    uint8_t divisor[QR_ECC_LEN];
    computeDivisor(divisor);
    uint32_t offset = 0;
    for (int b = 0; b < QR_BLOCKS; b++) {
        computeEcc(divisor, l_data + offset, blockLength(b), l_ecc[b]);
        offset += blockLength(b);
    }

    uint32_t out = 0;
    for (int i = 0; i <= QR_SHORT_DATA_LEN; i++) {
        uint32_t blockStart = 0;
        for (int b = 0; b < QR_BLOCKS; b++) {
            if (i < blockLength(b)) {
                l_codewords[out++] = l_data[blockStart + i];
            }
            blockStart += blockLength(b);
        }
    }
    for (int i = 0; i < QR_ECC_LEN; i++) {
        for (int b = 0; b < QR_BLOCKS; b++) {
            l_codewords[out++] = l_ecc[b][i];
        }
    }
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
        if (a < 0) {
            return false;
        }
        if (i + 1 < length) {
            int b = alnumIndex(text[i + 1]);
            if (b < 0) {
                return false;
            }
            putBits(&pos, a * 45 + b, 11);
        } else {
            putBits(&pos, a, 6);
        }
    }

    uint32_t capacity = QR_DATA_CODEWORDS * 8;
    uint32_t terminator = capacity - pos < 4 ? capacity - pos : 4;
    putBits(&pos, 0, terminator);
    pos = (pos + 7) & ~7;
    for (uint8_t pad = 0xEC; pos < capacity; pad ^= 0xEC ^ 0x11) {
        putBits(&pos, pad, 8);
    }

    buildCodewords();
    memset(qr->modules, 0, sizeof(qr->modules));
    drawFunctionPatterns(qr);

    uint32_t bit = 0;
    for (int right = QR_SIZE - 1; right >= 1; right -= 2) {
        if (right == 6) {
            right = 5;
        }
        bool upward = ((right + 1) & 2) == 0;
        for (int vert = 0; vert < QR_SIZE; vert++) {
            int y = upward ? QR_SIZE - 1 - vert : vert;
            for (int j = 0; j < 2; j++) {
                int x = right - j;
                if (isFunctionModule(x, y)) {
                    continue;
                }
                bool dark = false;
                if (bit < QR_TOTAL_CODEWORDS * 8) {
                    dark = (l_codewords[bit >> 3] >> (7 - (bit & 7))) & 1;
                    bit++;
                }
                setModule(qr, x, y, dark);
            }
        }
    }

    int bestMask = 0;
    int bestPenalty = 0;
    for (int mask = 0; mask < QR_MASK_COUNT; mask++) {
        memcpy(&l_candidate, qr, sizeof(l_candidate));
        applyMask(&l_candidate, mask);
        drawFormat(&l_candidate, mask);
        int penalty = maskPenalty(&l_candidate);
        if (mask == 0 || penalty < bestPenalty) {
            bestMask = mask;
            bestPenalty = penalty;
        }
    }
    applyMask(qr, bestMask);
    drawFormat(qr, bestMask);
    return true;
}
