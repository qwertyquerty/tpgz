#ifndef TPGZ_MODULES_BOOT_INCLUDE_UTILS_CURSOR_H
#define TPGZ_MODULES_BOOT_INCLUDE_UTILS_CURSOR_H
#include <stdint.h>
#include "controller.h"

#ifdef GCN_PLATFORM
#define SCROLL_P10_BTN X
#define SCROLL_M10_BTN Y
#endif

#ifdef WII_PLATFORM
#define SCROLL_P10_BTN TWO
#define SCROLL_M10_BTN ONE
#endif

struct Cursor {
    enum Mode {
        MODE_SINGLE_COLUMN,
        MODE_LIST,
        MODE_UNRESTRICTED,
    };

    Cursor() { reset(); }

    void move(int max_x, int max_y);

    template <typename T>
    static void moveList(T& index) {
        if (GZ_getButtonRepeat(DPAD_LEFT)) {
            index--;
        } else if (GZ_getButtonRepeat(DPAD_RIGHT)) {
            index++;
        } else if (GZ_getButtonRepeat(SCROLL_M10_BTN)) {
            index -= 10;
        } else if (GZ_getButtonRepeat(SCROLL_P10_BTN)) {
            index += 10;
        }
    }

    void reset();
    void lock(bool x, bool y);
    void setMode(uint8_t m);

    int x;
    int y;
    uint8_t mode;
    bool lock_x;
    bool lock_y;
};

extern bool g_cursorEnabled;

void GZ_setCursorColor();

#endif
