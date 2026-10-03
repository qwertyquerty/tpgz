#ifndef TPGZ_MODULES_BOOT_INCLUDE_CONTROLLER_H
#define TPGZ_MODULES_BOOT_INCLUDE_CONTROLLER_H
#include <stdint.h>
#include "SSystem/SComponent/c_counter.h"
#include "JSystem/JUtility/JUTGamePad.h"
#include "types.h"

namespace CButton {
#ifdef GCN_PLATFORM
enum {
    DPAD_LEFT = 0x0001, DPAD_RIGHT = 0x0002, DPAD_DOWN = 0x0004, DPAD_UP = 0x0008,
    Z = 0x0010, R = 0x0020, L = 0x0040, A = 0x0100, B = 0x0200, X = 0x0400,
    Y = 0x0800, START = 0x1000,
};
#else
enum {
    DPAD_LEFT = 0x0001, DPAD_RIGHT = 0x0002, DPAD_DOWN = 0x0004, DPAD_UP = 0x0008,
    PLUS = 0x0010, TWO = 0x0100, ONE = 0x0200, B = 0x0400, A = 0x0800,
    MINUS = 0x1000, Z = 0x2000, C = 0x4000, HOME = 0x8000,
};
#endif
}

#ifdef GCN_PLATFORM
enum GZPad { DPAD_LEFT, DPAD_RIGHT, DPAD_DOWN, DPAD_UP, Z, R, L, A, B, X, Y, START };
#else
enum GZPad { DPAD_LEFT, DPAD_RIGHT, DPAD_DOWN, DPAD_UP, PLUS, TWO, ONE, B, A, MINUS, Z, C, HOME };
#endif

#ifdef WII_PLATFORM
#include "m_Re/m_Re_controller_pad.h"

inline KPADStatus& GZ_getWiiPadStatus() {
    return mReCPd::getPad(0).field_0x4[0];
}
#endif

bool GZ_getButtonRepeat(int idx);
bool GZ_getButtonRepeat(int idx, uint16_t repeat_time);
bool GZ_getButtonPressed(int idx);
bool GZ_getButtonHold(int idx, int phase = 0);
uint16_t GZ_getButtonStatus();
uint16_t GZ_getButtonStatusSaved();
void GZ_readController();
bool GZ_getButtonTrig(int idx);
void GZ_getButtonPressCount(u8& i_pressCounter, int i_button, int i_gzButton);
bool GZ_getPadTrigAny(uint16_t pad);
inline uint32_t GZ_getFrameCount() { return g_Counter.mCounter0; }

void setGamepadButtons(u16 buttons);
void setGamepadTrig(u16 buttons);

#endif
