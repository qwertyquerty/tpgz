#include "game_state.h"
#include "defines.h"
#include <cstdio>
#include "fast_eel_regrab_check.h"
#include "controller.h"
#include "fifo_queue.h"
#include "d/d_com_inf_game.h"
#include "SSystem/SComponent/c_counter.h"
#include "f_op/f_op_scene_req.h"

#ifdef GCN_PLATFORM
#define X_HELD_CHECK !GZ_getButtonHold(X)
#define X_DOWN_CHECK GZ_getButtonPressed(X)
#define Y_HELD_CHECK !GZ_getButtonHold(Y)
#define Y_DOWN_CHECK GZ_getButtonPressed(Y)

#else
#define X_HELD_CHECK !GZ_getButtonHold(DPAD_DOWN)
#define X_DOWN_CHECK GZ_getButtonPressed(DPAD_DOWN)
#define Y_HELD_CHECK !GZ_getButtonHold(B)
#define Y_DOWN_CHECK GZ_getButtonPressed(B)
#endif

#define first_frame 9

#define PAD Pad

KEEP_FUNC void FastEelRegrabChecker::execute() {
    static bool sTimerStarted = false;
    static bool sGoalHit = false;
    static uint32_t sFrameCount = 0;

    // reset counters on load
    if (l_fopScnRq_IsUsingOfOverlap) {
        sFrameCount = 0;
        sGoalHit = false;
        sTimerStarted = false;
    }

    bool claw_on_x = dComIfGs_getSelectItemIndex(SELECT_ITEM_X) == 9; // claw on x

    if (!sTimerStarted){
        if ((claw_on_x && X_DOWN_CHECK) || (!claw_on_x && Y_DOWN_CHECK)) {
            sTimerStarted = true;
        }
    }

    if (sTimerStarted) {
        sFrameCount++;

        if (sFrameCount < 20) {
            if (!sGoalHit && ((claw_on_x && Y_DOWN_CHECK && Y_HELD_CHECK) || (!claw_on_x && X_DOWN_CHECK && X_HELD_CHECK))) {
                char buf[20];
                if (sFrameCount < first_frame) {
                    snprintf(buf, sizeof(buf), "boots %df early", first_frame - sFrameCount);
                    FIFOQueue::push(buf, Queue, 0x0000FF00);
                } else if (sFrameCount == first_frame) {
                    FIFOQueue::push("boots 1st frame", Queue, 0x00CC0000);
                } else if (sFrameCount == first_frame + 1) {
                    FIFOQueue::push("boots 2nd frame", Queue, 0x00CC0000);
                } else if (sFrameCount == first_frame + 2) {
                    FIFOQueue::push("boots 3rd frame", Queue, 0x00CC0000);
                } else if (sFrameCount > first_frame + 2) {
                    snprintf(buf, sizeof(buf), "boots %df late", sFrameCount - first_frame - 2);
                    FIFOQueue::push(buf, Queue, 0x99000000);
                    sGoalHit = true;
                }
            }
        } else {
            sFrameCount = 0;
            sGoalHit = false;
            sTimerStarted = false;
        }
    }
}