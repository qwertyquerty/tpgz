#include "timer.h"
#include <cstdio>
#include "font.h"
#include "pos_settings.h"
#include "settings.h"
#include "tools.h"
#include "SSystem/SComponent/c_counter.h"
#include "f_op/f_op_scene_req.h"
#include "rels/include/defines.h"
#include "game_state.h"
#include "d/d_com_inf_game.h"

static bool l_restartTimer = false;

KEEP_FUNC void Timer::restartOnLoad(GZSettingID trigger) {
    if (GZStng_getData(STNG_TOOLS_TIMER, false) && GZStng_getData(trigger, false)) {
        l_restartTimer = true;
    }
}

KEEP_FUNC void Timer::drawTimer() {
    static bool init_start_time = false;
    static OSTime timer = 0;
    static OSTime start_time = 0;
    static int frame_timer = 0;

    if (!GZStng_getData(STNG_TOOLS_TIMER, false)) {
        init_start_time = false;
        l_restartTimer = false;
        return;
    }

    if (l_restartTimer) {
        timer = 0;
        frame_timer = 0;
        init_start_time = false;
        g_timerEnabled = !l_fopScnRq_IsUsingOfOverlap && dComIfGp_getPlayer(0) != NULL;
        l_restartTimer = !g_timerEnabled;
    }

    if (g_timerEnabled) {
        if (!init_start_time) {
            start_time = OSGetTime();
            init_start_time = true;
        }

        timer = (OSGetTime() - start_time);
        frame_timer++;
    }

    if (g_resetTimer) {
        timer = 0;
        start_time = 0;
        frame_timer = 0;
        init_start_time = false;
        g_resetTimer = false;
        g_timerEnabled = false;
    }

    OSCalendarTime ctime;
    OSTicksToCalendarTime(timer, &ctime);
    char timerF[12];
    char timerS[16];
    snprintf(timerF, sizeof(timerF), "%d", frame_timer);
    snprintf(timerS, sizeof(timerS), "%02d:%02d:%02d.%03d", ctime.hour, ctime.min,
             ctime.sec, ctime.msec);

    uint32_t display = GZStng_getData<uint32_t>(STNG_TIMER_DISPLAY, TIMER_DISPLAY_BOTH);
    Vec2 spriteOffset = GZ_getSpriteOffset(STNG_SPRITES_TIMER_SPR);
    float y = spriteOffset.y;
    if (display != TIMER_DISPLAY_REAL_TIME) {
        Font::GZ_drawStr(timerF, spriteOffset.x, y, 0xFFFFFFFF, GZ_checkDropShadows());
        y += 15.0f;
    }
    if (display != TIMER_DISPLAY_FRAMES) {
        Font::GZ_drawStr(timerS, spriteOffset.x, y, 0xFFFFFFFF, GZ_checkDropShadows());
    }
}

KEEP_FUNC void Timer::drawIGT() {
    static bool init_start_time = false;
    static bool init_load_starttime = false;
    static OSTime timer = 0;
    static OSTime start_time = 0;

    static OSTime load_start_time = 0;
    static OSTime load_total_time = 0;
    static OSTime load_timer = 0;

    static OSCalendarTime ctime;
    static OSCalendarTime load_ctime;

    if (!GZStng_getData(STNG_TOOLS_IGT_TIMER, false)) {
        init_start_time = false;
        return;
    }

    if (g_timerEnabled) {
        if (!init_start_time) {
            start_time = OSGetTime();
            init_start_time = true;
        }

        if (l_fopScnRq_IsUsingOfOverlap) {
            if (!init_load_starttime) {
                load_start_time = OSGetTime();
                init_load_starttime = true;
            }

            if (init_load_starttime) {
                load_timer = OSGetTime() - load_start_time;
            }

            OSTicksToCalendarTime(load_timer, &load_ctime);
        } else {
            init_load_starttime = false;
            load_total_time += load_timer;
            load_timer = 0;

            timer = (OSGetTime() - start_time) - load_total_time;
            OSTicksToCalendarTime(timer, &ctime);
        }
    }

    if (g_resetTimer) {
        timer = 0;
        start_time = 0;
        ctime.hour = 0;
        ctime.min = 0;
        ctime.sec = 0;
        ctime.msec = 0;
        load_total_time = 0;
        init_start_time = false;
        g_resetTimer = false;
        g_timerEnabled = false;
    }

    char buf[16] = {0};
    snprintf(buf, sizeof(buf), "%02d:%02d:%02d.%03d", ctime.hour, ctime.min, ctime.sec,
             ctime.msec);
    Vec2 spriteOffset = GZ_getSpriteOffset(STNG_SPRITES_IGT_TIMER_SPR);
    Font::GZ_drawStr(buf, spriteOffset.x, spriteOffset.y, 0xFFFFFFFF, GZ_checkDropShadows());
}

KEEP_FUNC void Timer::drawLoadTimer() {
    static bool init_load_starttime = false;

    static OSTime load_start_time = 0;
    static OSTime load_total_time = 0;
    static OSTime load_timer = 0;

    static OSCalendarTime load_ctime;

    if (!GZStng_getData(STNG_TOOLS_LOAD_TIMER, false)) {
        return;
    }

    if (l_fopScnRq_IsUsingOfOverlap) {
        if (!init_load_starttime) {
            load_start_time = OSGetTime();
            init_load_starttime = true;
        }

        if (init_load_starttime) {
            load_timer = OSGetTime() - load_start_time;
        }
    } else {
        init_load_starttime = false;
        load_total_time += load_timer;
        load_timer = 0;

        OSTicksToCalendarTime(load_total_time, &load_ctime);
    }

    if (g_resetTimer) {
        load_ctime.hour = 0;
        load_ctime.min = 0;
        load_ctime.sec = 0;
        load_ctime.msec = 0;
        load_total_time = 0;
        g_resetTimer = false;
        g_timerEnabled = false;
    }

    char buf[16] = {0};
    snprintf(buf, sizeof(buf), "%02d:%02d:%02d.%03d", load_ctime.hour, load_ctime.min,
             load_ctime.sec, load_ctime.msec);
    Vec2 spriteOffset = GZ_getSpriteOffset(STNG_SPRITES_LOAD_TIMER_SPR);
    Font::GZ_drawStr(buf, spriteOffset.x, spriteOffset.y, 0xFFFFFFFF, GZ_checkDropShadows());
}
