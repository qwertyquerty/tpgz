#include "d/actor/d_a_alink.h"
#include "m_Do/m_Do_controller_pad.h"
#include "game_state.h"
#include "gz_flags.h"
#include <cstring>
#include "controller.h"
#include "fifo_queue.h"
#include "scene.h"
#include "tools.h"
#include "utils/audio.h"
#include "utils/lines.h"
#include "utils/link.h"
#include "utils/loading.h"
#include "flaglog.h"
#include "save_manager.h"
#include "memfiles.h"
#include "utils/draw.h"
#include "JSystem/JUtility/JUTGamePad.h"
#include "f_op/f_op_scene_req.h"
#include "rels/include/defines.h"
#include "m_Do/m_Do_printf.h"

bool g_framePaused = false;

// Initialized in the "init" module
KEEP_VAR tpgz::containers::deque<GZFlag*> g_gzFlags;

#ifdef GCN_PLATFORM
#define HOLD_BTNS mDoCPd_c::getCpadInfo(0).mButtonFlags
#define TRIG_BTNS mDoCPd_c::getCpadInfo(0).mPressedButtonFlags
#endif

#ifdef WII_PLATFORM
#define HOLD_BTNS GZ_getWiiPadStatus().hold
#define TRIG_BTNS GZ_getWiiPadStatus().trig
#endif

KEEP_FUNC void GZFlg_addFlag(GZFlag* flag) {
    g_gzFlags.push_back(flag);
}

KEEP_FUNC GZFlag* GZFlg_removeFlag(GZFlags flag_id) {
    tpgz::containers::deque<GZFlag*>::iterator it = g_gzFlags.begin();
    for (;it != g_gzFlags.end(); ++it) {
        if ((*it)->id == flag_id) {
            break;
        }
    }
    GZFlag* flag = *it;
    g_gzFlags.erase(it);
    return flag;
}



KEEP_FUNC void GZ_frameAdvance() {
    if (!g_framePaused) {
        return;
    }
    static int holdCounter = 0;
    static uint32_t buttonsPrev = 0;
    sPauseTimer = 1;

    TRIG_BTNS = HOLD_BTNS & ~buttonsPrev;

    uint16_t frameAdvancePad = GZStng_getData(STNG_CMD_FRAME_ADVANCE, FRAME_ADVANCE_PAD);

    if (HOLD_BTNS & frameAdvancePad) {
        holdCounter++;
    } else {
        holdCounter = 0;
    }

    if (GZ_getPadTrigAny(frameAdvancePad)) {
        // this sets pause timer to 0 for 1 frame,
        // which lets 1 frame pass before pausing again
        sPauseTimer = 0;
        buttonsPrev = HOLD_BTNS;
        HOLD_BTNS &= ~frameAdvancePad;
    }

    // frames start passing at normal speed after holding for 30 frames
    if (holdCounter >= 30) {
        sPauseTimer = 0;
        buttonsPrev = HOLD_BTNS;
        HOLD_BTNS &= ~frameAdvancePad;
    }
}

void GZ_drawFrameTex(Texture* pauseTex, Texture* playTex) {
    if (g_framePaused) {
        if (sPauseTimer == 1) {
            Draw::drawRect(0xFFFFFFFF, makeVec2(550.0f, 5.0f), makeVec2(32 * (isWidescreen ? 0.75f : 1.0f), 32),
                           &pauseTex->_texObj);
        } else {
            Draw::drawRect(0xFFFFFFFF, makeVec2(550.0f, 5.0f), makeVec2(32 * (isWidescreen ? 0.75f : 1.0f), 32),
                           &playTex->_texObj);
        }
    }
}

void GZ_execute(int phase) {
    for (tpgz::containers::deque<GZFlag*>::iterator gzFlagIt = g_gzFlags.begin(); gzFlagIt != g_gzFlags.end(); ++gzFlagIt) {
        GZFlag* gzFlag = *gzFlagIt;
        if (gzFlag->mPhase == phase && gzFlag->mpFlag != NULL) {
            if (gzFlag->mpFlag() && gzFlag->mpActiveFunc) {
                gzFlag->mpActiveFunc();
            } else if (gzFlag->mpDeactiveFunc) {
                gzFlag->mpDeactiveFunc();
            }
        }
    }

    if (GZStng_getData(STNG_TOOLS_SAND, false)) {
        if (dComIfGp_getPlayer(0) != NULL) {
            ((daAlink_c*)dComIfGp_getPlayer(0))->mSinkShapeOffset = 0.0f;
        }
    }

    // separate variable to make sure the after-callback is only run after a load has happened
    static bool load_started = false;
    static bool load_finished_will_teleport = false;
    if (l_fopScnRq_IsUsingOfOverlap && !load_started) {
        load_started = true;
    }

    // Check for post load callback and run it once link is valid
    if (load_started && !l_fopScnRq_IsUsingOfOverlap && dComIfGp_getPlayer(0)) {
        if (gSaveManager.mPracticeFileOpts.inject_options_after_load) {
            gSaveManager.mPracticeFileOpts.inject_options_after_load();
            gSaveManager.mPracticeFileOpts.inject_options_after_load = NULL;
        }
        load_started = false;
        if (gSaveManager.mPracticeFileOpts.inject_options_after_counter > 0) {
            load_finished_will_teleport = true;
        }
    }

    // maybe a bit convoluted but if Link needs to be teleported x frames after the post load
    if (load_finished_will_teleport && !l_fopScnRq_IsUsingOfOverlap && dComIfGp_getPlayer(0)) {
        if (gSaveManager.mPracticeFileOpts.inject_options_after_counter > 0) {
            gSaveManager.mPracticeFileOpts.inject_options_after_counter--;
        } else {
            gSaveManager.setLinkInfo();
            load_finished_will_teleport = false;
        }
    }

    // normally oxygen doesn't get set until going to the file select screen
    // so this fixes oxygen issues when loading a save from title screen directly after boot
    if (g_dComIfG_gameInfo.play.getMaxOxygen() == 0) {
        dComIfGp_setOxygen(600);
        dComIfGp_setNowOxygen(600);
        dComIfGp_setMaxOxygen(600);
    }
}

#define ACTIVE_FLAG_FUNC(name, stngId) \
    KEEP_FUNC bool name() { return GZStng_getData(stngId, false); }

ACTIVE_FLAG_FUNC(GZ_freezeActors_active, STNG_SCENE_FREEZE_ACTOR)
ACTIVE_FLAG_FUNC(GZ_hideActors_active, STNG_SCENE_HIDE_ACTOR)
ACTIVE_FLAG_FUNC(GZ_freezeCamera_active, STNG_SCENE_FREEZE_CAMERA)
ACTIVE_FLAG_FUNC(GZ_hideHUD_active, STNG_SCENE_HIDE_HUD)
ACTIVE_FLAG_FUNC(GZ_freezeTime_active, STNG_SCENE_FREEZE_TIME)
ACTIVE_FLAG_FUNC(GZ_disableBgm_active, STNG_SCENE_DISABLE_BG)
ACTIVE_FLAG_FUNC(GZ_disableSFX_active, STNG_SCENE_DISABLE_SFX)
