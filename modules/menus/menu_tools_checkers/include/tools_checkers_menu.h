#ifndef TPGZ_MODULES_MENUS_MENU_TOOLS_CHECKERS_INCLUDE_TOOLS_CHECKERS_MENU_H
#define TPGZ_MODULES_MENUS_MENU_TOOLS_CHECKERS_INCLUDE_TOOLS_CHECKERS_MENU_H
#include "menus/menu.h"
#include "tools.h"

#ifdef GCN_PLATFORM
#define GORGE_VOID_TEXT "L+Z"
#define PREVIOUS_GORGE_OPTION Y
#define PREVIOUS_GORGE_OPTION_TEXT "Y"
#define NEXT_GORGE_OPTION X
#define NEXT_GORGE_OPTION_TEXT "X"
#endif

#ifdef WII_PLATFORM
#define GORGE_VOID_TEXT "Z+C+A+1"
#define BACK_IN_TIME_TEXT "Z+C+A+2"
#define PREVIOUS_GORGE_OPTION DPAD_LEFT
#define PREVIOUS_GORGE_OPTION_TEXT "DPad Left"
#define NEXT_GORGE_OPTION DPAD_RIGHT
#define NEXT_GORGE_OPTION_TEXT "DPad Right"

#endif

enum CheckersIndex {
    
#ifdef WII_PLATFORM
    BIT_INDEX,
    SLASH_INDEX,
#endif
    COROTD_INDEX,
    EBMB_INDEX,
    ELEVATOR_ESCAPE_INDEX,
    LFC_INDEX,
    MASH_CHECKER_INDEX,
    ROLL_INDEX,
    UMD_INDEX,
    FAST_EEL_REGRAB_INDEX,
    GORGE_INDEX,

    CHECKERS_COUNT,
};

struct CheckersData {
    uint8_t l_gorge_idx;
};

class CheckersMenu : public Menu {
public:
    CheckersMenu(Cursor&);
    virtual ~CheckersMenu();
    virtual void draw();

private:
};

#endif
