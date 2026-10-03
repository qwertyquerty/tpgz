#ifndef TPGZ_MODULES_MENUS_MENU_TOOLS_TIMERS_INCLUDE_TOOLS_TIMERS_MENU_H
#define TPGZ_MODULES_MENUS_MENU_TOOLS_TIMERS_INCLUDE_TOOLS_TIMERS_MENU_H
#include "menus/menu.h"
#include "tools.h"

#ifdef GCN_PLATFORM
#define TIMER_TOGGLE_TEXT "Z+A"
#define TIMER_RESET_TEXT "Z+B"
#endif

#ifdef WII_PLATFORM
#define TIMER_TOGGLE_TEXT "Z+C+A+B"
#define TIMER_RESET_TEXT "Z+C+B+1"
#endif

enum ToolsTimersIndex {
    TIMER_INDEX,
    LOAD_TIMER_INDEX,
    IGT_TIMER_INDEX,

    TOOLS_TIMERS_COUNT
};

class ToolsTimersMenu : public Menu {
public:
    ToolsTimersMenu(Cursor&);
    virtual ~ToolsTimersMenu();
    virtual void draw();

private:

};

#endif
