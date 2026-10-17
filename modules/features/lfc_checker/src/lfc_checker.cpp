#include "defines.h"
#include "d/actor/d_a_alink.h"
#include "controller.h"
#include "lfc_checker.h"
#include "controller.h"
#include "fifo_queue.h"
#include <cstdio>
#include "d/d_com_inf_game.h"
#include "f_op/f_op_actor_mng.h"
#include "m_Do/m_Do_printf.h"
#include "f_pc/f_pc_name.h"
#include "SSystem/SComponent/c_counter.h"

enum Phase {
    LADDER_START,
    LADDER_DURING,
    LADDER_DURING_2,
    LADDER_DROP,
    LADDER_DROP_2,
    LADDER_REGRAB
};

u8 phase = LADDER_START;
int frameDelta = 0;
s32 drop_1_start_frame;       // The frame the first drop started on.
s32 drop_2_start_frame;       // The frame the second drop started on.
bool game_paused;

static void* searchLadderFreezard(void* i_actor, void* i_data) {
    const float LADDER_FREEZARD_Y_POS = 950.0f;

    if (fopAcM_GetName(i_actor) == fpcNm_E_FB_e && static_cast<fopAc_ac_c*>(i_actor)->current.pos.y == LADDER_FREEZARD_Y_POS) {
        return i_actor;
    }

    return NULL;
}

void checkOnLadder(daAlink_c* link) {
    u16 current_action = link->mProcID;
    
    if (current_action == daAlink_c::PROC_LADDER_UP_START ||
            current_action == daAlink_c::PROC_LADDER_UP_END ||
            current_action == daAlink_c::PROC_LADDER_DOWN_START ||
            current_action == daAlink_c::PROC_LADDER_DOWN_END ||
            current_action == daAlink_c::PROC_LADDER_MOVE) {
                if (phase == LADDER_START) {
                    phase = LADDER_DURING;
                } else if (phase == LADDER_DROP) {
                    phase = LADDER_DURING_2;
                } else {
                    phase = LADDER_REGRAB;
                }
    } else if (phase != LADDER_DROP) {
        phase = LADDER_START;
    }
}

void checkFirstDrop(daAlink_c* link) {
    if (link->mProcID == daAlink_c::PROC_FALL) {
        if (link->current.pos.y < 247.0f) {
            char msg[32];
            sprintf(msg, "off by -%f", 247.0f - link->current.pos.y);
            FIFOQueue::push(msg, Queue, 0xFFFF0000);
            phase = LADDER_START;
            return;
        }

        if (link->current.pos.y >= 247.0f && link->current.pos.y < 249.8f) {
            FIFOQueue::push("Got it (1/2)", Queue, 0x00FF0000);
            phase = LADDER_DROP;
            drop_1_start_frame = GZ_getFrameCount();
            return;
        }

        if (link->current.pos.y >= 249.8f) {
            char msg[32];
            sprintf(msg, "off by +%f", link->current.pos.y - 249.8f);
            FIFOQueue::push(msg, Queue, 0xFF000000);
            phase = LADDER_START;
            return;
        }
    }
}

void checkSecondDrop(daAlink_c* link) {
    if (link->mProcID == daAlink_c::PROC_FALL) {
        drop_2_start_frame = GZ_getFrameCount();
        s32 delta = drop_2_start_frame - drop_1_start_frame;
#if TPGZ_DEBUG
        OSReport("delta: %d\n", delta);
#endif


        if (delta < 260 && delta >= 255) {
            char msg[32];
            sprintf(msg, "early by %df", (int)(260 - delta));
            FIFOQueue::push(msg, Queue, 0x00FF0000);
            phase = LADDER_DROP_2;
            return;
        }

        if (delta >= 260 && delta < 268) {
            FIFOQueue::push("Got it (2/2)", Queue, 0x00FF0000);
            phase = LADDER_REGRAB;
            return;
        }

        if (delta >= 268 && delta < 273) {
            char msg[32];
            sprintf(msg, "late by %df", (int)(delta - 267));
            FIFOQueue::push(msg, Queue, 0x00FF0000);
            phase = LADDER_DROP_2;
            return;
        }
        
        if (delta >= 273) {
            phase = LADDER_START;
            return;
        }
        
    }
}

void framePauseUpdate() {
    if (dComIfGp_isPauseFlag()) {
#if TPGZ_DEBUG
        OSReport("game paused, incrementing start frame!\n");
#endif

        drop_1_start_frame += 1;
        game_paused = true;
    } else {
        if (game_paused) {
            game_paused = false;
            drop_1_start_frame++;
        }
    }
}

KEEP_FUNC void LFCChecker::execute() {
    // Retrieve player pointer
    daAlink_c* link = (daAlink_c*)dComIfGp_getPlayer(0);

    // Early return if player pointer is invalid
    if (!link) {
        return;
    }

    if (dComIfGp_getEvent()->mEventStatus == true) {
#if TPGZ_DEBUG
        OSReport("In event\n");
#endif
        return;
    }

    if (dComIfGs_getTransformStatus() != TF_STATUS_HUMAN) {
#if TPGZ_DEBUG
        OSReport("Player is not in human form\n");
#endif
        return;
    }

    if (!daAlink_c::checkStageName("D_MN11")) {
#if TPGZ_DEBUG
        OSReport("Player is not in Snowpeak Ruins\n");
#endif
        return;
    }

    if (fopAcM_GetRoomNo(link) != 4) {
#if TPGZ_DEBUG
        OSReport("Player is not in courtyard\n");
#endif
        return;
    }

    fopAc_ac_c* ladder_freezard = (fopAc_ac_c*)fpcEx_Search(searchLadderFreezard, NULL);

    if (!ladder_freezard) {
#if TPGZ_DEBUG
        OSReport("Ladder Freezard not found\n");
#endif
        return;
    }

    switch(phase) {
        case LADDER_START:
            OSReport("Checking for link on ladder 1\n");
            checkOnLadder(link);
            break;
        case LADDER_DURING:
            OSReport("Checking for first drop\n");
            checkFirstDrop(link);
            break;
        case LADDER_DROP:
            OSReport("Checking for link on ladder 2\n");
            checkOnLadder(link);
            framePauseUpdate();
            break;
        case LADDER_DURING_2:
            OSReport("Checking for second drop\n");
            framePauseUpdate();
            checkSecondDrop(link);
            break;
        case LADDER_DROP_2:
            OSReport("Checking for link on ladder 3\n");
            checkOnLadder(link);
            break;
        case LADDER_REGRAB:
            OSReport("Finished!\n");
        default:
            phase = LADDER_START;
            break;
    }
}