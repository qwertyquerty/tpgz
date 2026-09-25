#ifndef TPGZ_MODULES_MENUS_MENU_TOOLS_SCENE_INCLUDE_TOOLS_SCENE_MENU_H
#define TPGZ_MODULES_MENUS_MENU_TOOLS_SCENE_INCLUDE_TOOLS_SCENE_MENU_H
#include "menus/menu.h"
#include "tools.h"

#ifdef GCN_PLATFORM
#define FRAME_ADVANCE_TEXT "R + D-Pad Left"
#define FRAME_PAUSE_TEXT "R"
#define RELOAD_AREA_TEXT "L+R+A+Start"
#define FREE_CAM_TEXT "Z+B+A"
#define SAVE_STATE_TEXT "L+R+D-Pad Left"
#define LOAD_STATE_TEXT "L+R+D-Pad Right"
#endif

#ifdef WII_PLATFORM
#define FRAME_ADVANCE_TEXT "Z+C+Plus+Minus"
#define FRAME_PAUSE_TEXT "2"
#define RELOAD_AREA_TEXT "Z+C+B+2"
#define FREE_CAM_TEXT "Z+C+B+Minus"
#define SAVE_STATE_TEXT "Z+C+D-Pad Left"
#define LOAD_STATE_TEXT "Z+C+D-Pad Right"
#endif

enum ToolsSceneIndex {
    AREA_RELOAD_INDEX,
    FRAME_ADVANCE_INDEX,
    FREE_CAM_INDEX,
    HEAP_DEBUG_INDEX,
    SAVE_STATES_INDEX,

    // Entry used as a counter
    TOOLS_SCENE_COUNT
};

class ToolsSceneMenu : public Menu {
public:
    ToolsSceneMenu(Cursor&);
    virtual ~ToolsSceneMenu();
    virtual void draw();

private:
};

#endif
