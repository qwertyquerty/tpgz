#include "defines.h"
#include "d/actor/d_a_alink.h"
#include "controller.h"
#include "ee_checker.h"
#include "controller.h"
#include "fifo_queue.h"
#include <cstdio>
#include "d/d_com_inf_game.h"
#include "f_op/f_op_actor_mng.h"
#include "m_Do/m_Do_printf.h"
#include "f_pc/f_pc_name.h"
#include "SSystem/SComponent/c_counter.h"

u16 previous_action;        // Tracks the previous action
char msg_buffer[20];        // Buffer for the message to be printed
s32 early_roll_frame;       // Frame where the early roll happened
s32 late_roll_frame;        // Frame where the late roll happened
s32 target_frame;           // Frame where the roll *should* happen to get the trick
const u8 metamorphose_anm_length = 56; // length of the metamorphose animation in frames
s32 metamorphose_start_frame; // frame that the metamorphose animation starts on

void checkRollFrame(daAlink_c* link) {
    switch (link->mProcID) {
    case daAlink_c::PROC_TALK:
        previous_action = daAlink_c::PROC_TALK;
        break;
    case daAlink_c::PROC_METAMORPHOSE:
        if (previous_action == daAlink_c::PROC_TALK) {
            metamorphose_start_frame = GZ_getFrameCount();
        }

#if TPGZ_DEBUG
        OSReport("previous action: %d\n", previous_action);
        OSReport("metamorphose start frame: %d\n", metamorphose_start_frame);
        OSReport("Current metamorphose frame: %d\n", (GZ_getFrameCount() - metamorphose_start_frame));
#endif
            
        if (GZ_getButtonPressed(A) && !GZ_getButtonHold(A)) {
            early_roll_frame = GZ_getFrameCount();
            snprintf(msg_buffer, sizeof(msg_buffer), "early by %df", (int)(metamorphose_anm_length - (GZ_getFrameCount() - metamorphose_start_frame)));
            FIFOQueue::push(msg_buffer, Queue, 0x0000CC00);
        }

        previous_action = daAlink_c::PROC_METAMORPHOSE;
        break;

    case daAlink_c::PROC_WAIT:
        late_roll_frame = 0;
        target_frame = GZ_getFrameCount();
        break;
    case daAlink_c::PROC_MOVE:
    case daAlink_c::PROC_WAIT_TURN:
    case daAlink_c::PROC_MOVE_TURN:
        if (GZ_getButtonPressed(A) && !GZ_getButtonHold(A)) {
            late_roll_frame = GZ_getFrameCount();
        }
        break;
    case daAlink_c::PROC_FRONT_ROLL:
#if TPGZ_DEBUG
            OSReport("Front roll, last action: %d\n", previous_action);
            OSReport("Early roll frame: %d\n", early_roll_frame);
            OSReport("Late roll frame: %d\n", late_roll_frame);
#endif
        if (late_roll_frame == 0 && previous_action != daAlink_c::PROC_FRONT_ROLL) {
            FIFOQueue::push("<3", Queue, 0x00CC0000);
        } else if (previous_action != daAlink_c::PROC_FRONT_ROLL) {
            snprintf(msg_buffer, sizeof(msg_buffer), "late by %df", (int)(late_roll_frame - target_frame));
            FIFOQueue::push(msg_buffer, Queue, 0xCC000000);
        }

        previous_action = daAlink_c::PROC_FRONT_ROLL;
        break;
    default:
        early_roll_frame = 0;
        late_roll_frame = 0;
    }
}

KEEP_FUNC void EEChecker::execute() {
    daAlink_c* link = (daAlink_c*)dComIfGp_getPlayer(0);

    if (!link) {
#if TPGZ_DEBUG
        OSReport("Player is not loaded\n");
#endif
        return;
    }

    if (!daAlink_c::checkStageName("R_SP110")) {
#if TPGZ_DEBUG
        OSReport("Player is not in goron elder room\n");
#endif
        return;
    }

    if (link->current.pos.x > -1400.0f || link->current.pos.x < -1600.0f) {
#if TPGZ_DEBUG
        OSReport("Player is not in the correct x position\n");
#endif
        return;
    }

    if (link->current.pos.z < 4000.0f || link->current.pos.z > 4400.0f) {
#if TPGZ_DEBUG
        OSReport("Player is not in the correct z position\n");
#endif
        return;
    }

    checkRollFrame(link);
}