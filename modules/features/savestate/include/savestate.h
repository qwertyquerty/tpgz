#ifndef TPGZ_MODULES_FEATURES_SAVESTATE_INCLUDE_SAVESTATE_H
#define TPGZ_MODULES_FEATURES_SAVESTATE_INCLUDE_SAVESTATE_H
#include "controller.h"

#ifdef GCN_PLATFORM
#define SAVE_STATE_BUTTONS (CButton::R | CButton::X)
#define LOAD_STATE_BUTTONS (CButton::R | CButton::Y)
#endif

#ifdef WII_PLATFORM
#define SAVE_STATE_MODIFIER_BUTTONS (CButton::Z | CButton::C)
#define SAVE_STATE_DPAD_BUTTONS (CButton::DPAD_LEFT | CButton::DPAD_RIGHT | CButton::DPAD_DOWN | CButton::DPAD_UP)
#define SAVE_STATE_BUTTONS (SAVE_STATE_MODIFIER_BUTTONS | CButton::DPAD_LEFT)
#define LOAD_STATE_BUTTONS (SAVE_STATE_MODIFIER_BUTTONS | CButton::DPAD_RIGHT)
#endif

void GZ_initSaveStates();
void GZ_exitSaveStates();
void GZ_handleSaveStates();

#endif
