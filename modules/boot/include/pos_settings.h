#ifndef TPGZ_MODULES_BOOT_INCLUDE_POS_SETTINGS_H
#define TPGZ_MODULES_BOOT_INCLUDE_POS_SETTINGS_H
#include "settings.h"
#include "tpgz_vec.h"

#define SPRITES_AMNT 12

enum SpritesIndex {
    MENU_INDEX,
    VIEWER_INDEX,
    DEBUG_INFO_INDEX,
    STAGE_INFO_INDEX,
    TIMER_SPR_INDEX,
    LOAD_TIMER_SPR_INDEX,
    IGT_TIMER_SPR_INDEX,
    FIFO_SPR_INDEX,
    HEAP_INFO_INDEX,
    MASH_INFO_INDEX,
    TRANSFORM_IND_INDEX,
    DISPLACEMENT_INDEX
};

inline Vec2 GZ_getSpriteOffset(GZSettingID sprIdx) {
    return GZStng_getData(sprIdx, makeVec2(0.0f, 0.0f));
}

#endif
