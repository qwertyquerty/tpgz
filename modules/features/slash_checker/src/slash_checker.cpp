#include "game_state.h"
#include "d/actor/d_a_alink.h"
#include "defines.h"
#ifdef WII_PLATFORM

#include <cstdio>
#include "slash_checker.h"
#include "controller.h"
#include "fifo_queue.h"
#include "d/d_com_inf_game.h"
#include "SSystem/SComponent/c_counter.h"
#include "f_op/f_op_scene_req.h"

#define ITEM_BUTTON_HELD_CHECK (!GZ_getButtonHold(A) || !GZ_getButtonHold(MINUS))
#define ITEM_BUTTON_DOWN_CHECK (GZ_getButtonPressed(A) || GZ_getButtonPressed(MINUS))
#define PAD Pad

KEEP_FUNC void SlashChecker::execute() {
    static bool sTimerStarted = false;
    static bool didFirstSwing = false;
    static uint32_t sFrameCount = 0;

    if (dComIfGp_getPlayer(0) == NULL) {
        return;
    }

    // reset counters on load
    if (l_fopScnRq_IsUsingOfOverlap) {
        sFrameCount = 0;
        sTimerStarted = false;
        didFirstSwing = false;
    }

    if (ITEM_BUTTON_HELD_CHECK && ITEM_BUTTON_DOWN_CHECK && ((daAlink_c*)dComIfGp_getPlayer(0))->mProcID != 33) {
        sTimerStarted = true;
        sFrameCount = 0;
        didFirstSwing = false;
    }

    uint32_t correctFrames = 7;

    if (sTimerStarted) {
        sFrameCount++;

        if (sFrameCount < 15) {
            if (mReCPd::getPad(0).m_remAcc.m_swing > 0) { // slash

                if (!didFirstSwing) {
                    FIFOQueue::push("<", Queue);
                    didFirstSwing = true;
                }

                char buf[20];
                if (sFrameCount < correctFrames) {
                    snprintf(buf, sizeof(buf), "%df early", correctFrames - sFrameCount);
                    FIFOQueue::push(buf, Queue, 0x0000FF00);
                } else if (sFrameCount == correctFrames) {
                    FIFOQueue::push("got it", Queue, 0x00CC0000);
                } else if (sFrameCount > correctFrames) {
                    if (sFrameCount - correctFrames == 1) {
                        FIFOQueue::push("1f late", Queue, 0xFF757500);
                    } else {
                        snprintf(buf, sizeof(buf), "%df late", sFrameCount - correctFrames);
                        FIFOQueue::push(buf, Queue, 0x99000000);
                    }
                }
            }
        } else {
            sFrameCount = 0;
            sTimerStarted = false;
            if (didFirstSwing) {
                FIFOQueue::push(">", Queue);
                didFirstSwing = false;
            }
            
        }
    }
}

#endif
