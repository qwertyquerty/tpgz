#ifndef TPGZ_MODULES_BOOT_INCLUDE_TIMER_H
#define TPGZ_MODULES_BOOT_INCLUDE_TIMER_H
#include "font.h"
#include "settings.h"

extern bool g_timerEnabled;
extern bool g_resetTimer;

enum TimerDisplay {
    TIMER_DISPLAY_BOTH,
    TIMER_DISPLAY_FRAMES,
    TIMER_DISPLAY_REAL_TIME,

    TIMER_DISPLAY_COUNT
};

namespace Timer {
void drawTimer();
void drawIGT();
void drawLoadTimer();
void restartOnLoad(GZSettingID trigger);
}  // namespace Timer

#endif
