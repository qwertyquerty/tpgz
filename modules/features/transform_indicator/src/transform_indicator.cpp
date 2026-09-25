#include "transform_indicator.h"
#include <cstdio>
#include "pos_settings.h"
#include "tools.h"
#include "rels/include/defines.h"
#include "utils/texture.h"
#include "utils/draw.h"
#include "settings.h"
#include "global_data.h"
#include "d/d_com_inf_game.h"
#include "f_op/f_op_actor_iter.h"
#include "d/actor/d_a_alink.h"

extern daMidna_c m_midnaActor__9daPy_py_c;

#include "game_state.h"

namespace TransformIndicator {
Texture l_humanTex;
Texture l_wolfTex;
}  // namespace TransformIndicator

void drawTexture(GXTexObj tex, bool greyed) {
    Draw::drawRect(greyed ? 0x3f3f3f7f : 0xFFFFFFFF, GZ_getSpriteOffset(STNG_SPRITES_TRANSFORM_IND),
                   makeVec2(30 * (isWidescreen ? 0.75f : 1.0f), 30), &tex);
}

KEEP_FUNC void TransformIndicator::draw() {
    bool cantTransform = !((*(uint8_t*)g_dComIfG_gameInfo.play.mPlayerPtr + 0x570) & 4) ||
        (g_env_light.mEvilInitialized & 0x80) ||
        (!g_dComIfG_gameInfo.info.getEvent().isEventBit(0xD04U));
        // TODO Add a the implementation of fopAcIt_Judge and daMidna_searchNpc for the last check.
    if (dComIfGs_getTransformStatus() == 0) {
        if (l_wolfTex.loadCode == TEX_OK) {
            drawTexture(l_wolfTex._texObj, !cantTransform);
        }
    } else {
        if (l_humanTex.loadCode == TEX_OK) {
            drawTexture(l_humanTex._texObj, !cantTransform);
        }
    }
}