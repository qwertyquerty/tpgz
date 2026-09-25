#include "m_Do/m_Do_audio.h"
#include "defines.h"
#include "utils/audio.h"

KEEP_FUNC void GZ_enableBGM() {
    Z2GetSeqMgr()->field_0x84.intensity_ = 1.0f;  // BG Audio
#ifndef WII_PLATFORM
    Z2GetSoundMgr()->getSeqMgr()->getParams()->params_.mVolume = 1.0f;
    Z2GetSoundMgr()->getStreamMgr()->getParams()->params_.mVolume = 1.0f;
#endif
}

KEEP_FUNC void GZ_disableBGM() {
    Z2GetSeqMgr()->field_0x84.intensity_ = 0.0f;  // BG Audio
#ifndef WII_PLATFORM
    Z2GetSoundMgr()->getSeqMgr()->getParams()->params_.mVolume = 0.0f;
    Z2GetSoundMgr()->getStreamMgr()->getParams()->params_.mVolume = 0.0f;
#endif
}

KEEP_FUNC void GZ_disableSFX() {
    for (int i = 0; i < 16; i++) {
        Z2GetSoundMgr()->getSeMgr()->getCategory(i)->getParams()->params_.mVolume = 0.0f;
    }

    Z2GetSoundMgr()->getSeMgr()->getParams()->params_.mVolume = 0.0f;
}

KEEP_FUNC void GZ_enableSFX() {
    for (int i = 0; i < 16; i++) {
        Z2GetSoundMgr()->getSeMgr()->getCategory(i)->getParams()->params_.mVolume = 1.0f;
    }

    Z2GetSoundMgr()->getSeMgr()->getParams()->params_.mVolume = 1.0f;
}
