#include "JSystem/JUtility/JUTGamePad.h"
#include "controller.h"
#include "m_Re/m_Re_controller_pad.h"

#ifdef GCN_PLATFORM
#define mPad_ (mPadButton__10JUTGamePad)
#endif
#ifdef WII_PLATFORM
#define mPad_ (mPad)
#endif

void setGamepadButtons(u16 buttons) {
#ifdef GCN_PLATFORM
    mPad_.mButton = buttons;
#endif

#ifdef WII_PLATFORM
    mPad.mHoldButton = buttons;
#endif
}

void setGamepadTrig(u16 buttons) {
#ifdef GCN_PLATFORM
    mPad_.mTrigger = buttons;
#endif

#ifdef WII_PLATFORM
    mPad.mTrigButton = buttons;
#endif
}
