#ifndef TPGZ_MODULES_BOOT_INCLUDE_TIMER_H
#define TPGZ_MODULES_BOOT_INCLUDE_TIMER_H
#include "font.h"

extern bool g_timerEnabled;
extern bool g_resetTimer;

namespace Timer {
void drawTimer();
void drawIGT();
void drawLoadTimer();
}  // namespace Timer

#endif
