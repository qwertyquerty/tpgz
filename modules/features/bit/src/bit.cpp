#include "game_state.h"
#include "defines.h"
#include "d/actor/d_a_alink.h"
#ifdef WII_PLATFORM
#include "bit.h"
#include <cstdio>
#include "tpgz_math.h"
#include "JSystem/JUtility/JUTGamePad.h"
#include "d/d_com_inf_game.h"
#include "d/d_com_inf_game.h"
#include "f_op/f_op_camera_mng.h"
#include "f_op/f_op_scene_req.h"
#include "m_Do/m_Do_audio.h"
#include "fifo_queue.h"
#include "commands.h"
#include "controller.h"
#include "fs.h"
#include "font.h"
#include "save_manager.h"
#include "settings.h"

#define VOID_HEIGHT -2098.58
#define NORMAL_ACC -3.4
#define NORMAL_TERM_VEL -200.0
#define BOOTS_ACC -7.65
#define BOOTS_TERM_VEL -300.0
#define TARGET_FRAME 28

#define LAST_Y_GROUND_POS (((daAlink_c*)dComIfGp_getPlayer(0))->field_0x3404)

#ifdef WII_PLATFORM
KEEP_FUNC void GZCmd_bitPractice() {
    if (GZCmd_checkTrig(GZStng_getData<uint16_t>(STNG_CMD_BIT, BACK_IN_TIME_BUTTONS))) {
        // TODO: maybe simplify this
        special sp[] = {
            special(0, NULL, BiTIndicator::setPosition),
        };

        SaveManager::triggerLoad(0, "any", sp, 1);
    }
}
#endif

static char buf[30];

KEEP_FUNC void BiTIndicator::setPosition() {
    dComIfGp_getPlayer(0)->current.pos.set(466.622467f, 319.770752f, -11651.3867f);
    dComIfGp_getPlayer(0)->shape_angle.y = 32000;
    dComIfGp_getCamera(0)->mCamera.mViewCache.mCenter = cXyz(465.674622f, 421.052704f, -11651.0684f);
    dComIfGp_getCamera(0)->mCamera.mViewCache.mEye = cXyz(735.525391f, 524.418701f, -11576.4746f);
}

KEEP_FUNC void BiTIndicator::execute() {
    double dt = 0;

    if (dComIfGp_getPlayer(0)) {
        const bool has_boots = daPy_getPlayerActorClass()->checkNoResetFlg0(daPy_py_c::FLG0_UNK_2) != 0;
        const double term_vel = has_boots ? BOOTS_TERM_VEL : NORMAL_TERM_VEL;
        const double acc = has_boots ? BOOTS_ACC : NORMAL_ACC;
        const double v_y1 = dComIfGp_getPlayer(0)->speed.y;
        const double dist_from_last_ground =
            (dComIfGp_getPlayer(0)->current.pos.y - LAST_Y_GROUND_POS);

        // Calculate how many frames before reaching terminal velocity
        double dt_1 = (term_vel - v_y1) / acc;
        // Calculate how much height we will lose during that period
        double x_dt_1 = dist_from_last_ground + v_y1 * dt_1 + 0.5 * acc * dt_1 * dt_1;

        // If we reach terminal velocity after the voiding point, ...
        if (x_dt_1 <= VOID_HEIGHT) {
            // ... just calculate the time remaining before void using the quadratic formula
            dt = (-v_y1 -
                  sqrt((double)(v_y1 * v_y1 + 2 * acc * (VOID_HEIGHT - dist_from_last_ground)))) /
                     acc +
                 0.5;
        } else {
            // ... else, the time remaining before the void is the time to reach terminal velocity +
            // linear time at velocity
            dt = dt_1 + (VOID_HEIGHT - x_dt_1) / term_vel + 0.5;
        }

        // if (l_dvdError$0.is_visible == 0 && !l_fopScnRq_IsUsingOfOverlap.isLoading) {
        //     sprintf(buf, "frames before void: %d", (int)dt);
        //     log.PrintLog(buf, DEBUG);
        // }

        if (daAlink_c::checkStageName("F_SP104") && GZ_getButtonPressed(HOME) &&
            !GZ_isDvdErrorShown() && !l_fopScnRq_IsUsingOfOverlap) {
            if ((int)dt == TARGET_FRAME) {
                snprintf(buf, sizeof(buf), "Got it");
            }
            if ((int)dt > TARGET_FRAME) {
                snprintf(buf, sizeof(buf), "%d frames early", (int)dt - TARGET_FRAME);
            }
            if ((int)dt < TARGET_FRAME) {
                snprintf(buf, sizeof(buf), "%d frames late", TARGET_FRAME - (int)dt);
            }
            FIFOQueue::push(buf, Queue);
        }
    }
}
#endif