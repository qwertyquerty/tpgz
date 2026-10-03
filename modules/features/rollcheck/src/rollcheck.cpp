#include "defines.h"
#include "d/actor/d_a_alink.h"
#include "controller.h"
#include "rollcheck.h"
#include <cstdio>
#include "controller.h"
#include "fifo_queue.h"
#include "d/d_com_inf_game.h"
#include "d/d_menu_window.h"
#include "SSystem/SComponent/c_counter.h"
#include "m_Do/m_Do_printf.h"

u8 current_frame_delta;     // The current active (unpaused) frame delta since the roll started.
u8 roll_end_frame;          // The frame the roll ends on. Dynamically updated based on the previous action
u8 roll_early_check_frame;  // The earliest frame the early check should be performed on. Dynamically updated based on the previous action.
u8 roll_late_check_frame;   // The latest frame the late check should be performed on. Dynamically updated based on the previous action.
u16 previous_action;        // Tracks the previous action for adjusting the frame timing window, because some actions like land dive cut the beginning of the roll short
char msg_buffer[20];        // Buffer for the message to be printed.

#if TPGZ_DEBUG
KEEP_FUNC void RollIndicator_debug() {
    OSReport("current frame: %d\n", GZ_getFrameCount());
    OSReport("current frame delta: %d\n", current_frame_delta);
}
#endif

KEEP_FUNC void RollIndicator::execute() {
    // Only run the flow if the game is not in an event, 
    // the player is in human form,
    // and the player pointer is valid
    if (dComIfGp_getEvent()->mEventStatus == false && dComIfGs_getTransformStatus() == TF_STATUS_HUMAN && dComIfGp_getPlayer(0)) {
#if TPGZ_DEBUG
        OSReport("-------\n");
        OSReport("running!\n");
        OSReport("current action: %d\n", ((daAlink_c*)dComIfGp_getPlayer(0))->mProcID);
#endif

        switch (((daAlink_c*)dComIfGp_getPlayer(0))->mProcID) {
        case daAlink_c::PROC_FRONT_ROLL:
            // this may need to be change to a switch in the future
            // if more actions are found that cut the roll short
            if (previous_action == daAlink_c::PROC_DIVE_JUMP) {
                roll_end_frame = 15;
                roll_early_check_frame = 9;
                roll_late_check_frame = 20;
            } else {
                roll_end_frame = 20;
                roll_early_check_frame = 14;
                roll_late_check_frame = 25;
            }

#if TPGZ_DEBUG
            OSReport("target roll end frame: %d\n", roll_end_frame);
            OSReport("front roll!\n");
#endif

            if (dComIfGp_isPauseFlag()) {
#if TPGZ_DEBUG
                OSReport("game paused!\n");
#endif
            } else {
                if (current_frame_delta == 0) {
                    current_frame_delta = 1;
                }

#if TPGZ_DEBUG
                RollIndicator_debug();
#endif

                if (GZ_getButtonPressed(A) && !GZ_getButtonHold(A)) {
                    if (current_frame_delta == roll_end_frame) {
#if TPGZ_DEBUG
                        OSReport("exactly %d frames!\n", roll_end_frame);
#endif
                        FIFOQueue::push("<3", Queue, 0x00CC0000);
                        current_frame_delta = 0;
                    } else if (current_frame_delta > roll_early_check_frame && current_frame_delta < roll_end_frame) {
#if TPGZ_DEBUG
                        OSReport("between %d and %d frames!\n", roll_early_check_frame, roll_end_frame);
#endif
                        snprintf(msg_buffer, sizeof(msg_buffer), "early by %d", (roll_end_frame - current_frame_delta));
                        FIFOQueue::push(msg_buffer, Queue, 0x0000FF00);
                    } else if (current_frame_delta > roll_end_frame && current_frame_delta <= roll_late_check_frame) {
#if TPGZ_DEBUG
                        OSReport("Between %d and %d frames!\n", roll_end_frame, roll_late_check_frame);
#endif
                        snprintf(msg_buffer, sizeof(msg_buffer), "late by %d", (current_frame_delta - roll_end_frame));
                        FIFOQueue::push(msg_buffer, Queue, 0x99000000);
                        current_frame_delta = 0;
                    }
                }

                if (current_frame_delta != 0) {
                    current_frame_delta++;
                }
            }

            break;
        default:
#if TPGZ_DEBUG
            OSReport("Default!\n");
#endif
            if (dComIfGp_isPauseFlag()) {
#if TPGZ_DEBUG
                OSReport("game paused!\n");
#endif
            } else {
#if TPGZ_DEBUG
                RollIndicator_debug();
#endif

                if (GZ_getButtonPressed(A) && !GZ_getButtonHold(A)) {
                    if (current_frame_delta > roll_end_frame && current_frame_delta <= roll_late_check_frame) {
#if TPGZ_DEBUG
                        OSReport("Between %d and %d frames!\n", roll_end_frame, roll_late_check_frame);
#endif
                        snprintf(msg_buffer, sizeof(msg_buffer), "late by %d", (current_frame_delta - roll_end_frame));
                        FIFOQueue::push(msg_buffer, Queue, 0x99000000);
                        current_frame_delta = 0;
                    }
                }

                if (current_frame_delta > roll_end_frame && current_frame_delta <= roll_late_check_frame) {
                    current_frame_delta++;
                } else {
                    current_frame_delta = 0;
                }
            }

            // Update the previous action to the current action
            previous_action = ((daAlink_c*)dComIfGp_getPlayer(0))->mProcID;
            break;
        }
    } else {
        current_frame_delta = 0;
    }
}