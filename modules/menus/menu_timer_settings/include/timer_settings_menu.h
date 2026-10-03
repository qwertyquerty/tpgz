#ifndef TPGZ_MODULES_MENUS_MENU_TIMER_SETTINGS_INCLUDE_TIMER_SETTINGS_MENU_H
#define TPGZ_MODULES_MENUS_MENU_TIMER_SETTINGS_INCLUDE_TIMER_SETTINGS_MENU_H
#include "menus/menu.h"
#include "settings.h"

enum TimerSettingsIndex {
    RESET_ON_PRACTICE_SAVE_INDEX,
    RESET_ON_SAVE_STATE_INDEX,
    DISPLAY_INDEX,

    TIMER_SETTINGS_COUNT
};

class TimerSettingsMenu : public Menu {
public:
    TimerSettingsMenu(Cursor&);
    virtual ~TimerSettingsMenu();
    virtual void draw();
};

#endif
