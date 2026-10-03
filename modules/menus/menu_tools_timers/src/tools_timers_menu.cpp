#include "menus/menu_tools_timers/include/tools_timers_menu.h"
#include <cstdio>
#include "commands.h"
#include "global_data.h"
#include "timer.h"
#include "d/d_com_inf_game.h"
#include "gz_flags.h"
#include "rels/include/defines.h"
#include "menus/utils/menu_mgr.h"

const char l_descTemplates[TOOLS_COUNT][100] = {
    "frame timer: %s to start/stop, %s to reset",
    "loading zone timer: %s to reset",
    "In-game time timer: %s to start/stop, %s to reset",
};

static Line lines[TOOLS_TIMERS_COUNT] = {
    {"timer", TIMER_INDEX,
                 "frame timer: " TIMER_TOGGLE_TEXT " to start/stop, " TIMER_RESET_TEXT " to reset",
                 true, ACTIVE_FUNC(STNG_TOOLS_TIMER)},
                {"load timer", LOAD_TIMER_INDEX, "loading zone timer: " TIMER_RESET_TEXT " to reset",
                 true, ACTIVE_FUNC(STNG_TOOLS_LOAD_TIMER)},
                {"igt timer", IGT_TIMER_INDEX,
                 "In-game time timer: " TIMER_TOGGLE_TEXT " to start/stop, " TIMER_RESET_TEXT
                 " to reset",
                 true, ACTIVE_FUNC(STNG_TOOLS_IGT_TIMER)}
};

KEEP_FUNC ToolsTimersMenu::ToolsTimersMenu(Cursor& cursor)
    : Menu(cursor) {
}

ToolsTimersMenu::~ToolsTimersMenu() {}

GZSettingID l_mapping[] = {
    STNG_TOOLS_TIMER,
    STNG_TOOLS_LOAD_TIMER,
    STNG_TOOLS_IGT_TIMER
};

static void set_active(GZSettingID id, bool status) {
    GZSettingEntry* stng = GZStng_get(id);
    if (stng)
        *(bool*)stng->data = status;
}

void ToolsTimersMenu::draw() {
    if (GZ_getButtonTrig(BACK_BUTTON)) {
        g_menuMgr->pop();
        return;
    }

    if (GZ_getButtonTrig(SELECTION_BUTTON)) {
        GZSettingEntry* stng = NULL;

        stng = GZStng_get(l_mapping[cursor.y]);

        if (!stng) {
            stng = new GZSettingEntry(l_mapping[cursor.y], sizeof(bool), new bool);
            g_settings.push_back(stng);
        }

        if (stng) {
            *(bool*)stng->data = !*(bool*)stng->data;
            
            if (*(bool*)stng->data) {
                switch (cursor.y) {
                case TIMER_INDEX:
                    set_active(STNG_TOOLS_LOAD_TIMER, false);
                    set_active(STNG_TOOLS_IGT_TIMER, false);
                    break;
                case LOAD_TIMER_INDEX:
                    set_active(STNG_TOOLS_TIMER, false);
                    set_active(STNG_TOOLS_IGT_TIMER, false);
                    break;
                case IGT_TIMER_INDEX:
                    set_active(STNG_TOOLS_TIMER, false);
                    set_active(STNG_TOOLS_LOAD_TIMER, false);
                    break;
                }
            }
        }
    }

    char buf[100];
    switch (cursor.y) {
    case IGT_TIMER_INDEX: // fallthrough
    case TIMER_INDEX: {
        uint16_t comboPause =
            GZStng_getData<uint16_t>(STNG_CMD_TIMER_TOGGLE, TIMER_TOGGLE_BUTTONS);
        char* comboPauseStr = new char[GZCmd_getComboLen(comboPause) + 1];
        GZCmd_comboToStr(comboPause, comboPauseStr);
        uint16_t comboAdvance =
            GZStng_getData<uint16_t>(STNG_CMD_TIMER_RESET, TIMER_RESET_BUTTONS);
        char* comboAdvanceStr = new char[GZCmd_getComboLen(comboAdvance) + 1];
        GZCmd_comboToStr(comboAdvance, comboAdvanceStr);
        snprintf(buf, sizeof(buf), l_descTemplates[cursor.y], comboPauseStr, comboAdvanceStr);
        delete[] comboAdvanceStr;
        delete[] comboPauseStr;
        break;
    }
    case LOAD_TIMER_INDEX: {
        uint16_t combo = GZStng_getData<uint16_t>(STNG_CMD_TIMER_RESET, TIMER_RESET_BUTTONS);
        char* comboStr = new char[GZCmd_getComboLen(combo) + 1];
        GZCmd_comboToStr(combo, comboStr);
        snprintf(buf, sizeof(buf), l_descTemplates[cursor.y], comboStr);
        delete[] comboStr;
        break;
    }
    default: {
        snprintf(buf, sizeof(buf), l_descTemplates[cursor.y]);
        break;
    }
    }

    strncpy(lines[cursor.y].description, buf, sizeof(lines[cursor.y].description));

    cursor.move(0, MENU_LINE_NUM);
    GZ_drawMenuLines(lines, cursor.y, MENU_LINE_NUM);
}
