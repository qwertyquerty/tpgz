#include "tools.h"
#include "d/actor/d_a_alink.h"
#include "rels/include/defines.h"

#include "commands.h"
#include "global_data.h"
#include "settings.h"

int g_tunic_color;

KEEP_FUNC void GZ_handleTools() {
    if (GZStng_getData(STNG_TOOLS_FRAME_ADVANCE, false) && !GZCmd_getCmd(CMD_FRAME_PAUSE)) {
        GZCmd_addCmd(
            new Command(CMD_FRAME_PAUSE,
                        GZStng_getData<uint16_t>(STNG_CMD_FRAME_PAUSE, FRAME_PAUSE_BUTTONS),
                        GZCmd_pauseFrame));
    } else if (!GZStng_getData(STNG_TOOLS_FRAME_ADVANCE, false) &&
               GZCmd_getCmd(CMD_FRAME_PAUSE)) {
        Command* cmd = GZCmd_removeCmd(CMD_FRAME_PAUSE);
        delete cmd;
    }

    if (!(GZStng_getData(STNG_TOOLS_TIMER, false) ||
          GZStng_getData(STNG_TOOLS_IGT_TIMER, false)) &&
        GZCmd_getCmd(CMD_TIMER_TOGGLE)) {
        Command* cmd = GZCmd_removeCmd(CMD_TIMER_TOGGLE);
        delete cmd;
    }

    if (!(GZStng_getData(STNG_TOOLS_TIMER, false) ||
          GZStng_getData(STNG_TOOLS_IGT_TIMER, false) ||
          GZStng_getData(STNG_TOOLS_LOAD_TIMER, false)) &&
        GZCmd_getCmd(CMD_TIMER_RESET)) {
        Command* cmd = GZCmd_removeCmd(CMD_TIMER_RESET);
        delete cmd;
    }

    if ((GZStng_getData(STNG_TOOLS_TIMER, false) ||
         GZStng_getData(STNG_TOOLS_IGT_TIMER, false)) &&
        !GZCmd_getCmd(CMD_TIMER_TOGGLE)) {
        GZCmd_addCmd(new Command(
            CMD_TIMER_TOGGLE,
            GZStng_getData<uint16_t>(STNG_CMD_TIMER_TOGGLE, TIMER_TOGGLE_BUTTONS),
            GZCmd_toggleTimer));
    }

    if ((GZStng_getData(STNG_TOOLS_TIMER, false) ||
         GZStng_getData(STNG_TOOLS_IGT_TIMER, false) ||
         GZStng_getData(STNG_TOOLS_LOAD_TIMER, false)) &&
        !GZCmd_getCmd(CMD_TIMER_RESET)) {
        GZCmd_addCmd(
            new Command(CMD_TIMER_RESET,
                        GZStng_getData<uint16_t>(STNG_CMD_TIMER_RESET, TIMER_RESET_BUTTONS),
                        GZCmd_resetTimer));
    }

    if (GZStng_getData(STNG_TOOLS_TELEPORT, false || GZStng_getData(STNG_TOOLS_DISPLACEMENT, false)) && !GZCmd_getCmd(CMD_STORE_POSITION)) {
        GZCmd_addCmd(new Command(
            CMD_STORE_POSITION,
            GZStng_getData<uint16_t>(STNG_CMD_STORE_POSITION, STORE_POSITION_BUTTONS),
            GZCmd_storePosition));
    } else if (!GZStng_getData(STNG_TOOLS_TELEPORT, false) && !GZStng_getData(STNG_TOOLS_DISPLACEMENT, false) &&
               GZCmd_getCmd(CMD_STORE_POSITION)) {
        Command* cmd = GZCmd_removeCmd(CMD_STORE_POSITION);
        delete cmd;
    }

    // logic to prevent save states and teleport from conflicting
    bool loadPositionActive =
        GZStng_getData(STNG_TOOLS_TELEPORT, false) && !GZStng_getData(STNG_TOOLS_SAVE_STATES, false);
    if (loadPositionActive && !GZCmd_getCmd(CMD_LOAD_POSITION)) {
        GZCmd_addCmd(new Command(
            CMD_LOAD_POSITION,
            GZStng_getData<uint16_t>(STNG_CMD_LOAD_POSITION, LOAD_POSITION_BUTTONS),
            GZCmd_loadPosition));
    } else if (!loadPositionActive && GZCmd_getCmd(CMD_LOAD_POSITION)) {
        Command* cmd = GZCmd_removeCmd(CMD_LOAD_POSITION);
        delete cmd;
    }

    if (GZStng_getData(STNG_TOOLS_RELOAD_AREA, false) && !GZCmd_getCmd(CMD_RELOAD_AREA)) {
        GZCmd_addCmd(
            new Command(CMD_RELOAD_AREA,
                        GZStng_getData<uint16_t>(STNG_CMD_RELOAD_AREA, RELOAD_AREA_BUTTONS),
                        GZCmd_reloadArea));
    } else if (!GZStng_getData(STNG_TOOLS_RELOAD_AREA, false) &&
               GZCmd_getCmd(CMD_RELOAD_AREA)) {
        Command* cmd = GZCmd_removeCmd(CMD_RELOAD_AREA);
        delete cmd;
    }

    if (GZStng_getData(STNG_TOOLS_FREE_CAM, false) && !GZCmd_getCmd(CMD_FREE_CAM)) {
        GZCmd_addCmd(new Command(
            CMD_FREE_CAM, GZStng_getData<uint16_t>(STNG_CMD_FREE_CAM, FREE_CAM_BUTTONS),
            GZCmd_toggleFreeCam));
        g_freeCamEnabled = false;
    } else if (!GZStng_getData(STNG_TOOLS_FREE_CAM, false) && GZCmd_getCmd(CMD_FREE_CAM)) {
        Command* cmd = GZCmd_removeCmd(CMD_FREE_CAM);
        delete cmd;
        g_freeCamEnabled = false;
    }

    if (GZStng_getData(STNG_TOOLS_MOVE_LINK, false) && !GZCmd_getCmd(CMD_MOVE_LINK)) {
        GZCmd_addCmd(new Command(
            CMD_MOVE_LINK, GZStng_getData<uint16_t>(STNG_CMD_MOVE_LINK, MOVE_LINK_BUTTONS),
            GZCmd_toggleMoveLink));
        g_moveLinkEnabled = false;
    } else if (!GZStng_getData(STNG_TOOLS_MOVE_LINK, false) && GZCmd_getCmd(CMD_MOVE_LINK)) {
        Command* cmd = GZCmd_removeCmd(CMD_MOVE_LINK);
        delete cmd;
        g_moveLinkEnabled = false;
    }

    if (GZStng_getData(STNG_TOOLS_FAST_MOVEMENT, false)) {
        const_cast<daAlinkHIO_frontRoll_c1&>(daAlinkHIO_frontRoll_c0::m).mSpeedRate = 3.0f;
        const_cast<daAlinkHIO_swim_c1&>(daAlinkHIO_swim_c0::m).mUnderwaterMaxSpeed = 50;
        const_cast<daAlinkHIO_swim_c1&>(daAlinkHIO_swim_c0::m).mBackwardMaxSpeed = 50;
        const_cast<daAlinkHIO_swim_c1&>(daAlinkHIO_swim_c0::m).mStrafeMaxSpeed = 50;
        const_cast<daAlinkHIO_swim_c1&>(daAlinkHIO_swim_c0::m).mDashMaxSpeed = 50;
        const_cast<daAlinkHIO_swim_c1&>(daAlinkHIO_swim_c0::m).mForwardMaxSpeed = 50;
        const_cast<daAlinkHIO_swim_c1&>(daAlinkHIO_swim_c0::m).mUnderwaterFallMaxSpeed = 50;
        const_cast<daAlinkHIO_swim_c1&>(daAlinkHIO_swim_c0::m).mBootsMaxFallSpeed = -50;
        const_cast<daAlinkHIO_swim_c1&>(daAlinkHIO_swim_c0::m).mBootsGravity = -50;
        const_cast<daAlinkHIO_wlMove_c1&>(daAlinkHIO_wlMove_c0::m).mADashInitSpeed = 100;
        const_cast<daAlinkHIO_wlMove_c1&>(daAlinkHIO_wlMove_c0::m).mADashMaxSpeed = 100;
        const_cast<daAlinkHIO_wlMove_c1&>(daAlinkHIO_wlMove_c0::m).mADashInitSpeedSlow = 70;
        const_cast<daAlinkHIO_wlMove_c1&>(daAlinkHIO_wlMove_c0::m).mADashMaxSpeedSlow = 70;
        const_cast<daAlinkHIO_wlSwim_c1&>(daAlinkHIO_wlSwim_c0::m).mMaxSpeed = 50;
        const_cast<daAlinkHIO_wlSwim_c1&>(daAlinkHIO_wlSwim_c0::m).mMaxSpeedWeak = 50;
    } else {
        const_cast<daAlinkHIO_frontRoll_c1&>(daAlinkHIO_frontRoll_c0::m).mSpeedRate = 1.3;
        const_cast<daAlinkHIO_swim_c1&>(daAlinkHIO_swim_c0::m).mUnderwaterMaxSpeed = 12;
        const_cast<daAlinkHIO_swim_c1&>(daAlinkHIO_swim_c0::m).mForwardMaxSpeed = 8;
        const_cast<daAlinkHIO_swim_c1&>(daAlinkHIO_swim_c0::m).mBackwardMaxSpeed = 6;
        const_cast<daAlinkHIO_swim_c1&>(daAlinkHIO_swim_c0::m).mStrafeMaxSpeed = 8;
        const_cast<daAlinkHIO_swim_c1&>(daAlinkHIO_swim_c0::m).mDashMaxSpeed = 13;
        const_cast<daAlinkHIO_swim_c1&>(daAlinkHIO_swim_c0::m).mUnderwaterFallMaxSpeed = 8;
        const_cast<daAlinkHIO_swim_c1&>(daAlinkHIO_swim_c0::m).mBootsMaxFallSpeed = -20;
        const_cast<daAlinkHIO_swim_c1&>(daAlinkHIO_swim_c0::m).mBootsGravity = -0.699999988;
        const_cast<daAlinkHIO_wlMove_c1&>(daAlinkHIO_wlMove_c0::m).mADashInitSpeed = 65;
        const_cast<daAlinkHIO_wlMove_c1&>(daAlinkHIO_wlMove_c0::m).mADashMaxSpeed = 45;
        const_cast<daAlinkHIO_wlMove_c1&>(daAlinkHIO_wlMove_c0::m).mADashInitSpeedSlow = 35;
        const_cast<daAlinkHIO_wlMove_c1&>(daAlinkHIO_wlMove_c0::m).mADashMaxSpeedSlow = 33;
        const_cast<daAlinkHIO_wlSwim_c1&>(daAlinkHIO_wlSwim_c0::m).mMaxSpeed = 20;
        const_cast<daAlinkHIO_wlSwim_c1&>(daAlinkHIO_wlSwim_c0::m).mMaxSpeedWeak = 9;
    }

    if (GZStng_getData(STNG_TOOLS_FAST_BONK, false)) {
        const_cast<daAlinkHIO_frontRoll_c1&>(daAlinkHIO_frontRoll_c0::m).mCrashAnm.mStartFrame = 50.0f;
        const_cast<daAlinkHIO_frontRoll_c1&>(daAlinkHIO_frontRoll_c0::m).mCrashAnm.mSpeed = 0.0f;
    } else {
        const_cast<daAlinkHIO_frontRoll_c1&>(daAlinkHIO_frontRoll_c0::m).mCrashAnm.mStartFrame = 3.0f;
        const_cast<daAlinkHIO_frontRoll_c1&>(daAlinkHIO_frontRoll_c0::m).mCrashAnm.mSpeed = 0.8f;
    }
}
