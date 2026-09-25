#include "defines.h"
#include "menus/menu_tools/include/tools_menu.h"
#include "menus/utils/menu_mgr.h"

static Line lines[TOOLS_COUNT] = {
    {"checkers", CHECKERS_INDEX, "Various checker tools", false},
                {"controller", CONTROLLER_INDEX, "Controller related tools", false},
                {"link", LINK_INDEX, "Link related tools", false},
                {"scene", SCENE_INDEX, "Scene related tools", false},
                {"timers", TIMERS_INDEX, "Various timer tools", false},
                {"rng", RNG_INDEX, "Tools for modifying RNG values", false},
};

KEEP_FUNC ToolsMenu::ToolsMenu(Cursor& cursor)
    : Menu(cursor) {
}

ToolsMenu::~ToolsMenu() {}

void ToolsMenu::draw() {
    if (GZ_getButtonTrig(BACK_BUTTON)) {
        g_menuMgr->pop();
        return;
    }

    if (GZ_getButtonTrig(SELECTION_BUTTON)) {
        switch (cursor.y) {
        case CHECKERS_INDEX:
            g_menuMgr->push(MN_TOOLS_CHECKERS_INDEX);
            return;
        case CONTROLLER_INDEX:
            g_menuMgr->push(MN_TOOLS_CONTROLLER_INDEX);
            return;
        case LINK_INDEX:
            g_menuMgr->push(MN_TOOLS_LINK_INDEX);
            return;
        case SCENE_INDEX:
            g_menuMgr->push(MN_TOOLS_SCENE_INDEX);
            return;
        case TIMERS_INDEX:
            g_menuMgr->push(MN_TOOLS_TIMERS_INDEX);
            return;
        case RNG_INDEX:
            g_menuMgr->push(MN_TOOLS_RNG_INDEX);
            return;
        }
    }

    cursor.move(0, MENU_LINE_NUM);
    GZ_drawMenuLines(lines, cursor.y, MENU_LINE_NUM);
}
