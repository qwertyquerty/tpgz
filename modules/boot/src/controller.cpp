#include "JSystem/JUtility/JUTGamePad.h"
#include "cheats.h"
#include "commands.h"
#include "gz_flags.h"
#include "controller.h"
#include "SSystem/SComponent/c_counter.h"
#include "menus/menu.h"
#include "rels/include/defines.h"
#include "menus/utils/menu_mgr.h"

#ifdef GCN_PLATFORM
#define mPad_ (JUTGamePad::mPadButton[0])
#define buttonStatus (JUTGamePad::mPadStatus[0].button)
#define BUTTON_STATES 12
#define A_BUTTON CButton::A
#define TRIGGER_BUTTONS (CButton::L | CButton::R)
#else
#define buttonStatus (GZ_getWiiPadStatus().hold)
#define BUTTON_STATES 13
#define A_BUTTON CButton::A
#define TRIGGER_BUTTONS (CButton::Z | CButton::C)
#endif

#define REPEAT_TIME 4
#define REPEAT_DELAY 5

static uint16_t sButtonsLastFrame = 0;
static uint16_t sButtons = 0;
static uint16_t sButtonsPressed = 0;
static uint16_t sCursorEnableDelay = 0;

struct ButtonState {
    uint16_t button;
    uint32_t pressed_frame;
    bool is_down;
};

#ifdef GCN_PLATFORM
static ButtonState buttonStates[BUTTON_STATES] = {
    {CButton::DPAD_LEFT, 0xFFFFFFFF, false}, {CButton::DPAD_RIGHT, 0xFFFFFFFF, false},
    {CButton::DPAD_DOWN, 0xFFFFFFFF, false}, {CButton::DPAD_UP, 0xFFFFFFFF, false},
    {CButton::Z, 0xFFFFFFFF, false},         {CButton::R, 0xFFFFFFFF, false},
    {CButton::L, 0xFFFFFFFF, false},         {CButton::A, 0xFFFFFFFF, false},
    {CButton::B, 0xFFFFFFFF, false},         {CButton::X, 0xFFFFFFFF, false},
    {CButton::Y, 0xFFFFFFFF, false},         {CButton::START, 0xFFFFFFFF, false},
};
#else
static ButtonState buttonStates[BUTTON_STATES] = {
    {CButton::DPAD_LEFT, 0xFFFFFFFF, false}, {CButton::DPAD_RIGHT, 0xFFFFFFFF, false},
    {CButton::DPAD_DOWN, 0xFFFFFFFF, false}, {CButton::DPAD_UP, 0xFFFFFFFF, false},
    {CButton::PLUS, 0xFFFFFFFF, false},       {CButton::TWO, 0xFFFFFFFF, false},
    {CButton::ONE, 0xFFFFFFFF, false},        {CButton::B, 0xFFFFFFFF, false},
    {CButton::A, 0xFFFFFFFF, false},          {CButton::MINUS, 0xFFFFFFFF, false},
    {CButton::Z, 0xFFFFFFFF, false},          {CButton::C, 0xFFFFFFFF, false},
    {CButton::HOME, 0xFFFFFFFF, false},
};
#endif

static uint32_t getFrameCount() { return g_Counter.mCounter0; }

KEEP_FUNC void GZ_readController() {
    sButtonsLastFrame = sButtons;
    sButtons = buttonStatus;
    sButtonsPressed = sButtons & (0xFFFF ^ sButtonsLastFrame);

    for (uint8_t idx = 0; idx < BUTTON_STATES; idx++) {
        buttonStates[idx].is_down = (buttonStates[idx].button & sButtons) != 0;
        if ((buttonStates[idx].button & sButtonsPressed) != 0) {
            buttonStates[idx].pressed_frame = getFrameCount() + 1;
        }
    }

    GZ_applyCheats();
    if (g_menuMgr->isOpen()) {
#ifdef GCN_PLATFORM
        uint16_t current_input = GZ_getButtonStatus();
        if (!g_cursorEnabled) {
            if (current_input & CButton::DPAD_UP) {
                g_cursorEnabled = true;
            } else if (current_input & TRIGGER_BUTTONS) {
                sCursorEnableDelay = 0;
            } else if (sCursorEnableDelay < 1) {
                sCursorEnableDelay = 1;
            }
            if (sCursorEnableDelay >= 4) {
                g_cursorEnabled = true;
            } else if (sCursorEnableDelay > 0) {
                sCursorEnableDelay++;
            }
        }
#else
        g_cursorEnabled = true;
#endif
        setGamepadButtons(0);
        setGamepadTrig(0);

#ifdef WII_PLATFORM
        GZ_getWiiPadStatus().trig = 0;
#endif

#ifdef GCN_PLATFORM
        buttonStatus = 0;
        mPad_.mRepeat = 0;
#endif
    } else {
        g_cursorEnabled = false;
        sCursorEnableDelay = 0;
        GZCmd_processInputs();
    }
}

KEEP_FUNC bool GZ_getButtonPressed(int idx) { return buttonStates[idx].is_down; }

KEEP_FUNC bool GZ_getButtonRepeat(int idx, uint16_t repeat_time) {
    int32_t delta = getFrameCount() - buttonStates[idx].pressed_frame;
    bool just_clicked = delta == 0;
    bool held_down_long_enough = delta > REPEAT_DELAY;
    bool is_repeat_frame = held_down_long_enough && (delta % repeat_time == 0);
    return GZ_getButtonPressed(idx) && (just_clicked || is_repeat_frame);
}

KEEP_FUNC bool GZ_getButtonRepeat(int idx) { return GZ_getButtonRepeat(idx, REPEAT_TIME); }
KEEP_FUNC uint16_t GZ_getButtonStatus() { return buttonStatus; }
KEEP_FUNC uint16_t GZ_getButtonStatusSaved() { return sButtons; }

KEEP_FUNC bool GZ_getButtonTrig(int idx) {
    return GZ_getButtonPressed(idx) && getFrameCount() - buttonStates[idx].pressed_frame == 0;
}

KEEP_FUNC bool GZ_getButtonHold(int idx, int phase) {
    uint32_t delta = getFrameCount() - buttonStates[idx].pressed_frame;
    if (phase != POST_GAME_LOOP) delta++;
    return delta != 0;
}

KEEP_FUNC void GZ_getButtonPressCount(u8& count, int button, int gzButton) {
    if ((GZ_getButtonStatus() & button) && (buttonStates[gzButton].button & sButtonsPressed)) count++;
}

KEEP_FUNC bool GZ_getPadTrigAny(uint16_t pad) {
    for (uint8_t idx = 0; idx < BUTTON_STATES; idx++) {
        if ((pad & buttonStates[idx].button) && GZ_getButtonTrig(idx) && (sButtons & pad) == pad) {
            return true;
        }
    }
    return false;
}

void setGamepadButtons(u16 buttons) {
#ifdef GCN_PLATFORM
    mPad_.mButton = buttons;
#else
    GZ_getWiiPadStatus().hold = buttons;
#endif
}

void setGamepadTrig(u16 buttons) {
#ifdef GCN_PLATFORM
    mPad_.mTrigger = buttons;
#else
    GZ_getWiiPadStatus().trig = buttons;
#endif
}
