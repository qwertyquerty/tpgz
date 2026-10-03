#include "menus/menu_timer_settings/include/timer_settings_menu.h"
#include "timer.h"
#include "rels/include/defines.h"
#include "menus/utils/menu_mgr.h"

static Line lines[TIMER_SETTINGS_COUNT] = {
    {"reset on practice save", RESET_ON_PRACTICE_SAVE_INDEX,
     "Restart the timer when a practice save finishes loading", true,
     ACTIVE_FUNC(STNG_TIMER_RESET_ON_PRACTICE_SAVE)},
    {"reset on save state", RESET_ON_SAVE_STATE_INDEX, "Restart the timer when a save state is loaded", true,
     ACTIVE_FUNC(STNG_TIMER_RESET_ON_SAVE_STATE)},
    {"display:", DISPLAY_INDEX, "Show the timer in frames, real time, or both", false, NULL,
     TIMER_DISPLAY_COUNT},
};

KEEP_FUNC TimerSettingsMenu::TimerSettingsMenu(Cursor& cursor) : Menu(cursor) {}

TimerSettingsMenu::~TimerSettingsMenu() {}

static void toggleSetting(GZSettingID id) {
    GZSettingEntry* stng = GZStng_get(id);
    if (!stng) {
        stng = new GZSettingEntry(id, sizeof(bool), new bool(false));
        g_settings.push_back(stng);
    }
    *static_cast<bool*>(stng->data) = !*static_cast<bool*>(stng->data);
}

void TimerSettingsMenu::draw() {
    cursor.setMode(Cursor::MODE_LIST);

    if (GZ_getButtonTrig(BACK_BUTTON)) {
        g_menuMgr->pop();
        return;
    }

    if (GZ_getButtonTrig(SELECTION_BUTTON)) {
        switch (cursor.y) {
        case RESET_ON_PRACTICE_SAVE_INDEX:
            toggleSetting(STNG_TIMER_RESET_ON_PRACTICE_SAVE);
            break;
        case RESET_ON_SAVE_STATE_INDEX:
            toggleSetting(STNG_TIMER_RESET_ON_SAVE_STATE);
            break;
        }
    }

    ListMember display_opt[TIMER_DISPLAY_COUNT] = {"both", "frames", "real time"};

    if (cursor.y == DISPLAY_INDEX) {
        GZSettingEntry* stng = GZStng_get(STNG_TIMER_DISPLAY);
        cursor.x = stng ? *static_cast<uint32_t*>(stng->data) : TIMER_DISPLAY_BOTH;
        int prev_x = cursor.x;
        cursor.move(TIMER_DISPLAY_COUNT, MENU_LINE_NUM);

        if (cursor.y == DISPLAY_INDEX && cursor.x != prev_x) {
            if (!stng) {
                stng = new GZSettingEntry(STNG_TIMER_DISPLAY, sizeof(uint32_t), new uint32_t(cursor.x));
                g_settings.push_back(stng);
            } else {
                *static_cast<uint32_t*>(stng->data) = cursor.x;
            }
        }
    } else {
        cursor.move(0, MENU_LINE_NUM);
    }

    uint32_t display = GZStng_getData<uint32_t>(STNG_TIMER_DISPLAY, TIMER_DISPLAY_BOTH);
    lines[DISPLAY_INDEX].printf(" <%s>", display_opt[display < TIMER_DISPLAY_COUNT ? display : 0].member);
    GZ_drawMenuLines(lines, cursor.y, MENU_LINE_NUM);
}
